#!/usr/bin/env texlua

if not modules then modules = { } end modules ['mtxrun'] = {
    version   = 1.001,
    comment   = "runner, lua replacement for texmfstart.rb",
    author    = "Hans Hagen, PRAGMA-ADE, Hasselt NL",
    copyright = "PRAGMA ADE / ConTeXt Development Team",
    license   = "see context related readme files"
}

-- one can make a stub:

-- mtxrun :
--
-- #!/bin/sh
-- env LUATEXDIR=/....../texmf/scripts/context/lua luatex --luaonly mtxrun.lua "$@"

-- mtxrun.cmd :
--
-- @luatex --luaonly %~d0%~p0mtxrun.lua %*

-- filename : mtxrun.lua
-- comment  : companion to context.tex
-- author   : Hans Hagen, PRAGMA-ADE, Hasselt NL
-- copyright: PRAGMA ADE / ConTeXt Development Team
-- license  : see context related readme files

-- This script is based on texmfstart.rb but does not use kpsewhich to locate files.
-- Although kpse is a library it never came to opening up its interface to other
-- programs (esp scripting languages) and so we do it ourselves. The lua variant
-- evolved out of an experimental ruby one. Interesting is that using a scripting
-- language instead of c does not have a speed penalty. Actually the lua variant is
-- more efficient, especially when multiple calls to kpsewhich are involved. The lua
-- library also gives way more control.

-- When libraries used here are updates you can run
--
--   mtxrun --selfmerge
--
-- to update the embedded code. After that you might need to run
--
--   mtxrun --selfupdate
--
-- to copy the new script (from scripts/context/lua) to location where
-- binaries are expected. If you want to remove the embedded code you can run
--
--   mtxxun --selfclean

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

package.loaded["l-bit32"] = package.loaded["l-bit32"] or true

-- original size: 3607, stripped down to: 3009

if not modules then modules={} end modules ['l-bit32']={
 version=1.001,
 license="the same as regular Lua",
 source="bitwise.lua, v 1.24 2014/12/26 17:20:53 roberto",
 comment="drop-in for bit32, adapted a bit by Hans Hagen",
}
if bit32 then
elseif utf8 then
 load ([[
local select = select -- instead of: arg = { ... }
bit32 = {
  bnot = function (a)
    return ~a & 0xFFFFFFFF
  end,
  band = function (x, y, z, ...)
    if not z then
      return ((x or -1) & (y or -1)) & 0xFFFFFFFF
    else
      local res = x & y & z
      for i=1,select("#",...) do
        res = res & select(i,...)
      end
      return res & 0xFFFFFFFF
    end
  end,
  bor = function (x, y, z, ...)
    if not z then
      return ((x or 0) | (y or 0)) & 0xFFFFFFFF
    else
      local res = x | y | z
      for i=1,select("#",...) do
        res = res | select(i,...)
      end
      return res & 0xFFFFFFFF
    end
  end,
  bxor = function (x, y, z, ...)
    if not z then
      return ((x or 0) ~ (y or 0)) & 0xFFFFFFFF
    else
      local res = x ~ y ~ z
      for i=1,select("#",...) do
        res = res ~ select(i,...)
      end
      return res & 0xFFFFFFFF
    end
  end,
  btest = function (x, y, z, ...)
    if not z then
      return (((x or -1) & (y or -1)) & 0xFFFFFFFF) ~= 0
    else
      local res = x & y & z
      for i=1,select("#",...) do
          res = res & select(i,...)
      end
      return (res & 0xFFFFFFFF) ~= 0
    end
  end,
  lshift = function (a, b)
    return ((a & 0xFFFFFFFF) << b) & 0xFFFFFFFF
  end,
  rshift = function (a, b)
    return ((a & 0xFFFFFFFF) >> b) & 0xFFFFFFFF
  end,
  arshift = function (a, b)
    a = a & 0xFFFFFFFF
    if b <= 0 or (a & 0x80000000) == 0 then
      return (a >> b) & 0xFFFFFFFF
    else
      return ((a >> b) | ~(0xFFFFFFFF >> b)) & 0xFFFFFFFF
    end
  end,
  lrotate = function (a ,b)
    b = b & 31
    a = a & 0xFFFFFFFF
    a = (a << b) | (a >> (32 - b))
    return a & 0xFFFFFFFF
  end,
  rrotate = function (a, b)
    b = -b & 31
    a = a & 0xFFFFFFFF
    a = (a << b) | (a >> (32 - b))
    return a & 0xFFFFFFFF
  end,
  extract = function (a, f, w)
    return (a >> f) & ~(-1 << (w or 1))
  end,
  replace = function (a, v, f, w)
    local mask = ~(-1 << (w or 1))
    return ((a & ~(mask << f)) | ((v & mask) << f)) & 0xFFFFFFFF
  end,
}
        ]] ) ()
elseif bit then
 load ([[
local band, bnot, rshift, lshift = bit.band, bit.bnot, bit.rshift, bit.lshift
bit32 = {
  arshift = bit.arshift,
  band    = band,
  bnot    = bnot,
  bor     = bit.bor,
  bxor    = bit.bxor,
  btest   = function(...)
    return band(...) ~= 0
  end,
  extract = function(a,f,w)
    return band(rshift(a,f),2^(w or 1)-1)
  end,
  lrotate = bit.rol,
  lshift  = lshift,
  replace = function(a,v,f,w)
    local mask = 2^(w or 1)-1
    return band(a,bnot(lshift(mask,f)))+lshift(band(v,mask),f)
  end,
  rrotate = bit.ror,
  rshift  = rshift,
}
        ]] ) ()
else
 xpcall(function() local _,t=require("bit32") if t then bit32=t end return end,function() end)
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-lua"] = package.loaded["l-lua"] or true

-- original size: 6529, stripped down to: 2933

if not modules then modules={} end modules ['l-lua']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local next,type,tonumber=next,type,tonumber
LUAMAJORVERSION,LUAMINORVERSION=string.match(_VERSION,"^[^%d]+(%d+)%.(%d+).*$")
LUAMAJORVERSION=tonumber(LUAMAJORVERSION) or 5
LUAMINORVERSION=tonumber(LUAMINORVERSION) or 1
LUAVERSION=LUAMAJORVERSION+LUAMINORVERSION/10
if LUAVERSION<5.2 and jit then
 MINORVERSION=2
 LUAVERSION=5.2
end
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
function setinspector(kind,inspector) 
 inspectors[kind]=inspector
end
function inspect(...) 
 for s=1,select("#",...) do
  local value=select(s,...)
  if value==nil then
   print("nil")
  else
   local done=false
   local kind=type(value)
   local inspector=inspectors[kind]
   if inspector then
    done=inspector(value)
    if done then
     break
    end
   end
   for kind,inspector in next,inspectors do
    done=inspector(value)
    if done then
     break
    end
   end
   if not done then
    print(tostring(value))
   end
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
if lua then
 lua.mask=load([[τεχ = 1]]) and "utf" or "ascii"
end
local flush=io.flush
if flush then
 local execute=os.execute if execute then function os.execute(...) flush() return execute(...) end end
 local exec=os.exec if exec then function os.exec   (...) flush() return exec   (...) end end
 local spawn=os.spawn   if spawn   then function os.spawn  (...) flush() return spawn  (...) end end
 local popen=io.popen   if popen   then function io.popen  (...) flush() return popen  (...) end end
end
FFISUPPORTED=type(ffi)=="table" and ffi.os~="" and ffi.arch~="" and ffi.load
if not FFISUPPORTED then
 local okay;okay,ffi=pcall(require,"ffi")
 FFISUPPORTED=type(ffi)=="table" and ffi.os~="" and ffi.arch~="" and ffi.load
end
if not FFISUPPORTED then
 ffi=nil
elseif not ffi.number then
 ffi.number=tonumber
end
if LUAVERSION>5.3 then
end
if status and os.setenv then
 os.setenv("engine",string.lower(status.luatex_engine or "unknown"))
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-macro"] = package.loaded["l-macro"] or true

-- original size: 10130, stripped down to: 5990

if not modules then modules={} end modules ['l-macros']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local S,P,R,V,C,Cs,Cc,Ct,Carg=lpeg.S,lpeg.P,lpeg.R,lpeg.V,lpeg.C,lpeg.Cs,lpeg.Cc,lpeg.Ct,lpeg.Carg
local lpegmatch=lpeg.match
local concat=table.concat
local format,sub,match=string.format,string.sub,string.match
local next,load,type=next,load,type
local newline=S("\n\r")^1
local continue=P("\\")*newline
local whitespace=S(" \t\n\r")
local spaces=S(" \t")+continue
local nametoken=R("az","AZ","__","09")
local name=nametoken^1
local body=((continue/""+1)-newline)^1
local lparent=P("(")
local rparent=P(")")
local noparent=1-(lparent+rparent)
local nested=P { lparent*(noparent+V(1))^0*rparent }
local escaped=P("\\")*P(1)
local squote=P("'")
local dquote=P('"')
local quoted=dquote*(escaped+(1-dquote))^0*dquote+squote*(escaped+(1-squote))^0*squote
local arguments=lparent*Ct((Cs((nested+(quoted+1-S("),")))^1)+S(", "))^0)*rparent
local macros=lua.macros or {}
lua.macros=macros
local patterns={}
local definitions={}
local resolve
local subparser
local report_lua=function(...)
 if logs and logs.reporter then
  report_lua=logs.reporter("system","lua")
  report_lua(...)
 else
  print(format(...))
 end
end
local safeguard=P("local")*whitespace^1*name*(whitespace+P("="))
resolve=safeguard+C(C(name)*(arguments^-1))/function(raw,s,a)
 local d=definitions[s]
 if d then
  if a then
   local n=#a
   local p=patterns[s][n]
   if p then
    local d=d[n]
    for i=1,n do
     a[i]=lpegmatch(subparser,a[i]) or a[i]
    end
    return lpegmatch(p,d,1,a) or d
   else
    return raw
   end
  else
   return d[0] or raw
  end
 elseif a then
  for i=1,#a do
   a[i]=lpegmatch(subparser,a[i]) or a[i]
  end
  return s.."("..concat(a,",")..")"
 else
  return raw
 end
end
subparser=Cs((resolve+P(1))^1)
local enddefine=P("#enddefine")/""
local beginregister=(C(name)*(arguments+Cc(false))*C((1-enddefine)^1)*enddefine)/function(k,a,v)
 local n=0
 if a then
  n=#a
  local pattern=P(false)
  for i=1,n do
   pattern=pattern+(P(a[i])*Carg(1))/function(t) return t[i] end
  end
  pattern=Cs((pattern+P(1))^1)
  local p=patterns[k]
  if not p then
   p={ [0]=false,false,false,false,false,false,false,false,false }
   patterns[k]=p
  end
  p[n]=pattern
 end
 local d=definitions[k]
 if not d then
  d={ a=a,[0]=false,false,false,false,false,false,false,false,false }
  definitions[k]=d
 end
 d[n]=lpegmatch(subparser,v) or v
 return ""
end
local register=(Cs(name)*(arguments+Cc(false))*spaces^0*Cs(body))/function(k,a,v)
 local n=0
 if a then
  n=#a
  local pattern=P(false)
  for i=1,n do
   pattern=pattern+(P(a[i])*Carg(1))/function(t) return t[i] end
  end
  pattern=Cs((pattern+P(1))^1)
  local p=patterns[k]
  if not p then
   p={ [0]=false,false,false,false,false,false,false,false,false }
   patterns[k]=p
  end
  p[n]=pattern
 end
 local d=definitions[k]
 if not d then
  d={ a=a,[0]=false,false,false,false,false,false,false,false,false }
  definitions[k]=d
 end
 d[n]=lpegmatch(subparser,v) or v
 return ""
end
local unregister=(C(name)*spaces^0*(arguments+Cc(false)))/function(k,a)
 local n=0
 if a then
  n=#a
  local p=patterns[k]
  if p then
   p[n]=false
  end
 end
 local d=definitions[k]
 if d then
  d[n]=false
 end
 return ""
end
local begindefine=(P("begindefine")*spaces^0/"")*beginregister
local define=(P("define"  )*spaces^0/"")*register
local undefine=(P("undefine"   )*spaces^0/"")*unregister
local parser=Cs((((P("#")/"")*(define+begindefine+undefine)*(newline^0/"") )+resolve+P(1) )^0 )
function macros.reset()
 definitions={}
 patterns={}
end
function macros.showdefinitions()
 for name,list in table.sortedhash(definitions) do
  local arguments=list.a
  if arguments then
   arguments="("..concat(arguments,",")..")"
  else
   arguments=""
  end
  print("macro: "..name..arguments)
  for i=0,#list do
   local l=list[i]
   if l then
    print("  "..l)
   end
  end
 end
end
function macros.resolvestring(str)
 return lpegmatch(parser,str) or str
end
function macros.resolving()
 return next(patterns)
end
local function reload(path,name,data)
 local only=match(name,".-([^/]+)%.lua")
 if only and only~="" then
  local name=path.."/"..only
  local f=io.open(name,"wb")
  f:write(data)
  f:close()
  local f=loadfile(name)
  os.remove(name)
  return f
 end
end
local function reload(path,name,data)
 if path and path~="" then
  local only=string.match(name,".-([^/]+)%.lua")
  if only and only~="" then
   local name=path.."/"..only.."-macro.lua"
   local f=io.open(name,"wb")
   if f then
    f:write(data)
    f:close()
    local l=loadfile(name)
    os.remove(name)
    return l
   end
  end
 end
 return load(data,name)
end
local function loaded(name,trace,detail)
 local f=io.open(name,"rb")
 if not f then
  return false,format("file '%s' not found",name)
 end
 local c=f:read("*a")
 if not c then
  return false,format("file '%s' is invalid",name)
 end
 f:close()
 local n=lpegmatch(parser,c)
 if trace then
  if #n~=#c then
   report_lua("macros expanded in '%s' (%i => %i bytes)",name,#c,#n)
   if detail then
    report_lua()
    report_lua(n)
    report_lua()
   end
  elseif detail then
   report_lua("no macros expanded in '%s'",name)
  end
 end
 return reload(lfs and lfs.currentdir(),name,n)
end
macros.loaded=loaded
function required(name,trace)
 local filename=file.addsuffix(name,"lua")
 local fullname=resolvers and resolvers.findfile(filename) or filename
 if not fullname or fullname=="" then
  return false
 end
 local codeblob=package.loaded[fullname]
 if codeblob then
  return codeblob
 end
 local code,message=loaded(fullname,macros,trace,trace)
 if type(code)=="function" then
  code=code()
 else
  report_lua("error when loading '%s'",fullname)
  return false,message
 end
 if code==nil then
  code=false
 end
 package.loaded[fullname]=code
 return code
end
macros.required=required


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-sandbox"] = package.loaded["l-sandbox"] or true

-- original size: 9604, stripped down to: 6394

if not modules then modules={} end modules ['l-sandbox']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local global=_G
local next=next
local unpack=unpack or table.unpack
local type=type
local tprint=texio and texio.write_nl or print
local tostring=tostring
local format=string.format 
local concat=table.concat
local sort=table.sort
local gmatch=string.gmatch
local gsub=string.gsub
local requiem=require
sandbox={}
local sandboxed=false
local overloads={}
local skiploads={}
local initializers={}
local finalizers={}
local originals={}
local comments={}
local trace=false
local logger=false
local blocked={}
local function report(...)
 tprint("sandbox         ! "..format(...)) 
end
sandbox.report=report
function sandbox.setreporter(r)
 report=r
 sandbox.report=r
end
function sandbox.settrace(v)
 trace=v
end
function sandbox.setlogger(l)
 logger=type(l)=="function" and l or false
end
local function register(func,overload,comment)
 if type(func)=="function" then
  if type(overload)=="string" then
   comment=overload
   overload=nil
  end
  local function f(...)
   if sandboxed then
    local overload=overloads[f]
    if overload then
     if logger then
      local result={ overload(func,...) }
      logger {
       comment=comments[f] or tostring(f),
       arguments={... },
       result=result[1] and true or false,
      }
      return unpack(result)
     else
      return overload(func,...)
     end
    else
    end
   else
    return func(...)
   end
  end
  if comment then
   comments[f]=comment
   if trace then
    report("registering function: %s",comment)
   end
  end
  overloads[f]=overload or false
  originals[f]=func
  return f
 end
end
local function redefine(func,comment)
 if type(func)=="function" then
  skiploads[func]=comment or comments[func] or "unknown"
  if overloads[func]==false then
   overloads[func]=nil 
  end
 end
end
sandbox.register=register
sandbox.redefine=redefine
function sandbox.original(func)
 return originals and originals[func] or func
end
function sandbox.overload(func,overload,comment)
 comment=comment or comments[func] or "?"
 if type(func)~="function" then
  if trace then
   report("overloading unknown function: %s",comment)
  end
 elseif type(overload)~="function" then
  if trace then
   report("overloading function with bad overload: %s",comment)
  end
 elseif overloads[func]==nil then
  if trace then
   report("function is not registered: %s",comment)
  end
 elseif skiploads[func] then
  if trace then
   report("function is not skipped: %s",comment)
  end
 else
  if trace then
   report("overloading function: %s",comment)
  end
  overloads[func]=overload
 end
 return func
end
local function whatever(specification,what,target)
 if type(specification)~="table" then
  report("%s needs a specification",what)
 elseif type(specification.category)~="string" or type(specification.action)~="function" then
  report("%s needs a category and action",what)
 elseif not sandboxed then
  target[#target+1]=specification
 elseif trace then
  report("already enabled, discarding %s",what)
 end
end
function sandbox.initializer(specification)
 whatever(specification,"initializer",initializers)
end
function sandbox.finalizer(specification)
 whatever(specification,"finalizer",finalizers)
end
function require(name)
 local n=gsub(name,"^.*[\\/]","")
 local n=gsub(n,"[%.].*$","")
 local b=blocked[n]
 if b==false then
  return nil 
 elseif b then
  if trace then
   report("using blocked: %s",n)
  end
  return b
 else
  if trace then
   report("requiring: %s",name)
  end
  return requiem(name)
 end
end
function blockrequire(name,lib)
 if trace then
  report("preventing reload of: %s",name)
 end
 blocked[name]=lib or _G[name] or false
end
function sandbox.enable()
 if not sandboxed then
  debug={
   traceback=debug.traceback,
  }
  for i=1,#initializers do
   initializers[i].action()
  end
  for i=1,#finalizers do
   finalizers[i].action()
  end
  local nnot=0
  local nyes=0
  local cnot={}
  local cyes={}
  local skip={}
  for k,v in next,overloads do
   local c=comments[k]
   if v then
    if c then
     cyes[#cyes+1]=c
    else 
     nyes=nyes+1
    end
   else
    if c then
     cnot[#cnot+1]=c
    else 
     nnot=nnot+1
    end
   end
  end
  for k,v in next,skiploads do
   skip[#skip+1]=v
  end
  if #cyes>0 then
   sort(cyes)
   report("overloaded known: %s",concat(cyes," | "))
  end
  if nyes>0 then
   report("overloaded unknown: %s",nyes)
  end
  if #cnot>0 then
   sort(cnot)
   report("not overloaded known: %s",concat(cnot," | "))
  end
  if nnot>0 then
   report("not overloaded unknown: %s",nnot)
  end
  if #skip>0 then
   sort(skip)
   report("not overloaded redefined: %s",concat(skip," | "))
  end
  initializers=nil
  finalizers=nil
  originals=nil
  sandboxed=true
 end
end
blockrequire("lfs",lfs)
blockrequire("io",io)
blockrequire("os",os)
blockrequire("ffi",ffi)
local function supported(library)
 local l=_G[library]
 return l
end
loadfile=register(loadfile,"loadfile")
if supported("lua") then
 lua.openfile=register(lua.openfile,"lua.openfile")
end
if supported("io") then
 io.open=register(io.open,"io.open")
 io.popen=register(io.popen,"io.popen") 
 io.lines=register(io.lines,"io.lines")
 io.output=register(io.output,"io.output")
 io.input=register(io.input,"io.input")
end
if supported("os") then
 os.execute=register(os.execute,"os.execute")
 os.spawn=register(os.spawn,"os.spawn")
 os.exec=register(os.exec,"os.exec")
 os.rename=register(os.rename,"os.rename")
 os.remove=register(os.remove,"os.remove")
end
if supported("lfs") then
 lfs.chdir=register(lfs.chdir,"lfs.chdir")
 lfs.mkdir=register(lfs.mkdir,"lfs.mkdir")
 lfs.rmdir=register(lfs.rmdir,"lfs.rmdir")
 lfs.isfile=register(lfs.isfile,"lfs.isfile")
 lfs.isdir=register(lfs.isdir,"lfs.isdir")
 lfs.attributes=register(lfs.attributes,"lfs.attributes")
 lfs.dir=register(lfs.dir,"lfs.dir")
 lfs.lock_dir=register(lfs.lock_dir,"lfs.lock_dir")
 lfs.touch=register(lfs.touch,"lfs.touch")
 lfs.link=register(lfs.link,"lfs.link")
 lfs.setmode=register(lfs.setmode,"lfs.setmode")
 lfs.readlink=register(lfs.readlink,"lfs.readlink")
 lfs.shortname=register(lfs.shortname,"lfs.shortname")
 lfs.symlinkattributes=register(lfs.symlinkattributes,"lfs.symlinkattributes")
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-package"] = package.loaded["l-package"] or true

-- original size: 11605, stripped down to: 8299

if not modules then modules={} end modules ['l-package']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local type=type
local gsub,format,find=string.gsub,string.format,string.find
local insert,remove=table.insert,table.remove
local P,S,Cs,lpegmatch=lpeg.P,lpeg.S,lpeg.Cs,lpeg.match
local package=package
local searchers=package.searchers or package.loaders
local filejoin=file and file.join  or function(path,name)   return path.."/"..name end
local isreadable=file and file.is_readable or function(name)  local f=io.open(name) if f then f:close() return true end end
local addsuffix=file and file.addsuffix   or function(name,suffix) return name.."."..suffix end
local function cleanpath(path) 
 return path
end
local pattern=Cs((((1-S("\\/"))^0*(S("\\/")^1/"/"))^0*(P(".")^1/"/"+P(1))^1)*-1)
local function lualibfile(name)
 return lpegmatch(pattern,name) or name
end
local offset=luarocks and 1 or 0 
local helpers=package.helpers or {
 cleanpath=cleanpath,
 lualibfile=lualibfile,
 trace=false,
 report=function(...) print(format(...)) end,
 builtin={
  ["preload table"]=searchers[1+offset],
  ["path specification"]=searchers[2+offset],
  ["cpath specification"]=searchers[3+offset],
  ["all in one fallback"]=searchers[4+offset],
 },
 methods={},
 sequence={
  "already loaded",
  "preload table",
  "qualified path",
  "lua extra list",
  "lib extra list",
  "path specification",
  "cpath specification",
  "all in one fallback",
  "not loaded",
 }
}
package.helpers=helpers
local methods=helpers.methods
local builtin=helpers.builtin
local extraluapaths={}
local extralibpaths={}
local luapaths=nil 
local libpaths=nil 
local oldluapath=nil
local oldlibpath=nil
local nofextralua=-1
local nofextralib=-1
local nofpathlua=-1
local nofpathlib=-1
local function listpaths(what,paths)
 local nofpaths=#paths
 if nofpaths>0 then
  for i=1,nofpaths do
   helpers.report("using %s path %i: %s",what,i,paths[i])
  end
 else
  helpers.report("no %s paths defined",what)
 end
 return nofpaths
end
local function getextraluapaths()
 if helpers.trace and #extraluapaths~=nofextralua then
  nofextralua=listpaths("extra lua",extraluapaths)
 end
 return extraluapaths
end
local function getextralibpaths()
 if helpers.trace and #extralibpaths~=nofextralib then
  nofextralib=listpaths("extra lib",extralibpaths)
 end
 return extralibpaths
end
local function getluapaths()
 local luapath=package.path or ""
 if oldluapath~=luapath then
  luapaths=file.splitpath(luapath,";")
  oldluapath=luapath
  nofpathlua=-1
 end
 if helpers.trace and #luapaths~=nofpathlua then
  nofpathlua=listpaths("builtin lua",luapaths)
 end
 return luapaths
end
local function getlibpaths()
 local libpath=package.cpath or ""
 if oldlibpath~=libpath then
  libpaths=file.splitpath(libpath,";")
  oldlibpath=libpath
  nofpathlib=-1
 end
 if helpers.trace and #libpaths~=nofpathlib then
  nofpathlib=listpaths("builtin lib",libpaths)
 end
 return libpaths
end
package.luapaths=getluapaths
package.libpaths=getlibpaths
package.extraluapaths=getextraluapaths
package.extralibpaths=getextralibpaths
local hashes={
 lua={},
 lib={},
}
local function registerpath(tag,what,target,...)
 local pathlist={... }
 local cleanpath=helpers.cleanpath
 local trace=helpers.trace
 local report=helpers.report
 local hash=hashes[what]
 local function add(path)
  local path=cleanpath(path)
  if not hash[path] then
   target[#target+1]=path
   hash[path]=true
   if trace then
    report("registered %s path %s: %s",tag,#target,path)
   end
  else
   if trace then
    report("duplicate %s path: %s",tag,path)
   end
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
end
local function pushpath(tag,what,target,path)
 local path=helpers.cleanpath(path)
 insert(target,1,path)
 if helpers.trace then
  helpers.report("pushing %s path in front: %s",tag,path)
 end
end
local function poppath(tag,what,target)
 local path=remove(target,1)
 if helpers.trace then
  if path then
   helpers.report("popping %s path from front: %s",tag,path)
  else
   helpers.report("no %s path to pop",tag)
  end
 end
end
helpers.registerpath=registerpath
function package.extraluapath(...)
 registerpath("extra lua","lua",extraluapaths,...)
end
function package.pushluapath(path)
 pushpath("extra lua","lua",extraluapaths,path)
end
function package.popluapath()
 poppath("extra lua","lua",extraluapaths)
end
function package.extralibpath(...)
 registerpath("extra lib","lib",extralibpaths,...)
end
function package.pushlibpath(path)
 pushpath("extra lib","lib",extralibpaths,path)
end
function package.poplibpath()
 poppath("extra lib","lua",extralibpaths)
end
local function loadedaslib(resolved,rawname) 
 local base=gsub(rawname,"%.","_")
 local init="luaopen_"..gsub(base,"%.","_")
 if helpers.trace then
  helpers.report("calling loadlib with '%s' with init '%s'",resolved,init)
 end
 return package.loadlib(resolved,init)
end
helpers.loadedaslib=loadedaslib
local function loadedbypath(name,rawname,paths,islib,what)
 local trace=helpers.trace
 for p=1,#paths do
  local path=paths[p]
  local resolved=filejoin(path,name)
  if trace then
   helpers.report("%s path, identifying '%s' on '%s'",what,name,path)
  end
  if isreadable(resolved) then
   if trace then
    helpers.report("%s path, '%s' found on '%s'",what,name,resolved)
   end
   if islib then
    return loadedaslib(resolved,rawname)
   else
    return loadfile(resolved)
   end
  end
 end
end
helpers.loadedbypath=loadedbypath
local function loadedbyname(name,rawname)
 if find(name,"^/") or find(name,"^[a-zA-Z]:/") then
  local trace=helpers.trace
  if trace then
   helpers.report("qualified name, identifying '%s'",what,name)
  end
  if isreadable(name) then
   if trace then
    helpers.report("qualified name, '%s' found",what,name)
   end
   return loadfile(name)
  end
 end
end
helpers.loadedbyname=loadedbyname
methods["already loaded"]=function(name)
 return package.loaded[name]
end
methods["preload table"]=function(name)
 return builtin["preload table"](name)
end
methods["qualified path"]=function(name)
  return loadedbyname(addsuffix(lualibfile(name),"lua"),name)
end
methods["lua extra list"]=function(name)
 return loadedbypath(addsuffix(lualibfile(name),"lua"),name,getextraluapaths(),false,"lua")
end
methods["lib extra list"]=function(name)
 return loadedbypath(addsuffix(lualibfile(name),os.libsuffix),name,getextralibpaths(),true,"lib")
end
methods["path specification"]=function(name)
 getluapaths() 
 return builtin["path specification"](name)
end
methods["cpath specification"]=function(name)
 getlibpaths() 
 return builtin["cpath specification"](name)
end
methods["all in one fallback"]=function(name)
 return builtin["all in one fallback"](name)
end
methods["not loaded"]=function(name)
 if helpers.trace then
  helpers.report("unable to locate '%s'",name or "?")
 end
 return nil
end
local level=0
local used={}
helpers.traceused=false
function helpers.loaded(name)
 local sequence=helpers.sequence
 level=level+1
 for i=1,#sequence do
  local method=sequence[i]
  if helpers.trace then
   helpers.report("%s, level '%s', method '%s', name '%s'","locating",level,method,name)
  end
  local result,rest=methods[method](name)
  if type(result)=="function" then
   if helpers.trace then
    helpers.report("%s, level '%s', method '%s', name '%s'","found",level,method,name)
   end
   if helpers.traceused then
    used[#used+1]={ level=level,name=name }
   end
   level=level-1
   return result,rest
  end
 end
 level=level-1
 return nil
end
function helpers.showused()
 local n=#used
 if n>0 then
  helpers.report("%s libraries loaded:",n)
  helpers.report()
  for i=1,n do
   local u=used[i]
   helpers.report("%i %a",u.level,u.name)
  end
  helpers.report()
  end
end
function helpers.unload(name)
 if helpers.trace then
  if package.loaded[name] then
   helpers.report("unloading, name '%s', %s",name,"done")
  else
   helpers.report("unloading, name '%s', %s",name,"not loaded")
  end
 end
 package.loaded[name]=nil
end
table.insert(searchers,1,helpers.loaded)
if context then
 package.path=""
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-lpeg"] = package.loaded["l-lpeg"] or true

-- original size: 38440, stripped down to: 19316

if not modules then modules={} end modules ['l-lpeg']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
lpeg=require("lpeg") 
local lpeg=lpeg
if not lpeg.print then function lpeg.print(...) print(lpeg.pcode(...)) end end
local type,next,tostring=type,next,tostring
local byte,char,gmatch,format=string.byte,string.char,string.gmatch,string.format
local floor=math.floor
local P,R,S,V,Ct,C,Cs,Cc,Cp,Cmt=lpeg.P,lpeg.R,lpeg.S,lpeg.V,lpeg.Ct,lpeg.C,lpeg.Cs,lpeg.Cc,lpeg.Cp,lpeg.Cmt
local lpegtype,lpegmatch,lpegprint=lpeg.type,lpeg.match,lpeg.print
if setinspector then
 setinspector("lpeg",function(v) if lpegtype(v) then lpegprint(v) return true end end)
end
lpeg.patterns=lpeg.patterns or {} 
local patterns=lpeg.patterns
local anything=P(1)
local endofstring=P(-1)
local alwaysmatched=P(true)
patterns.anything=anything
patterns.endofstring=endofstring
patterns.beginofstring=alwaysmatched
patterns.alwaysmatched=alwaysmatched
local sign=S('+-')
local zero=P('0')
local digit=R('09')
local digits=digit^1
local octdigit=R("07")
local octdigits=octdigit^1
local lowercase=R("az")
local uppercase=R("AZ")
local underscore=P("_")
local hexdigit=digit+lowercase+uppercase
local hexdigits=hexdigit^1
local cr,lf,crlf=P("\r"),P("\n"),P("\r\n")
local newline=P("\r")*(P("\n")+P(true))+P("\n")  
local escaped=P("\\")*anything
local squote=P("'")
local dquote=P('"')
local space=P(" ")
local period=P(".")
local comma=P(",")
local utfbom_32_be=P('\000\000\254\255') 
local utfbom_32_le=P('\255\254\000\000') 
local utfbom_16_be=P('\254\255')   
local utfbom_16_le=P('\255\254')   
local utfbom_8=P('\239\187\191')  
local utfbom=utfbom_32_be+utfbom_32_le+utfbom_16_be+utfbom_16_le+utfbom_8
local utftype=utfbom_32_be*Cc("utf-32-be")+utfbom_32_le*Cc("utf-32-le")+utfbom_16_be*Cc("utf-16-be")+utfbom_16_le*Cc("utf-16-le")+utfbom_8*Cc("utf-8")+alwaysmatched*Cc("utf-8") 
local utfstricttype=utfbom_32_be*Cc("utf-32-be")+utfbom_32_le*Cc("utf-32-le")+utfbom_16_be*Cc("utf-16-be")+utfbom_16_le*Cc("utf-16-le")+utfbom_8*Cc("utf-8")
local utfoffset=utfbom_32_be*Cc(4)+utfbom_32_le*Cc(4)+utfbom_16_be*Cc(2)+utfbom_16_le*Cc(2)+utfbom_8*Cc(3)+Cc(0)
local utf8next=R("\128\191")
patterns.utfbom_32_be=utfbom_32_be
patterns.utfbom_32_le=utfbom_32_le
patterns.utfbom_16_be=utfbom_16_be
patterns.utfbom_16_le=utfbom_16_le
patterns.utfbom_8=utfbom_8
patterns.utf_16_be_nl=P("\000\r\000\n")+P("\000\r")+P("\000\n") 
patterns.utf_16_le_nl=P("\r\000\n\000")+P("\r\000")+P("\n\000") 
patterns.utf_32_be_nl=P("\000\000\000\r\000\000\000\n")+P("\000\000\000\r")+P("\000\000\000\n")
patterns.utf_32_le_nl=P("\r\000\000\000\n\000\000\000")+P("\r\000\000\000")+P("\n\000\000\000")
patterns.utf8one=R("\000\127")
patterns.utf8two=R("\194\223")*utf8next
patterns.utf8three=R("\224\239")*utf8next*utf8next
patterns.utf8four=R("\240\244")*utf8next*utf8next*utf8next
patterns.utfbom=utfbom
patterns.utftype=utftype
patterns.utfstricttype=utfstricttype
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
local fullstripper=whitespace^0*C((whitespace^0*nonwhitespace^1)^0)
local collapser=Cs(spacer^0/""*nonspacer^0*((spacer^0/" "*nonspacer^1)^0))
local nospacer=Cs((whitespace^1/""+nonwhitespace^1)^0)
local b_collapser=Cs(whitespace^0/""*(nonwhitespace^1+whitespace^1/" ")^0)
local e_collapser=Cs((whitespace^1*endofstring/""+nonwhitespace^1+whitespace^1/" ")^0)
local m_collapser=Cs((nonwhitespace^1+whitespace^1/" ")^0)
local b_stripper=Cs(spacer^0/""*(nonspacer^1+spacer^1/" ")^0)
local e_stripper=Cs((spacer^1*endofstring/""+nonspacer^1+spacer^1/" ")^0)
local m_stripper=Cs((nonspacer^1+spacer^1/" ")^0)
patterns.stripper=stripper
patterns.fullstripper=fullstripper
patterns.collapser=collapser
patterns.nospacer=nospacer
patterns.b_collapser=b_collapser
patterns.m_collapser=m_collapser
patterns.e_collapser=e_collapser
patterns.b_stripper=b_stripper
patterns.m_stripper=m_stripper
patterns.e_stripper=e_stripper
patterns.lowercase=lowercase
patterns.uppercase=uppercase
patterns.letter=patterns.lowercase+patterns.uppercase
patterns.space=space
patterns.tab=P("\t")
patterns.spaceortab=patterns.space+patterns.tab
patterns.newline=newline
patterns.emptyline=newline^1
patterns.equal=P("=")
patterns.comma=comma
patterns.commaspacer=comma*spacer^0
patterns.period=period
patterns.colon=P(":")
patterns.semicolon=P(";")
patterns.underscore=underscore
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
patterns.digit=digit
patterns.digits=digits
patterns.octdigit=octdigit
patterns.octdigits=octdigits
patterns.hexdigit=hexdigit
patterns.hexdigits=hexdigits
patterns.sign=sign
patterns.cardinal=digits
patterns.integer=sign^-1*digits
patterns.unsigned=digit^0*period*digits
patterns.float=sign^-1*patterns.unsigned
patterns.cunsigned=digit^0*comma*digits
patterns.cpunsigned=digit^0*(period+comma)*digits
patterns.cfloat=sign^-1*patterns.cunsigned
patterns.cpfloat=sign^-1*patterns.cpunsigned
patterns.number=patterns.float+patterns.integer
patterns.cnumber=patterns.cfloat+patterns.integer
patterns.cpnumber=patterns.cpfloat+patterns.integer
patterns.oct=zero*octdigits 
patterns.octal=patterns.oct
patterns.HEX=zero*P("X")*(digit+uppercase)^1
patterns.hex=zero*P("x")*(digit+lowercase)^1
patterns.hexadecimal=zero*S("xX")*hexdigits
patterns.hexafloat=sign^-1*zero*S("xX")*(hexdigit^0*period*hexdigits+hexdigits*period*hexdigit^0+hexdigits)*(S("pP")*sign^-1*hexdigits)^-1
patterns.decafloat=sign^-1*(digit^0*period*digits+digits*period*digit^0+digits)*S("eE")*sign^-1*digits
patterns.propername=(uppercase+lowercase+underscore)*(uppercase+lowercase+underscore+digit)^0*endofstring
patterns.somecontent=(anything-newline-space)^1 
patterns.beginline=#(1-newline)
patterns.longtostring=Cs(whitespace^0/""*((patterns.quoted+nonwhitespace^1+whitespace^1/""*(endofstring+Cc(" ")))^0))
local function anywhere(pattern) 
 return (1-P(pattern))^0*P(pattern)
end
lpeg.anywhere=anywhere
function lpeg.instringchecker(p)
 p=anywhere(p)
 return function(str)
  return lpegmatch(p,str) and true or false
 end
end
function lpeg.splitter(pattern,action)
 if action then
  return (((1-P(pattern))^1)/action+1)^0
 else
  return (Cs((1-P(pattern))^1)+1)^0
 end
end
function lpeg.tsplitter(pattern,action)
 if action then
  return Ct((((1-P(pattern))^1)/action+1)^0)
 else
  return Ct((Cs((1-P(pattern))^1)+1)^0)
 end
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
local function f2(s) local c1,c2=byte(s,1,2) return   c1*64+c2-12416 end
local function f3(s) local c1,c2,c3=byte(s,1,3) return  (c1*64+c2)*64+c3-925824 end
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
function lpeg.finder(lst,makefunction,isutf) 
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
 if isutf then
  pattern=((utf8char or 1)-pattern)^0*pattern
 else
  pattern=(1-pattern)^0*pattern
 end
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
  local pattern=P(separator)
  splitter=C((1-pattern)^0)
  splitters_f[separator]=splitter
 end
 return splitter
end
function lpeg.secondofsplit(separator) 
 local splitter=splitters_s[separator]
 if not splitter then
  local pattern=P(separator)
  splitter=(1-pattern)^0*pattern*C(anything^0)
  splitters_s[separator]=splitter
 end
 return splitter
end
local splitters_s,splitters_p={},{}
function lpeg.beforesuffix(separator) 
 local splitter=splitters_s[separator]
 if not splitter then
  local pattern=P(separator)
  splitter=C((1-pattern)^0)*pattern*endofstring
  splitters_s[separator]=splitter
 end
 return splitter
end
function lpeg.afterprefix(separator) 
 local splitter=splitters_p[separator]
 if not splitter then
  local pattern=P(separator)
  splitter=pattern*C(anything^0)
  splitters_p[separator]=splitter
 end
 return splitter
end
function lpeg.balancer(left,right)
 left,right=P(left),P(right)
 return P { left*((1-left-right)+V(1))^0*right }
end
function lpeg.counter(pattern,action)
 local n=0
 local pattern=(P(pattern)/function() n=n+1 end+anything)^0
 if action then
  return function(str) n=0;lpegmatch(pattern,str);action(n) end
 else
  return function(str) n=0;lpegmatch(pattern,str);return n end
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
local p_false=P(false)
local p_true=P(true)
local lower=utf and utf.lower or string.lower
local upper=utf and utf.upper or string.upper
function lpeg.setutfcasers(l,u)
 lower=l or lower
 upper=u or upper
end
local function make1(t,rest)
 local p=p_false
 local keys=sortedkeys(t)
 for i=1,#keys do
  local k=keys[i]
  if k~="" then
   local v=t[k]
   if v==true then
    p=p+P(k)*p_true
   elseif v==false then
   else
    p=p+P(k)*make1(v,v[""])
   end
  end
 end
 if rest then
  p=p+p_true
 end
 return p
end
local function make2(t,rest) 
 local p=p_false
 local keys=sortedkeys(t)
 for i=1,#keys do
  local k=keys[i]
  if k~="" then
   local v=t[k]
   if v==true then
    p=p+(P(lower(k))+P(upper(k)))*p_true
   elseif v==false then
   else
    p=p+(P(lower(k))+P(upper(k)))*make2(v,v[""])
   end
  end
 end
 if rest then
  p=p+p_true
 end
 return p
end
local function utfchartabletopattern(list,insensitive) 
 local tree={}
 local n=#list
 if n==0 then
  for s in next,list do
   local t=tree
   local p,pk
   for c in gmatch(s,".") do
    if t==true then
     t={ [c]=true,[""]=true }
     p[pk]=t
     p=t
     t=false
    elseif t==false then
     t={ [c]=false }
     p[pk]=t
     p=t
     t=false
    else
     local tc=t[c]
     if not tc then
      tc=false
      t[c]=false
     end
     p=t
     t=tc
    end
    pk=c
   end
   if t==false then
    p[pk]=true
   elseif t==true then
   else
    t[""]=true
   end
  end
 else
  for i=1,n do
   local s=list[i]
   local t=tree
   local p,pk
   for c in gmatch(s,".") do
    if t==true then
     t={ [c]=true,[""]=true }
     p[pk]=t
     p=t
     t=false
    elseif t==false then
     t={ [c]=false }
     p[pk]=t
     p=t
     t=false
    else
     local tc=t[c]
     if not tc then
      tc=false
      t[c]=false
     end
     p=t
     t=tc
    end
    pk=c
   end
   if t==false then
    p[pk]=true
   elseif t==true then
   else
    t[""]=true
   end
  end
 end
 return (insensitive and make2 or make1)(tree)
end
lpeg.utfchartabletopattern=utfchartabletopattern
function lpeg.utfreplacer(list,insensitive)
 local pattern=Cs((utfchartabletopattern(list,insensitive)/list+utf8character)^0)
 return function(str)
  return lpegmatch(pattern,str) or str
 end
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
do
 local trailingzeros=zero^0*-digit 
 local stripper=Cs((
  digits*(
   period*trailingzeros/""+period*(digit-trailingzeros)^1*(trailingzeros/"")
  )+1
 )^0)
 lpeg.patterns.stripzeros=stripper 
 local nonzero=digit-zero
 local trailingzeros=zero^1*endofstring
 local stripper=Cs((1-period)^0*(
  period*trailingzeros/""+period*(nonzero^1+(trailingzeros/"")+zero^1)^0+endofstring
 ))
 lpeg.patterns.stripzero=stripper
end
local byte_to_HEX={}
local byte_to_hex={}
local byte_to_dec={} 
local hex_to_byte={}
for i=0,255 do
 local H=format("%02X",i)
 local h=format("%02x",i)
 local d=format("%03i",i)
 local c=char(i)
 byte_to_HEX[c]=H
 byte_to_hex[c]=h
 byte_to_dec[c]=d
 hex_to_byte[h]=c
 hex_to_byte[H]=c
end
local hextobyte=P(2)/hex_to_byte
local bytetoHEX=P(1)/byte_to_HEX
local bytetohex=P(1)/byte_to_hex
local bytetodec=P(1)/byte_to_dec
local hextobytes=Cs(hextobyte^0)
local bytestoHEX=Cs(bytetoHEX^0)
local bytestohex=Cs(bytetohex^0)
local bytestodec=Cs(bytetodec^0)
patterns.hextobyte=hextobyte
patterns.bytetoHEX=bytetoHEX
patterns.bytetohex=bytetohex
patterns.bytetodec=bytetodec
patterns.hextobytes=hextobytes
patterns.bytestoHEX=bytestoHEX
patterns.bytestohex=bytestohex
patterns.bytestodec=bytestodec
function string.toHEX(s)
 if not s or s=="" then
  return s
 else
  return lpegmatch(bytestoHEX,s)
 end
end
function string.tohex(s)
 if not s or s=="" then
  return s
 else
  return lpegmatch(bytestohex,s)
 end
end
function string.todec(s)
 if not s or s=="" then
  return s
 else
  return lpegmatch(bytestodec,s)
 end
end
function string.tobytes(s)
 if not s or s=="" then
  return s
 else
  return lpegmatch(hextobytes,s)
 end
end
local patterns={} 
local function containsws(what)
 local p=patterns[what]
 if not p then
  local p1=P(what)*(whitespace+endofstring)*Cc(true)
  local p2=whitespace*P(p1)
  p=P(p1)+P(1-p2)^0*p2+Cc(false)
  patterns[what]=p
 end
 return p
end
lpeg.containsws=containsws
function string.containsws(str,what)
 return lpegmatch(patterns[what] or containsws(what),str)
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-function"] = package.loaded["l-function"] or true

-- original size: 361, stripped down to: 317

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

-- original size: 6644, stripped down to: 3410

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
local fullstripper=patterns.fullstripper
local collapser=patterns.collapser
local nospacer=patterns.nospacer
local longtostring=patterns.longtostring
function string.strip(str)
 return str and lpegmatch(stripper,str) or ""
end
function string.fullstrip(str)
 return str and lpegmatch(fullstripper,str) or ""
end
function string.collapsespaces(str)
 return str and lpegmatch(collapser,str) or ""
end
function string.nospaces(str)
 return str and lpegmatch(nospacer,str) or ""
end
function string.longtostring(str)
 return str and lpegmatch(longtostring,str) or ""
end
local pattern=P(" ")^0*P(-1)
function string.is_empty(str)
 if not str or str=="" then
  return true
 else
  return lpegmatch(pattern,str) and true or false
 end
end
local anything=patterns.anything
local moreescapes=Cc("%")*S(".-+%?()[]*$^{}")
local allescapes=Cc("%")*S(".-+%?()[]*")   
local someescapes=Cc("%")*S(".-+%()[]")  
local matchescapes=Cc(".")*S("*?")     
local pattern_m=Cs ((moreescapes+anything )^0 )
local pattern_a=Cs ((allescapes+anything )^0 )
local pattern_b=Cs ((someescapes+matchescapes+anything )^0 )
local pattern_c=Cs (Cc("^")*(someescapes+matchescapes+anything )^0*Cc("$") )
function string.escapedpattern(str,simple)
 return lpegmatch(simple and pattern_b or pattern_a,str)
end
function string.topattern(str,lowercase,strict)
 if str=="" or type(str)~="string" then
  return ".*"
 elseif strict=="all" then
  str=lpegmatch(pattern_m,str)
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
local pattern_c=Ct(C(1)^0) 
local pattern_b=Ct((C(1)/byte)^0)
function string.totable(str,bytes)
 return lpegmatch(bytes and pattern_b or pattern_c,str)
end
local replacer=lpeg.replacer("@","%%") 
function string.tformat(fmt,...)
 return format(lpegmatch(replacer,fmt),...)
end
string.quote=string.quoted
string.unquote=string.unquoted
if not string.bytetable then 
 local limit=5000 
 function string.bytetable(str) 
  local n=#str
  if n>limit then
   local t={ byte(str,1,limit) }
   for i=limit+1,n do
    t[i]=byte(str,i)
   end
   return t
  else
   return { byte(str,1,n) }
  end
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-table"] = package.loaded["l-table"] or true

-- original size: 41758, stripped down to: 22643

if not modules then modules={} end modules ['l-table']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local type,next,tostring,tonumber,select,rawget=type,next,tostring,tonumber,select,rawget
local table,string=table,string
local concat,sort=table.concat,table.sort
local format,lower,dump=string.format,string.lower,string.dump
local getmetatable,setmetatable=getmetatable,setmetatable
local lpegmatch,patterns=lpeg.match,lpeg.patterns
local floor=math.floor
local stripper=patterns.stripper
function table.getn(t)
 return t and #t 
end
function table.strip(tab)
 local lst={}
 local l=0
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
  local keys={}
  local k=0
  for key in next,t do
   k=k+1
   keys[k]=key
  end
  return keys
 else
  return {}
 end
end
local function compare(a,b)
 local ta=type(a) 
 if ta=="number" then
  local tb=type(b) 
  if ta==tb then
   return a<b
  elseif tb=="string" then
   return tostring(a)<b
  end
 elseif ta=="string" then
  local tb=type(b) 
  if ta==tb then
   return a<b
  else
   return a<tostring(b)
  end
 end
 return tostring(a)<tostring(b) 
end
local function sortedkeys(tab)
 if tab then
  local srt={}
  local category=0 
  local s=0
  for key in next,tab do
   s=s+1
   srt[s]=key
   if category~=3 then
    local tkey=type(key)
    if category==1 then
     if tkey~="string" then
      category=3
     end
    elseif category==2 then
     if tkey~="number" then
      category=3
     end
    else
     if tkey=="string" then
      category=1
     elseif tkey=="number" then
      category=2
     else
      category=3
     end
    end
   end
  end
  if s<2 then
  elseif category==3 then
   sort(srt,compare)
  else
   sort(srt)
  end
  return srt
 else
  return {}
 end
end
local function sortedhashonly(tab)
 if tab then
  local srt={}
  local s=0
  for key in next,tab do
   if type(key)=="string" then
    s=s+1
    srt[s]=key
   end
  end
  if s>1 then
   sort(srt)
  end
  return srt
 else
  return {}
 end
end
local function sortedindexonly(tab)
 if tab then
  local srt={}
  local s=0
  for key in next,tab do
   if type(key)=="number" then
    s=s+1
    srt[s]=key
   end
  end
  if s>1 then
   sort(srt)
  end
  return srt
 else
  return {}
 end
end
local function sortedhashkeys(tab,cmp) 
 if tab then
  local srt={}
  local s=0
  for key in next,tab do
   if key then
    s=s+1
    srt[s]=key
   end
  end
  if s>1 then
   sort(srt,cmp)
  end
  return srt
 else
  return {}
 end
end
function table.allkeys(t)
 local keys={}
 for k,v in next,t do
  for k in next,v do
   keys[k]=true
  end
 end
 return sortedkeys(keys)
end
table.sortedkeys=sortedkeys
table.sortedhashonly=sortedhashonly
table.sortedindexonly=sortedindexonly
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
  local m=#s
  if m==1 then
   return next,t
  elseif m>0 then
   local n=0
   return function()
    if n<m then
     n=n+1
     local k=s[n]
     return k,t[k]
    end
   end
  end
 end
 return nothing
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
 if not t then
  t={}
 end
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
 local tmp={}
 local ntmp=0
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
 if not tables then
  tables={}
 end
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
 local hsh={}
 local h=0
 for k,v in next,t do
  if v then
   h=h+1
   hsh[h]=k
  end
 end
 return hsh
end
local noquotes,hexify,handle,compact,inline,functions,metacheck,accurate
local reserved=table.tohash { 
 'and','break','do','else','elseif','end','false','for','function','if',
 'in','local','nil','not','or','repeat','return','then','true','until','while',
 'NaN','goto','const',
}
local function is_simple_table(t,hexify,accurate) 
 local nt=#t
 if nt>0 then
  local n=0
  for _,v in next,t do
   n=n+1
   if type(v)=="table" then
    return nil
   end
  end
  local haszero=rawget(t,0) 
  if n==nt then
   local tt={}
   for i=1,nt do
    local v=t[i]
    local tv=type(v)
    if tv=="number" then
     if hexify then
      tt[i]=format("0x%X",v)
     elseif accurate then
      tt[i]=format("%q",v)
     else
      tt[i]=v 
     end
    elseif tv=="string" then
     tt[i]=format("%q",v) 
    elseif tv=="boolean" then
     tt[i]=v and "true" or "false"
    else
     return nil
    end
   end
   return tt
  elseif haszero and (n==nt+1) then
   local tt={}
   for i=0,nt do
    local v=t[i]
    local tv=type(v)
    if tv=="number" then
     if hexify then
      tt[i+1]=format("0x%X",v)
     elseif accurate then
      tt[i+1]=format("%q",v)
     else
      tt[i+1]=v 
     end
    elseif tv=="string" then
     tt[i+1]=format("%q",v) 
    elseif tv=="boolean" then
     tt[i+1]=v and "true" or "false"
    else
     return nil
    end
   end
   tt[1]="[0] = "..tt[1]
   return tt
  end
 end
 return nil
end
table.is_simple_table=is_simple_table
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
     handle(format("%s[0x%X]={",depth,name))
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
    handle(format("%s[%s]={",depth,name and "true" or "false"))
   else
    handle(format("%s{",depth))
   end
  end
 end
 if root and next(root)~=nil then
  local first=nil
  local last=0
  if compact then
   last=#root
   for k=1,last do
    if rawget(root,k)==nil then
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
   local tv=type(v)
   local tk=type(k)
   if compact and first and tk=="number" and k>=first and k<=last then
    if tv=="number" then
     if hexify then
      handle(format("%s 0x%X,",depth,v))
     elseif accurate then
      handle(format("%s %q,",depth,v))
     else
      handle(format("%s %s,",depth,v)) 
     end
    elseif tv=="string" then
     handle(format("%s %q,",depth,v))
    elseif tv=="table" then
     if next(v)==nil then
      handle(format("%s {},",depth))
     elseif inline then 
      local st=is_simple_table(v,hexify,accurate)
      if st then
       handle(format("%s { %s },",depth,concat(st,", ")))
      else
       do_serialize(v,k,depth,level+1,true)
      end
     else
      do_serialize(v,k,depth,level+1,true)
     end
    elseif tv=="boolean" then
     handle(format("%s %s,",depth,v and "true" or "false"))
    elseif tv=="function" then
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
   elseif tv=="number" then
    if tk=="number" then
     if hexify then
      handle(format("%s [0x%X]=0x%X,",depth,k,v))
     elseif accurate then
      handle(format("%s [%s]=%q,",depth,k,v))
     else
      handle(format("%s [%s]=%s,",depth,k,v)) 
     end
    elseif tk=="boolean" then
     if hexify then
      handle(format("%s [%s]=0x%X,",depth,k and "true" or "false",v))
     elseif accurate then
      handle(format("%s [%s]=%q,",depth,k and "true" or "false",v))
     else
      handle(format("%s [%s]=%s,",depth,k and "true" or "false",v)) 
     end
    elseif tk~="string" then
    elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
     if hexify then
      handle(format("%s %s=0x%X,",depth,k,v))
     elseif accurate then
      handle(format("%s %s=%q,",depth,k,v))
     else
      handle(format("%s %s=%s,",depth,k,v)) 
     end
    else
     if hexify then
      handle(format("%s [%q]=0x%X,",depth,k,v))
     elseif accurate then
      handle(format("%s [%q]=%q,",depth,k,v))
     else
      handle(format("%s [%q]=%s,",depth,k,v)) 
     end
    end
   elseif tv=="string" then
    if tk=="number" then
     if hexify then
      handle(format("%s [0x%X]=%q,",depth,k,v))
     elseif accurate then
      handle(format("%s [%q]=%q,",depth,k,v))
     else
      handle(format("%s [%s]=%q,",depth,k,v))
     end
    elseif tk=="boolean" then
     handle(format("%s [%s]=%q,",depth,k and "true" or "false",v))
    elseif tk~="string" then
    elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
     handle(format("%s %s=%q,",depth,k,v))
    else
     handle(format("%s [%q]=%q,",depth,k,v))
    end
   elseif tv=="table" then
    if next(v)==nil then
     if tk=="number" then
      if hexify then
       handle(format("%s [0x%X]={},",depth,k))
      elseif accurate then
       handle(format("%s [%q]={},",depth,k))
      else
       handle(format("%s [%s]={},",depth,k))
      end
     elseif tk=="boolean" then
      handle(format("%s [%s]={},",depth,k and "true" or "false"))
     elseif tk~="string" then
     elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
      handle(format("%s %s={},",depth,k))
     else
      handle(format("%s [%q]={},",depth,k))
     end
    elseif inline then
     local st=is_simple_table(v,hexify,accurate)
     if st then
      if tk=="number" then
       if hexify then
        handle(format("%s [0x%X]={ %s },",depth,k,concat(st,", ")))
       elseif accurate then
        handle(format("%s [%q]={ %s },",depth,k,concat(st,", ")))
       else
        handle(format("%s [%s]={ %s },",depth,k,concat(st,", ")))
       end
      elseif tk=="boolean" then
       handle(format("%s [%s]={ %s },",depth,k and "true" or "false",concat(st,", ")))
      elseif tk~="string" then
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
   elseif tv=="boolean" then
    if tk=="number" then
     if hexify then
      handle(format("%s [0x%X]=%s,",depth,k,v and "true" or "false"))
     elseif accurate then
      handle(format("%s [%q]=%s,",depth,k,v and "true" or "false"))
     else
      handle(format("%s [%s]=%s,",depth,k,v and "true" or "false"))
     end
    elseif tk=="boolean" then
     handle(format("%s [%s]=%s,",depth,tostring(k),v and "true" or "false"))
    elseif tk~="string" then
    elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
     handle(format("%s %s=%s,",depth,k,v and "true" or "false"))
    else
     handle(format("%s [%q]=%s,",depth,k,v and "true" or "false"))
    end
   elseif tv=="function" then
    if functions then
     local getinfo=debug and debug.getinfo
     if getinfo then
      local f=getinfo(v).what=="C" and dump(dummy) or dump(v)
      if tk=="number" then
       if hexify then
        handle(format("%s [0x%X]=load(%q),",depth,k,f))
       elseif accurate then
        handle(format("%s [%q]=load(%q),",depth,k,f))
       else
        handle(format("%s [%s]=load(%q),",depth,k,f))
       end
      elseif tk=="boolean" then
       handle(format("%s [%s]=load(%q),",depth,k and "true" or "false",f))
      elseif tk~="string" then
      elseif noquotes and not reserved[k] and lpegmatch(propername,k) then
       handle(format("%s %s=load(%q),",depth,k,f))
      else
       handle(format("%s [%q]=load(%q),",depth,k,f))
      end
     end
    end
   else
    if tk=="number" then
     if hexify then
      handle(format("%s [0x%X]=%q,",depth,k,tostring(v)))
     elseif accurate then
      handle(format("%s [%q]=%q,",depth,k,tostring(v)))
     else
      handle(format("%s [%s]=%q,",depth,k,tostring(v)))
     end
    elseif tk=="boolean" then
     handle(format("%s [%s]=%q,",depth,k and "true" or "false",tostring(v)))
    elseif tk~="string" then
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
  accurate=specification.accurate
  handle=_handle or specification.handle or print
  functions=specification.functions
  compact=specification.compact
  inline=specification.inline and compact
  metacheck=specification.metacheck
  if functions==nil then
   functions=true
  end
  if compact==nil then
   compact=true
  end
  if inline==nil then
   inline=compact
  end
  if metacheck==nil then
   metacheck=true
  end
 else
  noquotes=false
  hexify=false
  handle=_handle or print
  compact=true
  inline=true
  functions=true
  metacheck=true
 end
 if tname=="string" then
  if name=="return" then
   handle("return {")
  else
   handle(name.."={")
  end
 elseif tname=="number" then
  if hexify then
   handle(format("[0x%X]={",name))
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
  if metacheck and getmetatable(root) then
   local dummy=root._w_h_a_t_e_v_e_r_
   root._w_h_a_t_e_v_e_r_=nil
  end
  if next(root)~=nil then
   do_serialize(root,name,"",0)
  end
 end
 handle("}")
end
function table.serialize(root,name,specification)
 local t={}
 local n=0
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
   local t={}
   local n=0
   local function flush(s)
    n=n+1
    t[n]=s
    if n>maxtab then
     f:write(concat(t,"\n"),"\n") 
     t={} 
     n=0
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
    f[#f+1]=v
   end
  end
 end
 for k=1,#t do
  local v=t[k]
  if depth>0 and type(v)=="table" then
   flattened(v,f,depth-1)
  else
   f[#f+1]=v
  end
 end
 return f
end
table.flattened=flattened
local function collapsed(t,f,h)
 if f==nil then
  f={}
  h={}
 end
 for k=1,#t do
  local v=t[k]
  if type(v)=="table" then
   collapsed(v,f,h)
  elseif not h[v] then
   f[#f+1]=v
   h[v]=true
  end
 end
 return f
end
local function collapsedhash(t,h)
 if h==nil then
  h={}
 end
 for k=1,#t do
  local v=t[k]
  if type(v)=="table" then
   collapsedhash(v,h)
  else
   h[v]=true
  end
 end
 return h
end
table.collapsed=collapsed  
table.collapsedhash=collapsedhash
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
 if a==b then
  return true
 elseif a and b and #a==#b then
  if not n then
   n=1
  end
  if not m then
   m=#a
  end
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
 if a~=b then
  for ka,va in next,a do
   local vb=b[ka]
   if va==vb then
   elseif type(va)=="table" and  type(vb)=="table" then
    if not identical(va,vb) then
     return false
    end
   else
    return false
   end
  end
 end
 return true
end
table.identical=identical
table.are_equal=are_equal
local function sparse(old,nest,keeptables)
 local new={}
 for k,v in next,old do
  if not (v=="" or v==false) then
   if nest and type(v)=="table" then
    v=sparse(v,nest)
    if keeptables or next(v)~=nil then
     new[k]=v
    end
   else
    new[k]=v
   end
  end
 end
 return new
end
table.sparse=sparse
function table.compact(t)
 return sparse(t,true,true)
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
function table.hashed(t) 
 for i=1,#t do
  t[t[i]]=i
 end
 return t
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
  local tt={}
  local tn=#t
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
  local m=n+1
  for i=1,floor(n/2) do 
   local j=m-i
   t[i],t[j]=t[j],t[i]
  end
  return t
 end
end
local function sequenced(t,sep,simple)
 if not t then
  return ""
 elseif type(t)~="table" then
  return t 
 end
 local n=#t
 local s={}
 if n>0 then
  for i=1,n do
   local v=t[i]
   if type(v)=="table" then
    s[i]="{"..sequenced(v,sep,simple).."}"
   else
    s[i]=tostring(t[i])
   end
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
     if type(v)=="table" then
      s[n]=k.."={"..sequenced(v,sep,simple).."}"
     else
      s[n]=k.."="..tostring(v)
     end
    end
   else
    n=n+1
    if type(v)=="table" then
     s[n]=k.."={"..sequenced(v,sep,simple).."}"
    else
     s[n]=k.."="..tostring(v)
    end
   end
  end
 end
 if sep==true then
  return "{ "..concat(s,", ").." }"
 else
  return concat(s,sep or " | ")
 end
end
table.sequenced=sequenced
function table.print(t,...)
 if type(t)~="table" then
  print(tostring(t))
 else
  serialize(print,t,...)
 end
end
if setinspector then
 setinspector("table",function(v) if type(v)=="table" then serialize(print,v,"table") return true end end)
end
function table.sub(t,i,j)
 return { unpack(t,i,j) }
end
function table.is_empty(t)
 return not t or next(t)==nil
end
function table.has_one_entry(t)
 return t and next(t,next(t))==nil
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
function table.values(t,s) 
 if t then
  local values={}
  local keys={}
  local v=0
  for key,value in next,t do
   if not keys[value] then
    v=v+1
    values[v]=value
    keys[k]=key
   end
  end
  if s then
   sort(values)
  end
  return values
 else
  return {}
 end
end
function table.filtered(t,pattern,sort,cmp)
 if t and type(pattern)=="string" then
  if sort then
   local s
   if cmp then
    s=sortedhashkeys(t,function(a,b) return cmp(t,a,b) end)
   else
    s=sortedkeys(t) 
   end
   local n=0
   local m=#s
   local function kv(s)
    while n<m do
     n=n+1
     local k=s[n]
     if find(k,pattern) then
      return k,t[k]
     end
    end
   end
   return kv,s
  else
   local n=next(t)
   local function iterator()
    while n~=nil do
     local k=n
     n=next(t,k)
     if find(k,pattern) then
      return k,t[k]
     end
    end
   end
   return iterator,t
  end
 else
  return nothing
 end
end
if not table.move then
 function table.move(a1,f,e,t,a2)
  if a2 and a1~=a2 then
   for i=f,e do
    a2[t]=a1[i]
    t=t+1
   end
   return a2
  else
   t=t+e-f
   for i=e,f,-1 do
    a1[t]=a1[i]
    t=t-1
   end
   return a1
  end
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-io"] = package.loaded["l-io"] or true

-- original size: 11829, stripped down to: 6331

if not modules then modules={} end modules ['l-io']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local io=io
local open,flush,write,read=io.open,io.flush,io.write,io.read
local byte,find,gsub,format=string.byte,string.find,string.gsub,string.format
local concat=table.concat
local type=type
if string.find(os.getenv("PATH") or "",";",1,true) then
 io.fileseparator,io.pathseparator="\\",";"
else
 io.fileseparator,io.pathseparator="/",":"
end
local large=0x01000000 
local medium=0x00100000 
local small=0x00020000
local function readall(f)
 local size=f:seek("end")
 if size>0 then
  f:seek("set",0)
  return f:read(size)
 else
  return ""
 end
end
io.readall=readall
function io.loaddata(filename,textmode) 
 local f=open(filename,(textmode and 'r') or 'rb')
 if f then
  local size=f:seek("end")
  local data=nil
  if size>0 then
   f:seek("set",0)
   data=f:read(size)
  end
  f:close()
  return data
 end
end
function io.copydata(source,target,action)
 local f=open(source,"rb")
 if f then
  local g=open(target,"wb")
  if g then
   local size=f:seek("end")
   if size>0 then
    f:seek("set",0)
    local data=f:read(size)
    if action then
     data=action(data)
    end
    if data then
     g:write(data)
    end
   end
   g:close()
  end
  f:close()
  flush()
 end
end
function io.savedata(filename,data,joiner)
 local f=open(filename,"wb")
 if f then
  if type(data)=="table" then
   f:write(concat(data,joiner or ""))
  elseif type(data)=="function" then
   data(f)
  else
   f:write(data or "")
  end
  f:close()
  flush()
  return true
 else
  return false
 end
end
if fio and fio.readline then
 local readline=fio.readline
 function io.loadlines(filename,n) 
  local f=open(filename,'r')
  if not f then
  elseif n then
   local lines={}
   for i=1,n do
    local line=readline(f)
    if line then
     lines[i]=line
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
   local line=readline(f)
   f:close()
   if line and #line>0 then
    return line
   end
  end
 end
else
 function io.loadlines(filename,n) 
  local f=open(filename,'r')
  if not f then
  elseif n then
   local lines={}
   for i=1,n do
    local line=f:read("*lines")
    if line then
     lines[i]=line
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
end
function io.loadchunk(filename,n)
 local f=open(filename,'rb')
 if f then
  local data=f:read(n or 1024)
  f:close()
  if #data>0 then
   return data
  end
 end
end
function io.exists(filename)
 local f=open(filename)
 if f==nil then
  return false
 else
  f:close()
  return true
 end
end
function io.size(filename)
 local f=open(filename)
 if f==nil then
  return 0
 else
  local s=f:seek("end")
  f:close()
  return s
 end
end
local function noflines(f)
 if type(f)=="string" then
  local f=open(filename)
  if f then
   local n=f and noflines(f) or 0
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
io.noflines=noflines
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
  write(question)
  if options then
   write(format(" [%s]",concat(options,"|")))
  end
  if default then
   write(format(" [%s]",default))
  end
  write(format(" "))
  flush()
  local answer=read()
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
  return 0x100*a+b
 elseif n==3 then
  local a,b,c=byte(f:read(3),1,3)
  return 0x10000*a+0x100*b+c
 elseif n==4 then
  local a,b,c,d=byte(f:read(4),1,4)
  return 0x1000000*a+0x10000*b+0x100*c+d
 elseif n==8 then
  local a,b=readnumber(f,4),readnumber(f,4)
  return 0x100*a+b
 elseif n==12 then
  local a,b,c=readnumber(f,4),readnumber(f,4),readnumber(f,4)
  return 0x10000*a+0x100*b+c
 elseif n==-2 then
  local b,a=byte(f:read(2),1,2)
  return 0x100*a+b
 elseif n==-3 then
  local c,b,a=byte(f:read(3),1,3)
  return 0x10000*a+0x100*b+c
 elseif n==-4 then
  local d,c,b,a=byte(f:read(4),1,4)
  return 0x1000000*a+0x10000*b+0x100*c+d
 elseif n==-8 then
  local h,g,f,e,d,c,b,a=byte(f:read(8),1,8)
  return 0x100000000000000*a+0x1000000000000*b+0x10000000000*c+0x100000000*d+0x1000000*e+0x10000*f+0x100*g+h
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


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-number"] = package.loaded["l-number"] or true

-- original size: 5720, stripped down to: 2176

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
local floor=math.floor
number=number or {}
local number=number
if bit32 then
 local bextract=bit32.extract
 local t={
  "0","0","0","0","0","0","0","0",
  "0","0","0","0","0","0","0","0",
  "0","0","0","0","0","0","0","0",
  "0","0","0","0","0","0","0","0",
 }
 function number.tobitstring(b,m,w)
  if not w then
   w=32
  end
  local n=w
  for i=0,w-1 do
   local v=bextract(b,i)
   local k=w-i
   if v==1 then
    n=k
    t[k]="1"
   else
    t[k]="0"
   end
  end
  if w then
   return concat(t,"",1,w)
  elseif m then
   m=33-m*8
   if m<1 then
    m=1
   end
   return concat(t,"",1,m)
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
function number.bytetodecimal(b)
 local d=floor(b*100/255+0.5)
 if d>100 then
  return 100
 elseif d<-100 then
  return -100
 else
  return d
 end
end
function number.decimaltobyte(d)
 local b=floor(d*255/100+0.5)
 if b>255 then
  return 255
 elseif b<-255 then
  return -255
 else
  return b
 end
end
function number.idiv(i,d)
 return floor(i/d) 
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-set"] = package.loaded["l-set"] or true

-- original size: 1923, stripped down to: 1044

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

-- original size: 19120, stripped down to: 10208

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
local type,setmetatable,tonumber,tostring=type,setmetatable,tonumber,tostring
do
 local selfdir=os.selfdir
 if selfdir=="" then
  selfdir=nil
 end
 if not selfdir then
  if arg then
   for i=1,#arg do
    local a=arg[i]
    if find(a,"^%-%-[c:]*texmfbinpath=") then
     selfdir=gsub(a,"^.-=","")
     break
    end
   end
  end
  if not selfdir then
   selfdir=os.selfbin or "luatex"
   if find(selfdir,"[/\\]") then
    selfdir=gsub(selfdir,"[/\\][^/\\]*$","")
   elseif os.getenv then
    local path=os.getenv("PATH")
    local name=gsub(selfdir,"^.*[/\\][^/\\]","")
    local patt="[^:]+"
    if os.type=="windows" then
     patt="[^;]+"
     name=name..".exe"
    end
    local isfile
    if lfs then
     local attributes=lfs.attributes
     isfile=function(name)
      local a=attributes(name,"mode")
      return a=="file" or a=="link" or nil
     end
    else
     local open=io.open
     isfile=function(name)
      local f=open(name)
      if f then
       f:close()
       return true
      end
     end
    end
    for p in gmatch(path,patt) do
     if isfile(p.."/"..name) then
      selfdir=p
      break
     end
    end
   end
  end
  os.selfdir=selfdir or "."
 end
end
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
local execute=os.execute
local iopopen=io.popen
local function resultof(command)
 local handle=iopopen(command,"r") 
 if handle then
  local result=handle:read("*all") or ""
  handle:close()
  return result
 else
  return ""
 end
end
os.resultof=resultof
function os.pipeto(command)
 return iopopen(command,"w") 
end
if not io.fileseparator then
 if find(os.getenv("PATH"),";",1,true) then
  io.fileseparator,io.pathseparator,os.type="\\",";",os.type or "windows"
 else
  io.fileseparator,io.pathseparator,os.type="/",":",os.type or "unix"
 end
end
os.type=os.type or (io.pathseparator==";"    and "windows") or "unix"
os.name=os.name or (os.type=="windows" and "mswin"  ) or "linux"
if os.type=="windows" then
 os.libsuffix,os.binsuffix,os.binsuffixes='dll','exe',{ 'exe','cmd','bat' }
else
 os.libsuffix,os.binsuffix,os.binsuffixes='so','',{ '' }
end
local launchers={
 windows="start %s",
 macosx="open %s",
 unix="xdg-open %s &> /dev/null &",
}
function os.launch(str)
 local command=format(launchers[os.name] or launchers.unix,str)
 execute(command)
end
local gettimeofday=os.gettimeofday or os.clock
os.gettimeofday=gettimeofday
local startuptime=gettimeofday()
function os.runtime()
 return gettimeofday()-startuptime
end
local resolvers=os.resolvers or {}
os.resolvers=resolvers
setmetatable(os,{ __index=function(t,k)
 local r=resolvers[k]
 return r and r(t,k) or nil 
end })
local name,platform=os.name or "linux",os.getenv("MTX_PLATFORM") or ""
if platform~="" then
 os.platform=platform
elseif os.type=="windows" then
 function resolvers.platform(t,k)
  local architecture=os.getenv("PROCESSOR_ARCHITECTURE") or ""
  local platform=""
  if find(architecture,"AMD64",1,true) then
   platform="win64"
  else
   platform="mswin"
  end
  os.setenv("MTX_PLATFORM",platform)
  os.platform=platform
  return platform
 end
elseif name=="linux" then
 function resolvers.platform(t,k)
  local architecture=os.getenv("HOSTTYPE") or resultof("uname -m") or ""
  local platform=os.getenv("MTX_PLATFORM") or ""
  local musl=find(os.selfdir or "","linuxmusl")
  if platform~="" then
  elseif find(architecture,"x86_64",1,true) then
   platform=musl and "linuxmusl" or "linux-64"
  elseif find(architecture,"ppc",1,true) then
   platform="linux-ppc"
  else
   platform=musl and "linuxmusl" or "linux"
  end
  os.setenv("MTX_PLATFORM",platform)
  os.platform=platform
  return platform
 end
elseif name=="macosx" then
 function resolvers.platform(t,k)
  local architecture=resultof("echo $HOSTTYPE") or ""
  local platform=""
  if architecture=="" then
   platform="osx-intel"
  elseif find(architecture,"i386",1,true) then
   platform="osx-intel"
  elseif find(architecture,"x86_64",1,true) then
   platform="osx-64"
  else
   platform="osx-ppc"
  end
  os.setenv("MTX_PLATFORM",platform)
  os.platform=platform
  return platform
 end
elseif name=="sunos" then
 function resolvers.platform(t,k)
  local architecture=resultof("uname -m") or ""
  local platform=""
  if find(architecture,"sparc",1,true) then
   platform="solaris-sparc"
  else 
   platform="solaris-intel"
  end
  os.setenv("MTX_PLATFORM",platform)
  os.platform=platform
  return platform
 end
elseif name=="freebsd" then
 function resolvers.platform(t,k)
  local architecture=resultof("uname -m") or ""
  local platform=""
  if find(architecture,"amd64",1,true) then
   platform="freebsd-amd64"
  else
   platform="freebsd"
  end
  os.setenv("MTX_PLATFORM",platform)
  os.platform=platform
  return platform
 end
elseif name=="kfreebsd" then
 function resolvers.platform(t,k)
  local architecture=os.getenv("HOSTTYPE") or resultof("uname -m") or ""
  local platform=""
  if find(architecture,"x86_64",1,true) then
   platform="kfreebsd-amd64"
  else
   platform="kfreebsd-i386"
  end
  os.setenv("MTX_PLATFORM",platform)
  os.platform=platform
  return platform
 end
else
 function resolvers.platform(t,k)
  local platform="linux"
  os.setenv("MTX_PLATFORM",platform)
  os.platform=platform
  return platform
 end
end
os.newline=name=="windows" and "\013\010" or "\010" 
function resolvers.bits(t,k)
 local bits=find(os.platform,"64",1,true) and 64 or 32
 os.bits=bits
 return bits
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
 d=d or ((tonumber(date("%H")) or 0)-(tonumber(date("!%H")) or 0))
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
local lasttime=nil
local lastdate=nil
function os.fulltime(t,default)
 t=t and tonumber(t) or 0
 if t>0 then
 elseif default then
  return default
 else
  t=time()
 end
 if t~=lasttime then
  lasttime=t
  lastdate=format(timeformat,date(dateformat))
 end
 return lastdate
end
local dateformat="%Y-%m-%d %H:%M:%S"
local lasttime=nil
local lastdate=nil
function os.localtime(t,default)
 t=t and tonumber(t) or 0
 if t>0 then
 elseif default then
  return default
 else
  t=time()
 end
 if t~=lasttime then
  lasttime=t
  lastdate=date(dateformat,t)
 end
 return lastdate
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
if not os.sleep then
 local socket=socket
 function os.sleep(n)
  if not socket then
   socket=require("socket")
  end
  socket.sleep(n)
 end
end
local function isleapyear(year)
 return (year%4==0) and (year%100~=0 or year%400==0)
end
os.isleapyear=isleapyear
local days={ 31,28,31,30,31,30,31,31,30,31,30,31 }
local function nofdays(year,month)
 if not month then
  return isleapyear(year) and 365 or 364
 else
  return month==2 and isleapyear(year) and 29 or days[month]
 end
end
os.nofdays=nofdays
function os.weekday(day,month,year)
 return date("%w",time { year=year,month=month,day=day })+1
end
function os.validdate(year,month,day)
 if month<1 then
  month=1
 elseif month>12 then
  month=12
 end
 if day<1 then
  day=1
 else
  local max=nofdays(year,month)
  if day>max then
   day=max
  end
 end
 return year,month,day
end
function os.date(fmt,...)
 if not fmt then
  fmt="%Y-%m-%d %H:%M"
 end
 return date(fmt,...)
end
local osexit=os.exit
local exitcode=nil
function os.setexitcode(code)
 exitcode=code
end
function os.exit(c)
 if exitcode~=nil then
  return osexit(exitcode)
 end
 if c~=nil then
  return osexit(c)
 end
 return osexit()
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-file"] = package.loaded["l-file"] or true

-- original size: 22175, stripped down to: 10302

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
local insert,concat=table.insert,table.concat
local match,find,gmatch=string.match,string.find,string.gmatch
local lpegmatch=lpeg.match
local getcurrentdir,attributes=lfs.currentdir,lfs.attributes
local checkedsplit=string.checkedsplit
local P,R,S,C,Cs,Cp,Cc,Ct=lpeg.P,lpeg.R,lpeg.S,lpeg.C,lpeg.Cs,lpeg.Cp,lpeg.Cc,lpeg.Ct
local attributes=lfs.attributes
function lfs.isdir(name)
 if name then
  return attributes(name,"mode")=="directory"
 end
end
function lfs.isfile(name)
 if name then
  local a=attributes(name,"mode")
  return a=="file" or a=="link" or nil
 end
end
function lfs.isfound(name)
 if name then
  local a=attributes(name,"mode")
  return (a=="file" or a=="link") and name or nil
 end
end
function lfs.modification(name)
 return name and attributes(name,"modification") or nil
end
if sandbox then
 sandbox.redefine(lfs.isfile,"lfs.isfile")
 sandbox.redefine(lfs.isdir,"lfs.isdir")
 sandbox.redefine(lfs.isfound,"lfs.isfound")
end
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
local pattern=(noslashes^0*slashes)^0*noperiod^1*((period*C(noperiod^1))^1)*-1+Cc("")
local function suffixesonly(name)
 if name then
  return lpegmatch(pattern,name)
 else
  return ""
 end
end
file.pathpart=pathpart
file.basename=basename
file.nameonly=nameonly
file.suffixonly=suffixonly
file.suffix=suffixonly
file.suffixesonly=suffixesonly
file.suffixes=suffixesonly
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
 if str then
  return lpegmatch(pattern_d,str) 
 else
  return "",str 
 end
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
if lfs.isreadablefile and lfs.iswritablefile then
 file.is_readable=lfs.isreadablefile
 file.is_writable=lfs.iswritablefile
else
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
local someslash=S("\\/")
local stripper=Cs(P(fwslash)^0/""*reslasher)
local isnetwork=someslash*someslash*(1-someslash)+(1-fwslash-colon)^1*colon
local isroot=fwslash^1*-1
local hasroot=fwslash^1
local reslasher=lpeg.replacer(S("\\/"),"/")
local deslasher=lpeg.replacer(S("\\/")^1,"/")
function file.join(one,two,three,...)
 if not two then
  return one=="" and one or lpegmatch(reslasher,one)
 end
 if one=="" then
  return lpegmatch(stripper,three and concat({ two,three,... },"/") or two)
 end
 if lpegmatch(isnetwork,one) then
  local one=lpegmatch(reslasher,one)
  local two=lpegmatch(deslasher,three and concat({ two,three,... },"/") or two)
  if lpegmatch(hasroot,two) then
   return one..two
  else
   return one.."/"..two
  end
 elseif lpegmatch(isroot,one) then
  local two=lpegmatch(deslasher,three and concat({ two,three,... },"/") or two)
  if lpegmatch(hasroot,two) then
   return two
  else
   return "/"..two
  end
 else
  return lpegmatch(deslasher,concat({  one,two,three,... },"/"))
 end
end
local drivespec=R("az","AZ")^1*colon
local anchors=fwslash+drivespec
local untouched=periods+(1-period)^1*P(-1)
local mswindrive=Cs(drivespec*(bwslash/"/"+fwslash)^0)
local mswinuncpath=(bwslash+fwslash)*(bwslash+fwslash)*Cc("//")
local splitstarter=(mswindrive+mswinuncpath+Cc(false))*Ct(lpeg.splitat(S("/\\")^1))
local absolute=fwslash
function file.collapsepath(str,anchor) 
 if not str then
  return
 end
 if anchor==true and not lpegmatch(anchors,str) then
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
  newelements=concat(newelements,'/')
  if anchor=="." and find(str,"^%./") then
   return "./"..newelements
  else
   return newelements
  end
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
local loaddata=io.loaddata
local savedata=io.savedata
file.readdata=loaddata
file.savedata=savedata
function file.copy(oldname,newname)
 if oldname and newname then
  local data=loaddata(oldname)
  if data and data~="" then
   savedata(newname,data)
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
function lfs.mkdirs(path)
 local full=""
 for sub in gmatch(path,"(/*[^\\/]+)") do 
  full=full..sub
  lfs.mkdir(full)
 end
end
function file.withinbase(path) 
 local l=0
 if not find(path,"^/") then
  path="/"..path
 end
 for dir in gmatch(path,"/([^/]+)") do
  if dir==".." then
   l=l-1
  elseif dir~="." then
   l=l+1
  end
  if l<0 then
   return false
  end
 end
 return true
end
local symlinkattributes=lfs.symlinkattributes
function lfs.readlink(name)
 return symlinkattributes(name,"target") or nil
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-gzip"] = package.loaded["l-gzip"] or true

-- original size: 5115, stripped down to: 1699

if not modules then modules={} end modules ['l-gzip']={
 version=1.001,
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
gzip=gzip or {} 
if not zlib then
 zlib=xzip 
elseif not xzip then
 xzip=zlib
end
if zlib then
 local suffix=file.suffix
 local suffixes=file.suffixes
 local find=string.find
 local openfile=io.open
 local gzipwindow=15+16 
 local gziplevel=3
 local identifier="^\x1F\x8B\x08"
 local compress=zlib.compress
 local decompress=zlib.decompress
 function gzip.load(filename)
  local f=openfile(filename,"rb")
  if not f then
  else
   local data=f:read("*all")
   f:close()
   if data and data~="" then
    if suffix(filename)=="gz" then
     data=decompress(data,gzipwindow)
    end
    return data
   end
  end
 end
 function gzip.save(filename,data,level)
  if suffix(filename)~="gz" then
   filename=filename..".gz"
  end
  local f=openfile(filename,"wb")
  if f then
   data=compress(data or "",level or gziplevel,nil,gzipwindow)
   f:write(data)
   f:close()
   return #data
  end
 end
 function gzip.suffix(filename)
  local suffix,extra=suffixes(filename)
  local gzipped=extra=="gz"
  return suffix,gzipped
 end
 function gzip.compressed(s)
  return s and find(s,identifier)
 end
 function gzip.compress(s,level)
  if s and not find(s,identifier) then 
   if not level then
    level=gziplevel
   elseif level<=0 then
    return s
   elseif level>9 then
    level=9
   end
   return compress(s,level or gziplevel,nil,gzipwindow) or s
  end
 end
 function gzip.decompress(s)
  if s and find(s,identifier) then
   return decompress(s,gzipwindow)
  else
   return s
  end
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-md5"] = package.loaded["l-md5"] or true

-- original size: 3414, stripped down to: 2307

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
local gsub=string.gsub
local modification,isfile,touch=lfs.modification,lfs.isfile,lfs.touch
local loaddata,savedata=io.loaddata,io.savedata
do
 local patterns=lpeg and lpeg.patterns
 if patterns then
  local bytestoHEX=patterns.bytestoHEX
  local bytestohex=patterns.bytestohex
  local bytestodec=patterns.bytestodec
  local lpegmatch=lpeg.match
  local md5sum=md5.sum
  if not md5.HEX then function md5.HEX(str) if str then return lpegmatch(bytestoHEX,md5sum(str)) end end end
  if not md5.hex then function md5.hex(str) if str then return lpegmatch(bytestohex,md5sum(str)) end end end
  if not md5.dec then function md5.dec(str) if str then return lpegmatch(bytestodec,md5sum(str)) end end end
  md5.sumhexa=md5.hex
  md5.sumHEXA=md5.HEX
 end
end
local md5HEX=md5.HEX
function file.needsupdating(oldname,newname,threshold) 
 local oldtime=modification(oldname)
 if oldtime then
  local newtime=modification(newname)
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
 local oldtime=modification(oldname)
 if oldtime and isfile(newname) then
  touch(newname,oldtime,oldtime)
 end
end
local function checksum(name)
 if md5 then
  local data=loaddata(name)
  if data then
   return md5HEX(data)
  end
 end
 return nil
end
file.checksum=checksum
function file.loadchecksum(name)
 if md5 then
  local data=loaddata(name..".md5")
  return data and (gsub(data,"%s",""))
 end
 return nil
end
function file.savechecksum(name,checksum)
 if not checksum then checksum=checksum(name) end
 if checksum then
  savedata(name..".md5",checksum)
  return checksum
 end
 return nil
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-sha"] = package.loaded["l-sha"] or true

-- original size: 1085, stripped down to: 969

if not modules then modules={} end modules ['l-sha']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
if sha2 then
 local lpegmatch=lpeg.match
 local lpegpatterns=lpeg.patterns
 local bytestohex=lpegpatterns.bytestohex
 local bytestoHEX=lpegpatterns.bytestoHEX
 local digest256=sha2.digest256
 local digest384=sha2.digest384
 local digest512=sha2.digest512
 sha2.hash256=function(str) return lpegmatch(bytestohex,digest256(str)) end
 sha2.hash384=function(str) return lpegmatch(bytestohex,digest384(str)) end
 sha2.hash512=function(str) return lpegmatch(bytestohex,digest512(str)) end
 sha2.HASH256=function(str) return lpegmatch(bytestoHEX,digest256(str)) end
 sha2.HASH384=function(str) return lpegmatch(bytestoHEX,digest384(str)) end
 sha2.HASH512=function(str) return lpegmatch(bytestoHEX,digest512(str)) end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-url"] = package.loaded["l-url"] or true

-- original size: 14755, stripped down to: 6981

if not modules then modules={} end modules ['l-url']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local char,format,byte=string.char,string.format,string.byte
local concat=table.concat
local tonumber,type,next=tonumber,type,next
local P,C,R,S,Cs,Cc,Ct,Cf,Cg,V=lpeg.P,lpeg.C,lpeg.R,lpeg.S,lpeg.Cs,lpeg.Cc,lpeg.Ct,lpeg.Cf,lpeg.Cg,lpeg.V
local lpegmatch,lpegpatterns,replacer=lpeg.match,lpeg.patterns,lpeg.replacer
local sortedhash=table.sortedhash
url=url or {}
local url=url
local unescapes={}
local escapes={}
setmetatable(unescapes,{ __index=function(t,k)
 local v=char(tonumber(k,16))
 t[k]=v
 return v
end })
setmetatable(escapes,{ __index=function(t,k)
 local v=format("%%%02X",byte(k))
 t[k]=v
 return v
end })
local colon=P(":")
local qmark=P("?")
local hash=P("#")
local slash=P("/")
local atsign=P("@")
local percent=P("%")
local endofstring=P(-1)
local hexdigit=R("09","AF","af")
local plus=P("+")
local nothing=Cc("")
local okay=R("09","AZ","az")+S("-_.,:=+*~!'()@&$")
local escapedchar=(percent*C(hexdigit*hexdigit))/unescapes
local unescapedchar=P(1)/escapes
local escaped=(plus/" ")+escapedchar 
local noslash=P("/")/""
local plustospace=P("+")/" "
local decoder=Cs((
     plustospace+escapedchar+P("\r\n")/"\n"+P(1)
    )^0 )
local encoder=Cs((
     R("09","AZ","az")^1+S("-./_")^1+P(" ")/"+"+P("\n")/"\r\n"+unescapedchar
    )^0 )
lpegpatterns.urldecoder=decoder
lpegpatterns.urlencoder=encoder
function url.decode  (str) return str and lpegmatch(decoder,str) or str end
function url.encode  (str) return str and lpegmatch(encoder,str) or str end
function url.unescape(str) return str and lpegmatch(unescaper,str) or str end
local schemestr=Cs((escaped+(1-colon-slash-qmark-hash))^2)
local authoritystr=Cs((escaped+(1-   slash-qmark-hash))^0)
local pathstr=Cs((escaped+(1-   qmark-hash))^0)
local querystr=Cs(((1-      hash))^0)
local fragmentstr=Cs((escaped+(1-     endofstring))^0)
local scheme=schemestr*colon+nothing
local authority=slash*slash*authoritystr+nothing
local path=slash*pathstr+nothing
local query=qmark*querystr+nothing
local fragment=hash*fragmentstr+nothing
local validurl=scheme*authority*path*query*fragment
local parser=Ct(validurl)
lpegpatterns.url=validurl
lpegpatterns.urlsplitter=parser
local escaper=Cs((R("09","AZ","az")^1+P(" ")/"%%20"+S("-./_:")^1+P(1)/escapes)^0) 
local unescaper=Cs((escapedchar+1)^0)
local getcleaner=Cs((P("+++")/"%%2B"+P("+")/"%%20"+P(1))^1)
lpegpatterns.urlunescaped=escapedchar
lpegpatterns.urlescaper=escaper
lpegpatterns.urlunescaper=unescaper
lpegpatterns.urlgetcleaner=getcleaner
function url.unescapeget(str)
 return lpegmatch(getcleaner,str)
end
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
local key=Cs(((plustospace+escapedchar+1)-equal     )^0)
local value=Cs(((plustospace+escapedchar+1)-amp-endofstring)^0)
local splitquery=Cf (Ct("")*P { "sequence",
 sequence=V("pair")*(amp*V("pair"))^0,
 pair=Cg(key*equal*value),
},rawset)
local userpart=(1-atsign-colon)^1
local serverpart=(1-colon)^1
local splitauthority=((Cs(userpart)*colon*Cs(userpart)+Cs(userpart)*Cc(nil))*atsign+Cc(nil)*Cc(nil))*Cs(serverpart)*(colon*(serverpart/tonumber)+Cc(nil))
local function hashed(str) 
 if not str or str=="" then
  return {
   scheme="invalid",
   original=str,
  }
 end
 local detailed=split(str)
 local rawscheme=""
 local rawquery=""
 local somescheme=false
 local somequery=false
 if detailed then
  rawscheme=detailed[1]
  rawquery=detailed[4]
  somescheme=rawscheme~=""
  somequery=rawquery~=""
 end
 if not somescheme and not somequery then
  return {
   scheme="file",
   authority="",
   path=str,
   query="",
   fragment="",
   original=str,
   noscheme=true,
   filename=str,
  }
 end
 local authority=detailed[2]
 local path=detailed[3]
 local filename  
 local username  
 local password  
 local host   
 local port   
 if authority~="" then
  username,password,host,port=lpegmatch(splitauthority,authority)
 end
 if authority=="" then
  filename=path
 elseif path=="" then
  filename=""
 else
  filename=authority.."/"..path
 end
 return {
  scheme=rawscheme,
  authority=authority,
  path=path,
  query=lpegmatch(unescaper,rawquery),
  queries=lpegmatch(splitquery,rawquery),
  fragment=detailed[5],
  original=str,
  noscheme=false,
  filename=filename,
  host=host,
  port=port,
 }
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
 local result,r={},0
 local scheme=hash.scheme
 local authority=hash.authority
 local path=hash.path
 local queries=hash.queries
 local fragment=hash.fragment
 if scheme and scheme~="" then
  r=r+1;result[r]=lpegmatch(escaper,scheme)
  r=r+1;result[r]="://"
 end
 if authority and authority~="" then
  r=r+1;result[r]=lpegmatch(escaper,authority)
 end
 if path and path~="" then
  r=r+1;result[r]="/"
  r=r+1;result[r]=lpegmatch(escaper,path)
 end
 if queries then
  local done=false
  for k,v in sortedhash(queries) do
   r=r+1;result[r]=done and "&" or "?"
   r=r+1;result[r]=lpegmatch(escaper,k) 
   r=r+1;result[r]="="
   r=r+1;result[r]=lpegmatch(escaper,v) 
   done=true
  end
 end
 if fragment and fragment~="" then
  r=r+1;result[r]="#"
  r=r+1;result[r]=lpegmatch(escaper,fragment)
 end
 return concat(result)
end
local pattern=Cs(slash^-1/""*R("az","AZ")*((S(":|")/":")+P(":"))*slash*P(1)^0)
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

-- original size: 18253, stripped down to: 10816

if not modules then modules={} end modules ['l-dir']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local type,select=type,select
local find,gmatch,match,gsub,sub=string.find,string.gmatch,string.match,string.gsub,string.sub
local concat,insert,remove,unpack=table.concat,table.insert,table.remove,table.unpack
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
local mkdir=lfs.mkdir
local onwindows=os.type=="windows" or find(os.getenv("PATH"),";",1,true)
if onwindows then
 local tricky=S("/\\")*P(-1)
 isdir=function(name)
  if lpegmatch(tricky,name) then
   return attributes(name,"mode")=="directory"
  else
   return attributes(name.."/.","mode")=="directory"
  end
 end
 isfile=function(name)
  return attributes(name,"mode")=="file"
 end
 lfs.isdir=isdir
 lfs.isfile=isfile
else
 isdir=function(name)
  return attributes(name,"mode")=="directory"
 end
 isfile=function(name)
  return attributes(name,"mode")=="file"
 end
 lfs.isdir=isdir
 lfs.isfile=isfile
end
function dir.current()
 return (gsub(currentdir(),"\\","/"))
end
local function glob_pattern_function(path,patt,recurse,action)
 if isdir(path) then
  local usedpath
  if path=="/" then
   usedpath="/."
  elseif not find(path,"/$") then
   usedpath=path.."/."
   path=path.."/"
  else
   usedpath=path
  end
  local dirs
  local nofdirs=0
  for name,mode,size,time in walkdir(usedpath) do
   if name~="." and name~=".." then
    local full=path..name
    if mode==nil then
     mode=attributes(full,'mode')
    end
    if mode=='file' then
     if not patt or find(full,patt) then
      action(full,size,time)
     end
    elseif recurse and mode=="directory" then
     if dirs then
      nofdirs=nofdirs+1
      dirs[nofdirs]=full
     else
      nofdirs=1
      dirs={ full }
     end
    end
   end
  end
  if dirs then
   for i=1,nofdirs do
    glob_pattern_function(dirs[i],patt,recurse,action)
   end
  end
 end
end
local function glob_pattern_table(path,patt,recurse,result)
 if not result then
  result={}
 end
 local usedpath
 if path=="/" then
  usedpath="/."
 elseif not find(path,"/$") then
  usedpath=path.."/."
  path=path.."/"
 else
  usedpath=path
 end
 local dirs
 local nofdirs=0
 local noffiles=#result
 for name,mode in walkdir(usedpath) do
  if name~="." and name~=".." then
   local full=path..name
   if mode==nil then
    mode=attributes(full,'mode')
   end
   if mode=='file' then
    if not patt or find(full,patt) then
     noffiles=noffiles+1
     result[noffiles]=full
    end
   elseif recurse and mode=="directory" then
    if dirs then
     nofdirs=nofdirs+1
     dirs[nofdirs]=full
    else
     nofdirs=1
     dirs={ full }
    end
   end
  end
 end
 if dirs then
  for i=1,nofdirs do
   glob_pattern_table(dirs[i],patt,recurse,result)
  end
 end
 return result
end
local function globpattern(path,patt,recurse,method)
 local kind=type(method)
 if patt and sub(patt,1,-3)==path then
  patt=false
 end
 local okay=isdir(path)
 if kind=="function" then
  return okay and glob_pattern_function(path,patt,recurse,method) or {}
 elseif kind=="table" then
  return okay and glob_pattern_table(path,patt,recurse,method) or method
 else
  return okay and glob_pattern_table(path,patt,recurse,{}) or {}
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
  if not find(path,"/$") then
   path=path..'/'
  end
  for name in scanner,first do 
   if name=="." then
   elseif name==".." then
   else
    local full=path..name
    local attr=attributes(full)
    local mode=attr.mode
    if mode=='file' then
     if find(full,patt) then
      result[name]=attr
     end
    elseif recurse and mode=="directory" then
     attr.list=collectpattern(full,patt,recurse)
     result[name]=attr
    end
   end
  end
 end
 return result
end
dir.collectpattern=collectpattern
local separator,pattern
if onwindows then 
 local slash=S("/\\")/"/"
 pattern={
  [1]=(Cs(P(".")+slash^1)+Cs(R("az","AZ")*P(":")*slash^0)+Cc("./"))*V(2)*V(3),
  [2]=Cs(((1-S("*?/\\"))^0*slash)^0),
  [3]=Cs(P(1)^0)
 }
else
 pattern={
  [1]=(C(P(".")+P("/")^1)+Cc("./"))*V(2)*V(3),
  [2]=C(((1-S("*?/"))^0*P("/"))^0),
  [3]=C(P(1)^0)
 }
end
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
   local root,path,base=lpegmatch(pattern,str) 
   if root and path and base then
    local recurse=find(base,"**",1,true) 
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
   local root,path,base=lpegmatch(pattern,str) 
   if root and path and base then
    local recurse=find(base,"**",1,true) 
    local start=root..path
    local result=lpegmatch(filter,start..base)
    return globpattern(start,result,recurse,t)
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
 for name,mode in walkdir(path) do
  if find(name,"^%.") then
  else
   if mode==nil then
    mode=attributes(name,'mode')
   end
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
local function globdirs(path,recurse,func,files) 
 if type(func)=="string" then
  local s=func
  func=function(name) return find(name,s) end
 end
 files=files or {}
 local noffiles=#files
 for name,mode in walkdir(path) do
  if find(name,"^%.") then
  else
   if mode==nil then
    mode=attributes(name,'mode')
   end
   if mode=="directory" then
    if not func or func(name) then
     noffiles=noffiles+1
     files[noffiles]=path.."/"..name
     if recurse then
      globdirs(path.."/"..name,recurse,func,files)
     end
    end
   end
  end
 end
 return files
end
dir.globdirs=globdirs
function dir.ls(pattern)
 return concat(glob(pattern),"\n")
end
local make_indeed=true 
if onwindows then
 function dir.mkdirs(...)
  local n=select("#",...)
  local str
  if n==1 then
   str=select(1,...)
   if isdir(str) then
    return str,true
   end
  else
   str=""
   for i=1,n do
    local s=select(i,...)
    if s=="" then
    elseif str=="" then
     str=s
    else
     str=str.."/"..s
    end
   end
  end
  local pth=""
  local drive=false
  local first,middle,last=match(str,"^(//)(//*)(.*)$")
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
    mkdir(pth)
   end
  end
  return pth,(isdir(pth)==true)
 end
else
 function dir.mkdirs(...)
  local n=select("#",...)
  local str,pth
  if n==1 then
   str=select(1,...)
   if isdir(str) then
    return str,true
   end
  else
   str=""
   for i=1,n do
    local s=select(i,...)
    if s and s~="" then 
     if str~="" then
      str=str.."/"..s
     else
      str=s
     end
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
     mkdir(pth)
    end
   end
  else
   pth="."
   for s in gmatch(str,"[^/]+") do
    pth=pth.."/"..s
    if make_indeed and not isdir(pth) then
     mkdir(pth)
    end
   end
  end
  return pth,(isdir(pth)==true)
 end
end
dir.makedirs=dir.mkdirs
do
 local chdir=sandbox and sandbox.original(chdir) or chdir
 if onwindows then
  local xcurrentdir=dir.current
  function dir.expandname(str) 
   local first,nothing,last=match(str,"^(//)(//*)(.*)$")
   if first then
    first=xcurrentdir().."/" 
   end
   if not first then
    first,last=match(str,"^(//)/*(.*)$")
   end
   if not first then
    first,last=match(str,"^([a-zA-Z]:)(.*)$")
    if first and not find(last,"^/") then
     local d=currentdir() 
     if chdir(first) then
      first=xcurrentdir() 
     end
     chdir(d)
    end
   end
   if not first then
    first,last=xcurrentdir(),str
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
end
file.expandname=dir.expandname 
local stack={}
function dir.push(newdir)
 local curdir=currentdir()
 insert(stack,curdir)
 if newdir and newdir~="" and chdir(newdir) then
  return newdir
 else
  return curdir
 end
end
function dir.pop()
 local d=remove(stack)
 if d then
  chdir(d)
 end
 return d
end
local function found(...) 
 for i=1,select("#",...) do
  local path=select(i,...)
  local kind=type(path)
  if kind=="string" then
   if isdir(path) then
    return path
   end
  elseif kind=="table" then
   local path=found(unpack(path))
   if path then
    return path
   end
  end
 end
end
dir.found=found


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-boolean"] = package.loaded["l-boolean"] or true

-- original size: 1850, stripped down to: 1498

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
 if  str==nil then
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
function string.is_boolean(str,default,strict)
 if type(str)=="string" then
  if str=="true" or str=="yes" or str=="on" or str=="t" or (not strict and str=="1") then
   return true
  elseif str=="false" or str=="no" or str=="off" or str=="f" or (not strict and str=="0") then
   return false
  end
 end
 return default
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-unicode"] = package.loaded["l-unicode"] or true

-- original size: 41281, stripped down to: 17261

if not modules then modules={} end modules ['l-unicode']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
utf=utf or {}
unicode=nil
if not string.utfcharacters then
 local gmatch=string.gmatch
 function string.characters(str)
  return gmatch(str,".[\128-\191]*")
 end
end
utf.characters=string.utfcharacters
local type=type
local char,byte,format,sub,gmatch=string.char,string.byte,string.format,string.sub,string.gmatch
local concat=table.concat
local P,C,R,Cs,Ct,Cmt,Cc,Carg,Cp=lpeg.P,lpeg.C,lpeg.R,lpeg.Cs,lpeg.Ct,lpeg.Cmt,lpeg.Cc,lpeg.Carg,lpeg.Cp
local lpegmatch=lpeg.match
local patterns=lpeg.patterns
local tabletopattern=lpeg.utfchartabletopattern
local bytepairs=string.bytepairs
local finder=lpeg.finder
local replacer=lpeg.replacer
local p_utftype=patterns.utftype
local p_utfstricttype=patterns.utfstricttype
local p_utfoffset=patterns.utfoffset
local p_utf8character=patterns.utf8character
local p_utf8char=patterns.utf8char
local p_utf8byte=patterns.utf8byte
local p_utfbom=patterns.utfbom
local p_newline=patterns.newline
local p_whitespace=patterns.whitespace
if not utf.char then
 utf.char=string.utfcharacter or (utf8 and utf8.char)
 if not utf.char then
  local char=string.char
  if bit32 then
   local rshift=bit32.rshift
   function utf.char(n)
    if n<0x80 then
     return char(n)
    elseif n<0x800 then
     return char(
      0xC0+rshift(n,6),
      0x80+(n%0x40)
     )
    elseif n<0x10000 then
     return char(
      0xE0+rshift(n,12),
      0x80+(rshift(n,6)%0x40),
      0x80+(n%0x40)
     )
    elseif n<0x200000 then
     return char(
      0xF0+rshift(n,18),
      0x80+(rshift(n,12)%0x40),
      0x80+(rshift(n,6)%0x40),
      0x80+(n%0x40)
     )
    else
     return ""
    end
   end
  else
   local floor=math.floor
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
 end
end
if not utf.byte then
 utf.byte=string.utfvalue or (utf8 and utf8.codepoint)
 if not utf.byte then
  function utf.byte(c)
   return lpegmatch(p_utf8byte,c)
  end
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
 utf.len=string.utflength or (utf8 and utf8.len)
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
 local pattern_zero=Cmt(p_utf8character,slide_zero)^0
 local pattern_one=Cmt(p_utf8character,slide_one )^0
 local pattern_two=Cmt(p_utf8character,slide_two )^0
 local pattern_first=C(p_utf8character)
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
  if start==1 and stop==1 then
   return lpegmatch(pattern_first,str) or ""
  elseif start>stop then
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
function utf.remapper(mapping,option,action) 
 local variant=type(mapping)
 if variant=="table" then
  action=action or mapping
  if option=="dynamic" then
   local pattern=false
   table.setmetatablenewindex(mapping,function(t,k,v) rawset(t,k,v) pattern=false end)
   return function(str)
    if not str or str=="" then
     return ""
    else
     if not pattern then
      pattern=Cs((tabletopattern(mapping)/action+p_utf8character)^0)
     end
     return lpegmatch(pattern,str)
    end
   end
  elseif option=="pattern" then
   return Cs((tabletopattern(mapping)/action+p_utf8character)^0)
  else
   local pattern=Cs((tabletopattern(mapping)/action+p_utf8character)^0)
   return function(str)
    if not str or str=="" then
     return ""
    else
     return lpegmatch(pattern,str)
    end
   end,pattern
  end
 elseif variant=="function" then
  if option=="pattern" then
   return Cs((p_utf8character/mapping+p_utf8character)^0)
  else
   local pattern=Cs((p_utf8character/mapping+p_utf8character)^0)
   return function(str)
    if not str or str=="" then
     return ""
    else
     return lpegmatch(pattern,str)
    end
   end,pattern
  end
 else
  return function(str)
   return str or ""
  end
 end
end
function utf.replacer(t) 
 local r=replacer(t,false,false,true)
 return function(str)
  return lpegmatch(r,str)
 end
end
function utf.subtituter(t) 
 local f=finder  (t)
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
local utfcharsplitter_ows=p_utfbom^-1*Ct(C(p_utf8character)^0)
local utfcharsplitter_iws=p_utfbom^-1*Ct((p_whitespace^1+C(p_utf8character))^0)
local utfcharsplitter_raw=Ct(C(p_utf8character)^0)
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
local utf16_to_utf8_be,utf16_to_utf8_le
local utf32_to_utf8_be,utf32_to_utf8_le
local utf_16_be_getbom=patterns.utfbom_16_be^-1
local utf_16_le_getbom=patterns.utfbom_16_le^-1
local utf_32_be_getbom=patterns.utfbom_32_be^-1
local utf_32_le_getbom=patterns.utfbom_32_le^-1
local utf_16_be_linesplitter=utf_16_be_getbom*lpeg.tsplitat(patterns.utf_16_be_nl)
local utf_16_le_linesplitter=utf_16_le_getbom*lpeg.tsplitat(patterns.utf_16_le_nl)
local utf_32_be_linesplitter=utf_32_be_getbom*lpeg.tsplitat(patterns.utf_32_be_nl)
local utf_32_le_linesplitter=utf_32_le_getbom*lpeg.tsplitat(patterns.utf_32_le_nl)
local more=0
local p_utf16_to_utf8_be=C(1)*C(1)/function(left,right)
 local now=256*byte(left)+byte(right)
 if more>0 then
  now=(more-0xD800)*0x400+(now-0xDC00)+0x10000
  more=0
  return utfchar(now)
 elseif now>=0xD800 and now<=0xDBFF then
  more=now
  return "" 
 else
  return utfchar(now)
 end
end
local p_utf16_to_utf8_le=C(1)*C(1)/function(right,left)
 local now=256*byte(left)+byte(right)
 if more>0 then
  now=(more-0xD800)*0x400+(now-0xDC00)+0x10000
  more=0
  return utfchar(now)
 elseif now>=0xD800 and now<=0xDBFF then
  more=now
  return "" 
 else
  return utfchar(now)
 end
end
local p_utf32_to_utf8_be=C(1)*C(1)*C(1)*C(1)/function(a,b,c,d)
 return utfchar(256*256*256*byte(a)+256*256*byte(b)+256*byte(c)+byte(d))
end
local p_utf32_to_utf8_le=C(1)*C(1)*C(1)*C(1)/function(a,b,c,d)
 return utfchar(256*256*256*byte(d)+256*256*byte(c)+256*byte(b)+byte(a))
end
p_utf16_to_utf8_be=P(true)/function() more=0 end*utf_16_be_getbom*Cs(p_utf16_to_utf8_be^0)
p_utf16_to_utf8_le=P(true)/function() more=0 end*utf_16_le_getbom*Cs(p_utf16_to_utf8_le^0)
p_utf32_to_utf8_be=P(true)/function() more=0 end*utf_32_be_getbom*Cs(p_utf32_to_utf8_be^0)
p_utf32_to_utf8_le=P(true)/function() more=0 end*utf_32_le_getbom*Cs(p_utf32_to_utf8_le^0)
patterns.utf16_to_utf8_be=p_utf16_to_utf8_be
patterns.utf16_to_utf8_le=p_utf16_to_utf8_le
patterns.utf32_to_utf8_be=p_utf32_to_utf8_be
patterns.utf32_to_utf8_le=p_utf32_to_utf8_le
utf16_to_utf8_be=function(s)
 if s and s~="" then
  return lpegmatch(p_utf16_to_utf8_be,s)
 else
  return s
 end
end
local utf16_to_utf8_be_t=function(t)
 if not t then
  return nil
 elseif type(t)=="string" then
  t=lpegmatch(utf_16_be_linesplitter,t)
 end
 for i=1,#t do
  local s=t[i]
  if s~="" then
   t[i]=lpegmatch(p_utf16_to_utf8_be,s)
  end
 end
 return t
end
utf16_to_utf8_le=function(s)
 if s and s~="" then
  return lpegmatch(p_utf16_to_utf8_le,s)
 else
  return s
 end
end
local utf16_to_utf8_le_t=function(t)
 if not t then
  return nil
 elseif type(t)=="string" then
  t=lpegmatch(utf_16_le_linesplitter,t)
 end
 for i=1,#t do
  local s=t[i]
  if s~="" then
   t[i]=lpegmatch(p_utf16_to_utf8_le,s)
  end
 end
 return t
end
utf32_to_utf8_be=function(s)
 if s and s~="" then
  return lpegmatch(p_utf32_to_utf8_be,s)
 else
  return s
 end
end
local utf32_to_utf8_be_t=function(t)
 if not t then
  return nil
 elseif type(t)=="string" then
  t=lpegmatch(utf_32_be_linesplitter,t)
 end
 for i=1,#t do
  local s=t[i]
  if s~="" then
   t[i]=lpegmatch(p_utf32_to_utf8_be,s)
  end
 end
 return t
end
utf32_to_utf8_le=function(s)
 if s and s~="" then
  return lpegmatch(p_utf32_to_utf8_le,s)
 else
  return s
 end
end
local utf32_to_utf8_le_t=function(t)
 if not t then
  return nil
 elseif type(t)=="string" then
  t=lpegmatch(utf_32_le_linesplitter,t)
 end
 for i=1,#t do
  local s=t[i]
  if s~="" then
   t[i]=lpegmatch(p_utf32_to_utf8_le,s)
  end
 end
 return t
end
utf.utf16_to_utf8_le_t=utf16_to_utf8_le_t
utf.utf16_to_utf8_be_t=utf16_to_utf8_be_t
utf.utf32_to_utf8_le_t=utf32_to_utf8_le_t
utf.utf32_to_utf8_be_t=utf32_to_utf8_be_t
utf.utf16_to_utf8_le=utf16_to_utf8_le
utf.utf16_to_utf8_be=utf16_to_utf8_be
utf.utf32_to_utf8_le=utf32_to_utf8_le
utf.utf32_to_utf8_be=utf32_to_utf8_be
function utf.utf8_to_utf8_t(t)
 return type(t)=="string" and lpegmatch(utflinesplitter,t) or t
end
function utf.utf16_to_utf8_t(t,endian)
 return endian and utf16_to_utf8_be_t(t) or utf16_to_utf8_le_t(t) or t
end
function utf.utf32_to_utf8_t(t,endian)
 return endian and utf32_to_utf8_be_t(t) or utf32_to_utf8_le_t(t) or t
end
if bit32 then
 local rshift=bit32.rshift
 local function little(b)
  if b<0x10000 then
   return char(b%256,rshift(b,8))
  else
   b=b-0x10000
   local b1=rshift(b,10)+0xD800
   local b2=b%1024+0xDC00
   return char(b1%256,rshift(b1,8),b2%256,rshift(b2,8))
  end
 end
 local function big(b)
  if b<0x10000 then
   return char(rshift(b,8),b%256)
  else
   b=b-0x10000
   local b1=rshift(b,10)+0xD800
   local b2=b%1024+0xDC00
   return char(rshift(b1,8),b1%256,rshift(b2,8),b2%256)
  end
 end
 local l_remap=Cs((p_utf8byte/little+P(1)/"")^0)
 local b_remap=Cs((p_utf8byte/big+P(1)/"")^0)
 local function utf8_to_utf16_be(str,nobom)
  if nobom then
   return lpegmatch(b_remap,str)
  else
   return char(254,255)..lpegmatch(b_remap,str)
  end
 end
 local function utf8_to_utf16_le(str,nobom)
  if nobom then
   return lpegmatch(l_remap,str)
  else
   return char(255,254)..lpegmatch(l_remap,str)
  end
 end
 utf.utf8_to_utf16_be=utf8_to_utf16_be
 utf.utf8_to_utf16_le=utf8_to_utf16_le
 function utf.utf8_to_utf16(str,littleendian,nobom)
  if littleendian then
   return utf8_to_utf16_le(str,nobom)
  else
   return utf8_to_utf16_be(str,nobom)
  end
 end
end
local pattern=Cs (
 (p_utf8byte/function(unicode    ) return format("0x%04X",unicode) end)*(p_utf8byte*Carg(1)/function(unicode,separator) return format("%s0x%04X",separator,unicode) end)^0
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
function utf.toeight(str)
 if not str or str=="" then
  return nil
 end
 local utftype=lpegmatch(p_utfstricttype,str)
 if utftype=="utf-8" then
  return sub(str,4)      
 elseif utftype=="utf-16-be" then
  return utf16_to_utf8_be(str) 
 elseif utftype=="utf-16-le" then
  return utf16_to_utf8_le(str) 
 else
  return str
 end
end
do
 local p_nany=p_utf8character/""
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
if not string.utfvalues then
 local find=string.find
 local dummy=function()
 end
 function string.utfvalues(str)
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
end
utf.values=string.utfvalues
function utf.chrlen(u) 
 return
  (u<0x80 and 1) or
  (u<0xE0 and 2) or
  (u<0xF0 and 3) or
  (u<0xF8 and 4) or
  (u<0xFC and 5) or
  (u<0xFE and 6) or 0
end
if bit32 then
 local extract=bit32.extract
 local char=string.char
 function utf.toutf32string(n)
  if n<=0xFF then
   return
    char(n).."\000\000\000"
  elseif n<=0xFFFF then
   return
    char(extract(n,0,8))..char(extract(n,8,8)).."\000\000"
  elseif n<=0xFFFFFF then
   return
    char(extract(n,0,8))..char(extract(n,8,8))..char(extract(n,16,8)).."\000"
  else
   return
    char(extract(n,0,8))..char(extract(n,8,8))..char(extract(n,16,8))..char(extract(n,24,8))
  end
 end
end
local len=utf.len
local rep=rep
function string.utfpadd(s,n)
 if n and n~=0 then
  local l=len(s)
  if n>0 then
   local d=n-l
   if d>0 then
    return rep(c or " ",d)..s
   end
  else
   local d=- n-l
   if d>0 then
    return s..rep(c or " ",d)
   end
  end
 end
 return s
end
do
 local utfcharacters=utf.characters or string.utfcharacters
 local utfchar=utf.char    or string.utfcharacter
 lpeg.UP=P
 if utfcharacters then
  function lpeg.US(str)
   local p=P(false)
   for uc in utfcharacters(str) do
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
   lpegmatch((p_utf8char/f)^0,str)
   return p
  end
 end
 local range=p_utf8byte*p_utf8byte+Cc(false) 
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
  end
  if not utfchar then
   utfchar=utf.char 
  end
  if utfchar and (last-first<8) then 
   local p=P(false)
   for i=first,last do
    p=p+P(utfchar(i))
   end
   return p 
  else
   local f=function(b)
    return b>=first and b<=last
   end
   return p_utf8byte/f 
  end
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["l-math"] = package.loaded["l-math"] or true

-- original size: 2555, stripped down to: 1831

if not modules then modules={} end modules ['l-math']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
if not math.ceiling then
 math.ceiling=math.ceil
end
if not math.round then
 local floor=math.floor
 function math.round(x) return floor(x+0.5) end
end
if not math.div then
 local floor=math.floor
 function math.div(n,m) return floor(n/m) end
end
if not math.mod then
 function math.mod(n,m) return n%m end
end
if not math.sind then
 local sin,cos,tan=math.sin,math.cos,math.tan
 local pipi=2*math.pi/360
 function math.sind(d) return sin(d*pipi) end
 function math.cosd(d) return cos(d*pipi) end
 function math.tand(d) return tan(d*pipi) end
end
if not math.odd then
 function math.odd (n) return n%2~=0 end
 function math.even(n) return n%2==0 end
end
if not math.cosh then
 local exp=math.exp
 function math.cosh(x)
  local xx=exp(x)
  return (xx+1/xx)/2
 end
 function math.sinh(x)
  local xx=exp(x)
  return (xx-1/xx)/2
 end
 function math.tanh(x)
  local xx=exp(x)
  return (xx-1/xx)/(xx+1/xx)
 end
end
if not math.pow then
 function math.pow(x,y)
  return x^y
 end
end
if not math.atan2 then
 math.atan2=math.atan
end
if not math.ldexp then
 function math.ldexp(x,e)
  return x*2.0^e
 end
end
if not math.log10 then
 local log=math.log
 function math.log10(x)
  return log(x,10)
 end
end
if not math.type then
 function math.type()
  return "float"
 end
end
if not math.tointeger then
 math.mininteger=-0x4FFFFFFFFFFF
 math.maxinteger=0x4FFFFFFFFFFF
 local floor=math.floor
 function math.tointeger(n)
  local f=floor(n)
  return f==n and f or nil
 end
end
if not math.ult then
 local floor=math.floor
 function math.tointeger(m,n)
  return floor(m)<floor(n) 
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-str"] = package.loaded["util-str"] or true

-- original size: 45153, stripped down to: 22734

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
local format,gsub,rep,sub,find=string.format,string.gsub,string.rep,string.sub,string.find
local load,dump=load,string.dump
local tonumber,type,tostring,next,setmetatable=tonumber,type,tostring,next,setmetatable
local unpack,concat=table.unpack,table.concat
local P,V,C,S,R,Ct,Cs,Cp,Carg,Cc=lpeg.P,lpeg.V,lpeg.C,lpeg.S,lpeg.R,lpeg.Ct,lpeg.Cs,lpeg.Cp,lpeg.Carg,lpeg.Cc
local patterns,lpegmatch=lpeg.patterns,lpeg.match
local utfchar,utfbyte,utflen=utf.char,utf.byte,utf.len
local loadstripped=function(str,shortcuts)
 if shortcuts then
  return load(dump(load(str),true),nil,nil,shortcuts)
 else
  return load(dump(load(str),true))
 end
end
if not number then number={} end 
local stripzero=patterns.stripzero
local stripzeros=patterns.stripzeros
local newline=patterns.newline
local endofstring=patterns.endofstring
local anything=patterns.anything
local whitespace=patterns.whitespace
local space=patterns.space
local spacer=patterns.spacer
local spaceortab=patterns.spaceortab
local digit=patterns.digit
local sign=patterns.sign
local period=patterns.period
local ptf=1/65536
local bpf=(7200/7227)/65536
local function points(n)
 if n==0 then
  return "0pt"
 end
 n=tonumber(n)
 if not n or n==0 then
  return "0pt"
 end
 n=n*ptf
 if n%1==0 then
  return format("%ipt",n)
 end
 return lpegmatch(stripzeros,format("%.5fpt",n)) 
end
local function basepoints(n)
 if n==0 then
  return "0pt"
 end
 n=tonumber(n)
 if not n or n==0 then
  return "0pt"
 end
 n=n*bpf
 if n%1==0 then
  return format("%ibp",n)
 end
 return lpegmatch(stripzeros,format("%.5fbp",n)) 
end
number.points=points
number.basepoints=basepoints
local rubish=spaceortab^0*newline
local anyrubish=spaceortab+newline
local stripped=(spaceortab^1/"")*newline
local leading=rubish^0/""
local trailing=(anyrubish^1*endofstring)/""
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
   end+newline*Cp()/function(position)
    extra,start=0,position
   end+anything
  )^1)
function strings.tabtospace(str,tab)
 return lpegmatch(pattern,str,1,tab or 7)
end
function string.utfpadding(s,n)
 if not n or n==0 then
  return ""
 end
 local l=utflen(s)
 if n>0 then
  return nspaces[n-l]
 else
  return nspaces[-n-l]
 end
end
local optionalspace=spacer^0
local nospace=optionalspace/""
local endofline=nospace*newline
local stripend=(whitespace^1*endofstring)/""
local normalline=(nospace*((1-optionalspace*(newline+endofstring))^1)*nospace)
local stripempty=endofline^1/""
local normalempty=endofline^1
local singleempty=endofline*(endofline^0/"")
local doubleempty=endofline*endofline^-1*(endofline^0/"")
local stripstart=stripempty^0
local intospace=whitespace^1/" "
local noleading=whitespace^1/""
local notrailing=noleading*endofstring
local p_prune_normal=Cs (stripstart*(stripend+normalline+normalempty )^0 )
local p_prune_collapse=Cs (stripstart*(stripend+normalline+doubleempty )^0 )
local p_prune_noempty=Cs (stripstart*(stripend+normalline+singleempty )^0 )
local p_prune_intospace=Cs (noleading*(notrailing+intospace+1     )^0 )
local p_retain_normal=Cs ((normalline+normalempty )^0 )
local p_retain_collapse=Cs ((normalline+doubleempty )^0 )
local p_retain_noempty=Cs ((normalline+singleempty )^0 )
local striplinepatterns={
 ["prune"]=p_prune_normal,
 ["prune and collapse"]=p_prune_collapse,
 ["prune and no empty"]=p_prune_noempty,
 ["prune and to space"]=p_prune_intospace,
 ["retain"]=p_retain_normal,
 ["retain and collapse"]=p_retain_collapse,
 ["retain and no empty"]=p_retain_noempty,
 ["collapse"]=patterns.collapser,
}
setmetatable(striplinepatterns,{ __index=function(t,k) return p_prune_collapse end })
strings.striplinepatterns=striplinepatterns
function strings.striplines(str,how)
 return str and lpegmatch(striplinepatterns[how],str) or str
end
function strings.collapse(str) 
 return str and lpegmatch(p_prune_intospace,str) or str
end
strings.striplong=strings.striplines
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
local tracedchars={ [0]=
 "[null]","[soh]","[stx]","[etx]","[eot]","[enq]","[ack]","[bel]",
 "[bs]","[ht]","[lf]","[vt]","[ff]","[cr]","[so]","[si]",
 "[dle]","[dc1]","[dc2]","[dc3]","[dc4]","[nak]","[syn]","[etb]",
 "[can]","[em]","[sub]","[esc]","[fs]","[gs]","[rs]","[us]",
 "[space]",
}
string.tracedchars=tracedchars
strings.tracers=tracedchars
function string.tracedchar(b)
 if type(b)=="number" then
  return tracedchars[b] or (utfchar(b).." (U+"..format("%05X",b)..")")
 else
  local c=utfbyte(b)
  return tracedchars[c] or (b.." (U+"..(c and format("%05X",c) or "?????")..")")
 end
end
function number.signed(i)
 if i>0 then
  return "+",i
 else
  return "-",-i
 end
end
local two=digit*digit
local three=two*digit
local prefix=(Carg(1)*three)^1
local splitter=Cs (
 (((1-(three^1*period))^1+C(three))*prefix+C((1-period)^1))*(anything/""*Carg(2))*C(2)
)
local splitter3=Cs (
 three*prefix*endofstring+two*prefix*endofstring+digit*prefix*endofstring+three+two+digit
)
patterns.formattednumber=splitter
function number.formatted(n,sep1,sep2)
 if sep1==false then
  if type(n)=="number" then
   n=tostring(n)
  end
  return lpegmatch(splitter3,n,1,sep2 or ".")
 else
  if type(n)=="number" then
   n=format("%0.2f",n)
  end
  if sep1==true then
   return lpegmatch(splitter,n,1,".",",")
  elseif sep1=="." then
   return lpegmatch(splitter,n,1,sep1,sep2 or ",")
  elseif sep1=="," then
   return lpegmatch(splitter,n,1,sep1,sep2 or ".")
  else
   return lpegmatch(splitter,n,1,sep1 or ",",sep2 or ".")
  end
 end
end
local p=Cs(
  P("-")^0*(P("0")^1/"")^0*(1-period)^0*(period*P("0")^1*endofstring/""+period^0)*P(1-P("0")^1*endofstring)^0
 )
function number.compactfloat(n,fmt)
 if n==0 then
  return "0"
 elseif n==1 then
  return "1"
 end
 n=lpegmatch(p,format(fmt or "%0.3f",n))
 if n=="." or n=="" or n=="-" then
  return "0"
 end
 return n
end
local zero=P("0")^1/""
local plus=P("+")/""
local minus=P("-")
local separator=period
local trailing=zero^1*#S("eE")
local exponent=(S("eE")*(plus+Cs((minus*zero^0*endofstring)/"")+minus)*zero^0*(endofstring*Cc("0")+anything^1))
local pattern_a=Cs(minus^0*digit^1*(separator/""*trailing+separator*(trailing+digit)^0)*exponent)
local pattern_b=Cs((exponent+anything)^0)
function number.sparseexponent(f,n)
 if not n then
  n=f
  f="%e"
 end
 local tn=type(n)
 if tn=="string" then 
  local m=tonumber(n)
  if m then
   return lpegmatch((f=="%e" or f=="%E") and pattern_a or pattern_b,format(f,m))
  end
 elseif tn=="number" then
  return lpegmatch((f=="%e" or f=="%E") and pattern_a or pattern_b,format(f,n))
 end
 return tostring(n)
end
local hf={}
local hs={}
setmetatable(hf,{ __index=function(t,k)
 local v="%."..k.."f"
 t[k]=v
 return v
end } )
setmetatable(hs,{ __index=function(t,k)
 local v="%"..k.."s"
 t[k]=v
 return v
end } )
function number.formattedfloat(n,b,a)
 local s=format(hf[a],n)
 local l=(b or 0)+(a or 0)+1
 if #s<l then
  return format(hs[l],s)
 else
  return s
 end
end
local template=[[
%s
%s
return function(%s) return %s end
]]
local pattern=Cs(Cc('"')*(
 (1-S('"\\\n\r'))^1+P('"')/'\\"'+P('\\')/'\\\\'+P('\n')/'\\n'+P('\r')/'\\r'
)^0*Cc('"'))
patterns.escapedquotes=pattern
function string.escapedquotes(s)
 return lpegmatch(pattern,s)
end
local preamble=""
local environment={
 global=global or _G,
 lpeg=lpeg,
 type=type,
 tostring=tostring,
 tonumber=tonumber,
 format=string.format,
 concat=table.concat,
 signed=number.signed,
 points=number.points,
 basepoints=number.basepoints,
 utfchar=utf.char,
 utfbyte=utf.byte,
 lpegmatch=lpeg.match,
 nspaces=string.nspaces,
 utfpadding=string.utfpadding,
 tracedchar=string.tracedchar,
 autosingle=string.autosingle,
 autodouble=string.autodouble,
 sequenced=table.sequenced,
 formattednumber=number.formatted,
 sparseexponent=number.sparseexponent,
 formattedfloat=number.formattedfloat,
 stripzero=patterns.stripzero,
 stripzeros=patterns.stripzeros,
 escapedquotes=string.escapedquotes,
 FORMAT=string.f6,
}
local arguments={ "a1" } 
setmetatable(arguments,{ __index=function(t,k)
  local v=t[k-1]..",a"..k
  t[k]=v
  return v
 end
})
local prefix_any=C((sign+space+period+digit)^0)
local prefix_sub=(C((sign+digit)^0)+Cc(0))*period*(C((sign+digit)^0)+Cc(0))
local prefix_tab=P("{")*C((1-P("}"))^0)*P("}")+C((1-R("az","AZ","09","%%"))^0)
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
local format_right=function(f)
 n=n+1
 f=tonumber(f)
 if not f or f==0 then
  return format("(a%s or '')",n)
 elseif f>0 then
  return format("utfpadding(a%s,%i)..a%s",n,f,n)
 else
  return format("a%s..utfpadding(a%s,%i)",n,n,f)
 end
end
local format_left=function(f)
 n=n+1
 f=tonumber(f)
 if not f or f==0 then
  return format("(a%s or '')",n)
 end
 if f<0 then
  return format("utfpadding(a%s,%i)..a%s",n,-f,n)
 else
  return format("a%s..utfpadding(a%s,%i)",n,n,-f)
 end
end
local format_q=JITSUPPORTED and function()
 n=n+1
 return format("(a%s ~= nil and format('%%q',tostring(a%s)) or '')",n,n)
end or function()
 n=n+1
 return format("(a%s ~= nil and format('%%q',a%s) or '')",n,n)
end
local format_Q=function() 
 n=n+1
 return format("escapedquotes(tostring(a%s))",n)
end
local format_i=function(f)
 n=n+1
 if f and f~="" then
  return format("format('%%%si',a%s)",f,n)
 else
  return format("format('%%i',a%s)",n) 
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
local format_F=function(f) 
 n=n+1
 if not f or f=="" then
  return format("(((a%s > -0.0000000005 and a%s < 0.0000000005) and '0') or format((a%s %% 1 == 0) and '%%i' or '%%.9f',a%s))",n,n,n,n)
 else
  return format("format((a%s %% 1 == 0) and '%%i' or '%%%sf',a%s)",n,f,n)
 end
end
local format_k=function(b,a) 
 n=n+1
 return format("formattedfloat(a%s,%s,%s)",n,b or 0,a or 0)
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
local format_j=function(f)
 n=n+1
 return format("sparseexponent('%%%se',a%s)",f,n)
end
local format_J=function(f)
 n=n+1
 return format("sparseexponent('%%%sE',a%s)",f,n)
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
local format_n=function() 
 n=n+1
 return format("((a%s %% 1 == 0) and format('%%i',a%s) or tostring(a%s))",n,n,n)
end
local format_N  if environment.FORMAT then
 format_N=function(f)
  n=n+1
  if not f or f=="" then
   return format("FORMAT(a%s,'%%.9f')",n)
  elseif f==".6" or f=="0.6" then
   return format("FORMAT(a%s)",n)
  else
   return format("FORMAT(a%s,'%%%sf')",n,f)
  end
 end
else
 format_N=function(f) 
  n=n+1
  if not f or f=="" then
   f=".9"
  end 
  return format("(((a%s %% 1 == 0) and format('%%i',a%s)) or lpegmatch(stripzero,format('%%%sf',a%s)))",n,n,f,n)
 end
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
local format_m=function(f)
 n=n+1
 if not f or f=="" then
  f=","
 end
 if f=="0" then
  return format([[formattednumber(a%s,false)]],n)
 else
  return format([[formattednumber(a%s,%q,".")]],n,f)
 end
end
local format_M=function(f)
 n=n+1
 if not f or f=="" then
  f="."
 end
 if f=="0" then
  return format([[formattednumber(a%s,false)]],n)
 else
  return format([[formattednumber(a%s,%q,",")]],n,f)
 end
end
local format_z=function(f)
 n=n+(tonumber(f) or 1)
 return "''" 
end
local format_rest=function(s)
 return format("%q",s) 
end
local format_extension=function(extensions,f,name)
 local extension=extensions[name] or "tostring(%s)"
 local f=tonumber(f) or 1
 local w=find(extension,"%.%.%.")
 if w then
  if f==0 then
   extension=gsub(extension,"%.%.%.","")
   return extension
  elseif f==1 then
   extension=gsub(extension,"%.%.%.","%%s")
   n=n+1
   local a="a"..n
   return format(extension,a,a) 
  elseif f<0 then
   local a="a"..(n+f+1)
   return format(extension,a,a)
  else
   extension=gsub(extension,"%.%.%.",rep("%%s,",f-1).."%%s")
   local t={}
   for i=1,f do
    n=n+1
    t[i]="a"..n
   end
   return format(extension,unpack(t))
  end
 else
  extension=gsub(extension,"%%s",function()
   n=n+1
   return "a"..n
  end)
  return extension
 end
end
local builder=Cs { "start",
 start=(
  (
   P("%")/""*(
    V("!") 
+V("s")+V("q")+V("i")+V("d")+V("f")+V("F")+V("g")+V("G")+V("e")+V("E")+V("x")+V("X")+V("o")
+V("c")+V("C")+V("S") 
+V("Q") 
+V("n") 
+V("N") 
+V("k")
+V("r")+V("h")+V("H")+V("u")+V("U")+V("p")+V("b")+V("t")+V("T")+V("l")+V("L")+V("I")+V("w") 
+V("W") 
+V("a") 
+V("A") 
+V("j")+V("J") 
+V("m")+V("M") 
+V("z")
+V(">") 
+V("<")
   )+V("*")
  )*(endofstring+Carg(1))
 )^0,
 ["s"]=(prefix_any*P("s"))/format_s,
 ["q"]=(prefix_any*P("q"))/format_q,
 ["i"]=(prefix_any*P("i"))/format_i,
 ["d"]=(prefix_any*P("d"))/format_d,
 ["f"]=(prefix_any*P("f"))/format_f,
 ["F"]=(prefix_any*P("F"))/format_F,
 ["g"]=(prefix_any*P("g"))/format_g,
 ["G"]=(prefix_any*P("G"))/format_G,
 ["e"]=(prefix_any*P("e"))/format_e,
 ["E"]=(prefix_any*P("E"))/format_E,
 ["x"]=(prefix_any*P("x"))/format_x,
 ["X"]=(prefix_any*P("X"))/format_X,
 ["o"]=(prefix_any*P("o"))/format_o,
 ["S"]=(prefix_any*P("S"))/format_S,
 ["Q"]=(prefix_any*P("Q"))/format_Q,
 ["n"]=(prefix_any*P("n"))/format_n,
 ["N"]=(prefix_any*P("N"))/format_N,
 ["k"]=(prefix_sub*P("k"))/format_k,
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
 ["l"]=(prefix_any*P("l"))/format_l,
 ["L"]=(prefix_any*P("L"))/format_L,
 ["I"]=(prefix_any*P("I"))/format_I,
 ["w"]=(prefix_any*P("w"))/format_w,
 ["W"]=(prefix_any*P("W"))/format_W,
 ["j"]=(prefix_any*P("j"))/format_j,
 ["J"]=(prefix_any*P("J"))/format_J,
 ["m"]=(prefix_any*P("m"))/format_m,
 ["M"]=(prefix_any*P("M"))/format_M,
 ["z"]=(prefix_any*P("z"))/format_z,
 ["a"]=(prefix_any*P("a"))/format_a,
 ["A"]=(prefix_any*P("A"))/format_A,
 ["<"]=(prefix_any*P("<"))/format_left,
 [">"]=(prefix_any*P(">"))/format_right,
 ["*"]=Cs(((1-P("%"))^1+P("%%")/"%%")^1)/format_rest,
 ["?"]=Cs(((1-P("%"))^1      )^1)/format_rest,
 ["!"]=Carg(2)*prefix_any*P("!")*C((1-P("!"))^1)*P("!")/format_extension,
}
local xx=setmetatable({},{ __index=function(t,k) local v=format("%02x",k) t[k]=v return v end })
local XX=setmetatable({},{ __index=function(t,k) local v=format("%02X",k) t[k]=v return v end })
local preset={
 ["%02x"]=function(n) return xx[n] end,
 ["%02X"]=function(n) return XX[n] end,
}
local direct=P("%")*(sign+space+period+digit)^0*S("sqidfgGeExXo")*endofstring/[[local format = string.format return function(str) return format("%0",str) end]]
local function make(t,str)
 local f=preset[str]
 if f then
  return f
 end
 local p=lpegmatch(direct,str)
 if p then
  f=loadstripped(p)()
 else
  n=0
  p=lpegmatch(builder,str,1,t._connector_,t._extensions_) 
  if n>0 then
   p=format(template,preamble,t._preamble_,arguments[n],p)
   f=loadstripped(p,t._environment_)() 
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
function strings.formatters.new(noconcat)
 local e={} 
 for k,v in next,environment do
  e[k]=v
 end
 local t={
  _type_="formatter",
  _connector_=noconcat and "," or "..",
  _extensions_={},
  _preamble_="",
  _environment_=e,
 }
 setmetatable(t,{ __index=make,__call=use })
 return t
end
local formatters=strings.formatters.new() 
string.formatters=formatters 
string.formatter=function(str,...) return formatters[str](...) end 
local function add(t,name,template,preamble)
 if type(t)=="table" and t._type_=="formatter" then
  t._extensions_[name]=template or "%s"
  if type(preamble)=="string" then
   t._preamble_=preamble.."\n"..t._preamble_ 
  elseif type(preamble)=="table" then
   for k,v in next,preamble do
    t._environment_[k]=v
   end
  end
 end
end
strings.formatters.add=add
patterns.xmlescape=Cs((P("<")/"&lt;"+P(">")/"&gt;"+P("&")/"&amp;"+P('"')/"&quot;"+anything)^0)
patterns.texescape=Cs((C(S("#$%\\{}"))/"\\%1"+anything)^0)
patterns.luaescape=Cs(((1-S('"\n'))^1+P('"')/'\\"'+P('\n')/'\\n"')^0) 
patterns.luaquoted=Cs(Cc('"')*((1-S('"\n'))^1+P('"')/'\\"'+P('\n')/'\\n"')^0*Cc('"'))
add(formatters,"xml",[[lpegmatch(xmlescape,%s)]],{ xmlescape=patterns.xmlescape })
add(formatters,"tex",[[lpegmatch(texescape,%s)]],{ texescape=patterns.texescape })
add(formatters,"lua",[[lpegmatch(luaescape,%s)]],{ luaescape=patterns.luaescape })
local dquote=patterns.dquote 
local equote=patterns.escaped+dquote/'\\"'+1
local cquote=Cc('"')
local pattern=Cs(dquote*(equote-P(-2))^0*dquote)     
+Cs(cquote*(equote-space)^0*space*equote^0*cquote) 
function string.optionalquoted(str)
 return lpegmatch(pattern,str) or str
end
local pattern=Cs((newline/(os.newline or "\r")+1)^0)
function string.replacenewlines(str)
 return lpegmatch(pattern,str)
end
function strings.newcollector()
 local result,r={},0
 return
  function(fmt,str,...) 
   r=r+1
   result[r]=str==nil and fmt or formatters[fmt](str,...)
  end,
  function(connector) 
   if result then
    local str=concat(result,connector)
    result,r={},0
    return str
   end
  end
end
local f_16_16=formatters["%0.5N"]
function number.to16dot16(n)
 return f_16_16(n/65536.0)
end
if not string.explode then
 local tsplitat=lpeg.tsplitat
 local p_utf=patterns.utf8character
 local p_check=C(p_utf)*(P("+")*Cc(true))^0
 local p_split=Ct(C(p_utf)^0)
 local p_space=Ct((C(1-P(" ")^1)+P(" ")^1)^0)
 function string.explode(str,symbol)
  if symbol=="" then
   return lpegmatch(p_split,str)
  elseif symbol then
   local a,b=lpegmatch(p_check,symbol)
   if b then
    return lpegmatch(tsplitat(P(a)^1),str)
   else
    return lpegmatch(tsplitat(a),str)
   end
  else
   return lpegmatch(p_space,str)
  end
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-tab"] = package.loaded["util-tab"] or true

-- original size: 32649, stripped down to: 18257

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
local format,gmatch,gsub,sub=string.format,string.gmatch,string.gsub,string.sub
local concat,insert,remove,sort=table.concat,table.insert,table.remove,table.sort
local setmetatable,getmetatable,tonumber,tostring,rawget=setmetatable,getmetatable,tonumber,tostring,rawget
local type,next,rawset,tonumber,tostring,load,select=type,next,rawset,tonumber,tostring,load,select
local lpegmatch,P,Cs,Cc=lpeg.match,lpeg.P,lpeg.Cs,lpeg.Cc
local sortedkeys,sortedpairs=table.sortedkeys,table.sortedpairs
local formatters=string.formatters
local utftoeight=utf.toeight
local splitter=lpeg.tsplitat(".")
function utilities.tables.definetable(target,nofirst,nolast) 
 local composed=nil
 local t={}
 local snippets=lpegmatch(splitter,target)
 for i=1,#snippets-(nolast and 1 or 0) do
  local name=snippets[i]
  if composed then
   composed=composed.."."..name
    t[#t+1]=formatters["if not %s then %s = { } end"](composed,composed)
  else
   composed=name
   if not nofirst then
    t[#t+1]=formatters["%s = %s or { }"](composed,composed)
   end
  end
 end
 if composed then
  if nolast then
   composed=composed.."."..snippets[#snippets]
  end
  return concat(t,"\n"),composed 
 else
  return "",target
 end
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
 local names=lpegmatch(splitter,target)
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
function tables.replacevalue(t,oldvalue,newvalue)
 if oldvalue and newvalue then
  for i=1,#t do
   if t[i]==oldvalue then
    t[i]=newvalue
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
  local noffields=#fields
  if specification.preamble==true then
   for f=1,noffields do
    r[f]=lpegmatch(escape,tostring(fields[f]))
   end
   result[1]=concat(r,separator)
  end
  for i=1,#t do
   local ti=t[i]
   for f=1,noffields do
    local field=ti[fields[f]]
    if type(field)=="string" then
     r[f]=lpegmatch(escape,field)
    else
     r[f]=tostring(field)
    end
   end
   result[i+1]=concat(r,separator)
  end
  return concat(result,"\n")
 else
  return ""
 end
end
local nspaces=utilities.strings.newrepeater(" ")
local function toxml(t,d,result,step)
 local r=#result
 for k,v in sortedpairs(t) do
  local s=nspaces[d] 
  local tk=type(k)
  local tv=type(v)
  if tv=="table" then
   if tk=="number" then
    r=r+1 result[r]=formatters["%s<entry n='%s'>"](s,k)
    toxml(v,d+step,result,step)
    r=r+1 result[r]=formatters["%s</entry>"](s,k)
   else
    r=r+1 result[r]=formatters["%s<%s>"](s,k)
    toxml(v,d+step,result,step)
    r=r+1 result[r]=formatters["%s</%s>"](s,k)
   end
  elseif tv=="string" then
   if tk=="number" then
    r=r+1 result[r]=formatters["%s<entry n='%s'>%!xml!</entry>"](s,k,v,k)
   else
    r=r+1 result[r]=formatters["%s<%s>%!xml!</%s>"](s,k,v,k)
   end
  elseif tk=="number" then
   r=r+1 result[r]=formatters["%s<entry n='%s'>%S</entry>"](s,k,v,k)
  else
   r=r+1 result[r]=formatters["%s<%s>%S</%s>"](s,k,v,k)
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
if JITSUPPORTED then
 local f_hashed_string=formatters["[%Q]=%Q,"]
 local f_hashed_number=formatters["[%Q]=%s,"]
 local f_hashed_boolean=formatters["[%Q]=%l,"]
 local f_hashed_table=formatters["[%Q]="]
 local f_indexed_string=formatters["[%s]=%Q,"]
 local f_indexed_number=formatters["[%s]=%s,"]
 local f_indexed_boolean=formatters["[%s]=%l,"]
 local f_indexed_table=formatters["[%s]="]
 local f_ordered_string=formatters["%Q,"]
 local f_ordered_number=formatters["%s,"]
 local f_ordered_boolean=formatters["%l,"]
 function table.fastserialize(t,prefix)
  local r={ type(prefix)=="string" and prefix or "return" }
  local m=1
  local function fastserialize(t,outer) 
   local n=#t
   m=m+1
   r[m]="{"
   if n>0 then
    local v=t[0]
    if v then
     local tv=type(v)
     if tv=="string" then
      m=m+1 r[m]=f_indexed_string(0,v)
     elseif tv=="number" then
      m=m+1 r[m]=f_indexed_number(0,v)
     elseif tv=="table" then
      m=m+1 r[m]=f_indexed_table(0)
      fastserialize(v)
      m=m+1 r[m]=f_indexed_table(0)
     elseif tv=="boolean" then
      m=m+1 r[m]=f_indexed_boolean(0,v)
     end
    end
    for i=1,n do
     local v=t[i]
     local tv=type(v)
     if tv=="string" then
      m=m+1 r[m]=f_ordered_string(v)
     elseif tv=="number" then
      m=m+1 r[m]=f_ordered_number(v)
     elseif tv=="table" then
      fastserialize(v)
     elseif tv=="boolean" then
      m=m+1 r[m]=f_ordered_boolean(v)
     end
    end
   end
   for k,v in next,t do
    local tk=type(k)
    if tk=="number" then
     if k>n or k<0 then
      local tv=type(v)
      if tv=="string" then
       m=m+1 r[m]=f_indexed_string(k,v)
      elseif tv=="number" then
       m=m+1 r[m]=f_indexed_number(k,v)
      elseif tv=="table" then
       m=m+1 r[m]=f_indexed_table(k)
       fastserialize(v)
      elseif tv=="boolean" then
       m=m+1 r[m]=f_indexed_boolean(k,v)
      end
     end
    else
     local tv=type(v)
     if tv=="string" then
      m=m+1 r[m]=f_hashed_string(k,v)
     elseif tv=="number" then
      m=m+1 r[m]=f_hashed_number(k,v)
     elseif tv=="table" then
      m=m+1 r[m]=f_hashed_table(k)
      fastserialize(v)
     elseif tv=="boolean" then
      m=m+1 r[m]=f_hashed_boolean(k,v)
     end
    end
   end
   m=m+1
   if outer then
    r[m]="}"
   else
    r[m]="},"
   end
   return r
  end
  return concat(fastserialize(t,true))
 end
else
 local f_v=formatters["[%q]=%q,"]
 local f_t=formatters["[%q]="]
 local f_q=formatters["%q,"]
 function table.fastserialize(t,prefix) 
  local r={ type(prefix)=="string" and prefix or "return" }
  local m=1
  local function fastserialize(t,outer) 
   local n=#t
   m=m+1
   r[m]="{"
   if n>0 then
    local v=t[0]
    if v then
     m=m+1
     r[m]="[0]='"
     if type(v)=="table" then
      fastserialize(v)
     else
      r[m]=format("%q,",v)
     end
    end
    for i=1,n do
     local v=t[i]
     m=m+1
     if type(v)=="table" then
      r[m]=format("[%i]=",i)
      fastserialize(v)
     else
      r[m]=format("[%i]=%q,",i,v)
     end
    end
   end
   for k,v in next,t do
    local tk=type(k)
    if tk=="number" then
     if k>n or k<0 then
      m=m+1
      if type(v)=="table" then
       r[m]=format("[%i]=",k)
       fastserialize(v)
      else
       r[m]=format("[%i]=%q,",k,v)
      end
     end
    else
     m=m+1
     if type(v)=="table" then
      r[m]=format("[%q]=",k)
      fastserialize(v)
     else
      r[m]=format("[%q]=%q,",k,v)
     end
    end
   end
   m=m+1
   if outer then
    r[m]="}"
   else
    r[m]="},"
   end
   return r
  end
  return concat(fastserialize(t,true))
 end
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
function table.load(filename,loader)
 if filename then
  local t=(loader or io.loaddata)(filename)
  if t and t~="" then
   local t=utftoeight(t)
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
 io.savedata(filename,table.serialize(t,n==nil and true or n,...)) 
end
local f_key_value=formatters["%s=%q"]
local f_add_table=formatters[" {%t},\n"]
local f_return_table=formatters["return {\n%t}"]
local function slowdrop(t) 
 local r={}
 local l={}
 for i=1,#t do
  local ti=t[i]
  local j=0
  for k,v in next,ti do
   j=j+1
   l[j]=f_key_value(k,v)
  end
  r[i]=f_add_table(l)
 end
 return f_return_table(r)
end
local function fastdrop(t)
 local r={ "return {\n" }
 local m=1
 for i=1,#t do
  local ti=t[i]
  m=m+1 r[m]=" {"
  for k,v in next,ti do
   m=m+1 r[m]=f_key_value(k,v)
  end
  m=m+1 r[m]="},\n"
 end
 m=m+1
 r[m]="}"
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
local selfmapper={ __index=function(t,k) t[k]=k return k end }
function table.twowaymapper(t) 
 if not t then     
  t={}       
 else        
  local zero=rawget(t,0)  
  for i=zero and 0 or 1,#t do
   local ti=t[i]    
   if ti then
    local i=tostring(i)
    t[i]=ti   
    t[ti]=i    
   end
  end
 end
 setmetatable(t,selfmapper)
 return t
end
local f_start_key_idx=formatters["%w{"]
local f_start_key_num=JITSUPPORTED and formatters["%w[%s]={"] or formatters["%w[%q]={"]
local f_start_key_str=formatters["%w[%q]={"]
local f_start_key_boo=formatters["%w[%l]={"]
local f_start_key_nop=formatters["%w{"]
local f_stop=formatters["%w},"]
local f_key_num_value_num=JITSUPPORTED and formatters["%w[%s]=%s,"] or formatters["%w[%s]=%q,"]
local f_key_str_value_num=JITSUPPORTED and formatters["%w[%Q]=%s,"] or formatters["%w[%Q]=%q,"]
local f_key_boo_value_num=JITSUPPORTED and formatters["%w[%l]=%s,"] or formatters["%w[%l]=%q,"]
local f_key_num_value_str=JITSUPPORTED and formatters["%w[%s]=%Q,"] or formatters["%w[%q]=%Q,"]
local f_key_str_value_str=formatters["%w[%Q]=%Q,"]
local f_key_boo_value_str=formatters["%w[%l]=%Q,"]
local f_key_num_value_boo=JITSUPPORTED and formatters["%w[%s]=%l,"] or formatters["%w[%q]=%l,"]
local f_key_str_value_boo=formatters["%w[%Q]=%l,"]
local f_key_boo_value_boo=formatters["%w[%l]=%l,"]
local f_key_num_value_not=JITSUPPORTED and formatters["%w[%s]={},"] or formatters["%w[%q]={},"]
local f_key_str_value_not=formatters["%w[%Q]={},"]
local f_key_boo_value_not=formatters["%w[%l]={},"]
local f_key_num_value_seq=JITSUPPORTED and formatters["%w[%s]={ %, t },"] or formatters["%w[%q]={ %, t },"]
local f_key_str_value_seq=formatters["%w[%Q]={ %, t },"]
local f_key_boo_value_seq=formatters["%w[%l]={ %, t },"]
local f_val_num=JITSUPPORTED and formatters["%w%s,"] or formatters["%w%q,"]
local f_val_str=formatters["%w%Q,"]
local f_val_boo=formatters["%w%l,"]
local f_val_not=formatters["%w{},"]
local f_val_seq=formatters["%w{ %, t },"]
local f_fin_seq=formatters[" %, t }"]
local f_table_return=formatters["return {"]
local f_table_name=formatters["%s={"]
local f_table_direct=formatters["{"]
local f_table_entry=formatters["[%Q]={"]
local f_table_finish=formatters["}"]
local spaces=utilities.strings.newrepeater(" ")
local original_serialize=table.serialize
local is_simple_table=table.is_simple_table
local function serialize(root,name,specification)
 if type(specification)=="table" then
  return original_serialize(root,name,specification) 
 end
 local t 
 local n=1
 local unknown=false
 local function do_serialize(root,name,depth,level,indexed)
  if level>0 then
   n=n+1
   if indexed then
    t[n]=f_start_key_idx(depth)
   else
    local tn=type(name)
    if tn=="number" then
     t[n]=f_start_key_num(depth,name)
    elseif tn=="string" then
     t[n]=f_start_key_str(depth,name)
    elseif tn=="boolean" then
     t[n]=f_start_key_boo(depth,name)
    else
     t[n]=f_start_key_nop(depth)
    end
   end
   depth=depth+1
  end
  if root and next(root)~=nil then
   local first=nil
   local last=#root
   if last>0 then
    for k=1,last do
     if rawget(root,k)==nil then
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
    local tv=type(v)
    local tk=type(k)
    if first and tk=="number" and k<=last and k>=first then
     if tv=="number" then
      n=n+1 t[n]=f_val_num(depth,v)
     elseif tv=="string" then
      n=n+1 t[n]=f_val_str(depth,v)
     elseif tv=="table" then
      if next(v)==nil then 
       n=n+1 t[n]=f_val_not(depth)
      else
       local st=is_simple_table(v)
       if st then
        n=n+1 t[n]=f_val_seq(depth,st)
       else
        do_serialize(v,k,depth,level+1,true)
       end
      end
     elseif tv=="boolean" then
      n=n+1 t[n]=f_val_boo(depth,v)
     elseif unknown then
      n=n+1 t[n]=f_val_str(depth,tostring(v))
     end
    elseif tv=="number" then
     if tk=="number" then
      n=n+1 t[n]=f_key_num_value_num(depth,k,v)
     elseif tk=="string" then
      n=n+1 t[n]=f_key_str_value_num(depth,k,v)
     elseif tk=="boolean" then
      n=n+1 t[n]=f_key_boo_value_num(depth,k,v)
     elseif unknown then
      n=n+1 t[n]=f_key_str_value_num(depth,tostring(k),v)
     end
    elseif tv=="string" then
     if tk=="number" then
      n=n+1 t[n]=f_key_num_value_str(depth,k,v)
     elseif tk=="string" then
      n=n+1 t[n]=f_key_str_value_str(depth,k,v)
     elseif tk=="boolean" then
      n=n+1 t[n]=f_key_boo_value_str(depth,k,v)
     elseif unknown then
      n=n+1 t[n]=f_key_str_value_str(depth,tostring(k),v)
     end
    elseif tv=="table" then
     if next(v)==nil then
      if tk=="number" then
       n=n+1 t[n]=f_key_num_value_not(depth,k)
      elseif tk=="string" then
       n=n+1 t[n]=f_key_str_value_not(depth,k)
      elseif tk=="boolean" then
       n=n+1 t[n]=f_key_boo_value_not(depth,k)
      elseif unknown then
       n=n+1 t[n]=f_key_str_value_not(depth,tostring(k))
      end
     else
      local st=is_simple_table(v)
      if not st then
       do_serialize(v,k,depth,level+1)
      elseif tk=="number" then
       n=n+1 t[n]=f_key_num_value_seq(depth,k,st)
      elseif tk=="string" then
       n=n+1 t[n]=f_key_str_value_seq(depth,k,st)
      elseif tk=="boolean" then
       n=n+1 t[n]=f_key_boo_value_seq(depth,k,st)
      elseif unknown then
       n=n+1 t[n]=f_key_str_value_seq(depth,tostring(k),st)
      end
     end
    elseif tv=="boolean" then
     if tk=="number" then
      n=n+1 t[n]=f_key_num_value_boo(depth,k,v)
     elseif tk=="string" then
      n=n+1 t[n]=f_key_str_value_boo(depth,k,v)
     elseif tk=="boolean" then
      n=n+1 t[n]=f_key_boo_value_boo(depth,k,v)
     elseif unknown then
      n=n+1 t[n]=f_key_str_value_boo(depth,tostring(k),v)
     end
    else
     if tk=="number" then
      n=n+1 t[n]=f_key_num_value_str(depth,k,tostring(v))
     elseif tk=="string" then
      n=n+1 t[n]=f_key_str_value_str(depth,k,tostring(v))
     elseif tk=="boolean" then
      n=n+1 t[n]=f_key_boo_value_str(depth,k,tostring(v))
     elseif unknown then
      n=n+1 t[n]=f_key_str_value_str(depth,tostring(k),tostring(v))
     end
    end
   end
  end
  if level>0 then
   n=n+1 t[n]=f_stop(depth-1)
  end
 end
 local tname=type(name)
 if tname=="string" then
  if name=="return" then
   t={ f_table_return() }
  else
   t={ f_table_name(name) }
  end
 elseif tname=="number" then
  t={ f_table_entry(name) }
 elseif tname=="boolean" then
  if name then
   t={ f_table_return() }
  else
   t={ f_table_direct() }
  end
 else
  t={ f_table_name("t") }
 end
 if root then
  if getmetatable(root) then 
   local dummy=root._w_h_a_t_e_v_e_r_ 
   root._w_h_a_t_e_v_e_r_=nil
  end
  if next(root)~=nil then
   local st=is_simple_table(root)
   if st then
    return t[1]..f_fin_seq(st) 
   else
    do_serialize(root,name,1,0)
   end
  end
 end
 n=n+1
 t[n]=f_table_finish()
 return concat(t,"\n")
end
table.serialize=serialize
if setinspector then
 setinspector("table",function(v)
  if type(v)=="table" then
   print(serialize(v,"table",{ metacheck=false }))
   return true
  end
 end)
end
local mt={
 __newindex=function(t,k,v)
  local n=t.last+1
  t.last=n
  t.list[n]=k
  t.hash[k]=v
 end,
 __index=function(t,k)
  return t.hash[k]
 end,
 __len=function(t)
  return t.last
 end,
}
function table.orderedhash()
 return setmetatable({ list={},hash={},last=0 },mt)
end
function table.ordered(t)
 local n=t.last
 if n>0 then
  local l=t.list
  local i=1
  local h=t.hash
  local f=function()
   if i<=n then
    local k=i
    local v=h[l[k]]
    i=i+1
    return k,v
   end
  end
  return f,1,h[l[1]]
 else
  return function() end
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-fil"] = package.loaded["util-fil"] or true

-- original size: 11530, stripped down to: 9007

if not modules then modules={} end modules ['util-fil']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local tonumber=tonumber
local byte=string.byte
local char=string.char
utilities=utilities or {}
local files={}
utilities.files=files
local zerobased={}
function files.open(filename,zb)
 local f=io.open(filename,"rb")
 if f then
  zerobased[f]=zb or false
 end
 return f
end
function files.close(f)
 zerobased[f]=nil
 f:close()
end
function files.size(f)
 local current=f:seek()
 local size=f:seek("end")
 f:seek("set",current)
 return size
end
files.getsize=files.size
function files.setposition(f,n)
 if zerobased[f] then
  f:seek("set",n)
 else
  f:seek("set",n-1)
 end
end
function files.getposition(f)
 if zerobased[f] then
  return f:seek()
 else
  return f:seek()+1
 end
end
function files.look(f,n,chars)
 local p=f:seek()
 local s=f:read(n)
 f:seek("set",p)
 if chars then
  return s
 else
  return byte(s,1,#s)
 end
end
function files.skip(f,n)
 if n==1 then
  f:read(n)
 else
  f:seek("set",f:seek()+n)
 end
end
function files.readbyte(f)
 return byte(f:read(1))
end
function files.readbytes(f,n)
 return byte(f:read(n),1,n)
end
function files.readbytetable(f,n)
 local s=f:read(n or 1)
 return { byte(s,1,#s) } 
end
function files.readchar(f)
 return f:read(1)
end
function files.readstring(f,n)
 return f:read(n or 1)
end
function files.readinteger1(f)  
 local n=byte(f:read(1))
 if n>=0x80 then
  return n-0x100
 else
  return n
 end
end
files.readcardinal1=files.readbyte  
files.readcardinal=files.readcardinal1
files.readinteger=files.readinteger1
files.readsignedbyte=files.readinteger1
function files.readcardinal2(f)
 local a,b=byte(f:read(2),1,2)
 return 0x100*a+b
end
function files.readcardinal2le(f)
 local b,a=byte(f:read(2),1,2)
 return 0x100*a+b
end
function files.readinteger2(f)
 local a,b=byte(f:read(2),1,2)
 if a>=0x80 then
  return 0x100*a+b-0x10000
 else
  return 0x100*a+b
 end
end
function files.readinteger2le(f)
 local b,a=byte(f:read(2),1,2)
 if a>=0x80 then
  return 0x100*a+b-0x10000
 else
  return 0x100*a+b
 end
end
function files.readcardinal3(f)
 local a,b,c=byte(f:read(3),1,3)
 return 0x10000*a+0x100*b+c
end
function files.readcardinal3le(f)
 local c,b,a=byte(f:read(3),1,3)
 return 0x10000*a+0x100*b+c
end
function files.readinteger3(f)
 local a,b,c=byte(f:read(3),1,3)
 if a>=0x80 then
  return 0x10000*a+0x100*b+c-0x1000000
 else
  return 0x10000*a+0x100*b+c
 end
end
function files.readinteger3le(f)
 local c,b,a=byte(f:read(3),1,3)
 if a>=0x80 then
  return 0x10000*a+0x100*b+c-0x1000000
 else
  return 0x10000*a+0x100*b+c
 end
end
function files.readcardinal4(f)
 local a,b,c,d=byte(f:read(4),1,4)
 return 0x1000000*a+0x10000*b+0x100*c+d
end
function files.readcardinal4le(f)
 local d,c,b,a=byte(f:read(4),1,4)
 return 0x1000000*a+0x10000*b+0x100*c+d
end
function files.readinteger4(f)
 local a,b,c,d=byte(f:read(4),1,4)
 if a>=0x80 then
  return 0x1000000*a+0x10000*b+0x100*c+d-0x100000000
 else
  return 0x1000000*a+0x10000*b+0x100*c+d
 end
end
function files.readinteger4le(f)
 local d,c,b,a=byte(f:read(4),1,4)
 if a>=0x80 then
  return 0x1000000*a+0x10000*b+0x100*c+d-0x100000000
 else
  return 0x1000000*a+0x10000*b+0x100*c+d
 end
end
function files.readfixed2(f)
 local a,b=byte(f:read(2),1,2)
 if a>=0x80 then
  tonumber((a-0x100).."."..b)
 else
  tonumber((a    ).."."..b)
 end
end
function files.readfixed4(f)
 local a,b,c,d=byte(f:read(4),1,4)
 if a>=0x80 then
  tonumber((0x100*a+b-0x10000).."."..(0x100*c+d))
 else
  tonumber((0x100*a+b    ).."."..(0x100*c+d))
 end
end
if bit32 then
 local extract=bit32.extract
 local band=bit32.band
 function files.read2dot14(f)
  local a,b=byte(f:read(2),1,2)
  if a>=0x80 then
   local n=-(0x100*a+b)
   return-(extract(n,14,2)+(band(n,0x3FFF)/16384.0))
  else
   local n=0x100*a+b
   return   (extract(n,14,2)+(band(n,0x3FFF)/16384.0))
  end
 end
end
function files.skipshort(f,n)
 f:read(2*(n or 1))
end
function files.skiplong(f,n)
 f:read(4*(n or 1))
end
if bit32 then
 local rshift=bit32.rshift
 function files.writecardinal2(f,n)
  local a=char(n%256)
  n=rshift(n,8)
  local b=char(n%256)
  f:write(b,a)
 end
 function files.writecardinal4(f,n)
  local a=char(n%256)
  n=rshift(n,8)
  local b=char(n%256)
  n=rshift(n,8)
  local c=char(n%256)
  n=rshift(n,8)
  local d=char(n%256)
  f:write(d,c,b,a)
 end
 function files.writecardinal2le(f,n)
  local a=char(n%256)
  n=rshift(n,8)
  local b=char(n%256)
  f:write(a,b)
 end
 function files.writecardinal4le(f,n)
  local a=char(n%256)
  n=rshift(n,8)
  local b=char(n%256)
  n=rshift(n,8)
  local c=char(n%256)
  n=rshift(n,8)
  local d=char(n%256)
  f:write(a,b,c,d)
 end
else
 local floor=math.floor
 function files.writecardinal2(f,n)
  local a=char(n%256)
  n=floor(n/256)
  local b=char(n%256)
  f:write(b,a)
 end
 function files.writecardinal4(f,n)
  local a=char(n%256)
  n=floor(n/256)
  local b=char(n%256)
  n=floor(n/256)
  local c=char(n%256)
  n=floor(n/256)
  local d=char(n%256)
  f:write(d,c,b,a)
 end
 function files.writecardinal2le(f,n)
  local a=char(n%256)
  n=floor(n/256)
  local b=char(n%256)
  f:write(a,b)
 end
 function files.writecardinal4le(f,n)
  local a=char(n%256)
  n=floor(n/256)
  local b=char(n%256)
  n=floor(n/256)
  local c=char(n%256)
  n=floor(n/256)
  local d=char(n%256)
  f:write(a,b,c,d)
 end
end
function files.writestring(f,s)
 f:write(char(byte(s,1,#s)))
end
function files.writebyte(f,b)
 f:write(char(b))
end
if fio and fio.readcardinal1 then
 files.readcardinal1=fio.readcardinal1
 files.readcardinal2=fio.readcardinal2
 files.readcardinal3=fio.readcardinal3
 files.readcardinal4=fio.readcardinal4
 files.readcardinal1le=fio.readcardinal1le or files.readcardinal1le
 files.readcardinal2le=fio.readcardinal2le or files.readcardinal2le
 files.readcardinal3le=fio.readcardinal3le or files.readcardinal3le
 files.readcardinal4le=fio.readcardinal4le or files.readcardinal4le
 files.readinteger1=fio.readinteger1
 files.readinteger2=fio.readinteger2
 files.readinteger3=fio.readinteger3
 files.readinteger4=fio.readinteger4
 files.readinteger1le=fio.readinteger1le or files.readinteger1le
 files.readinteger2le=fio.readinteger2le or files.readinteger2le
 files.readinteger3le=fio.readinteger3le or files.readinteger3le
 files.readinteger4le=fio.readinteger4le or files.readinteger4le
 files.readfixed2=fio.readfixed2
 files.readfixed4=fio.readfixed4
 files.read2dot14=fio.read2dot14
 files.setposition=fio.setposition
 files.getposition=fio.getposition
 files.readbyte=files.readcardinal1
 files.readsignedbyte=files.readinteger1
 files.readcardinal=files.readcardinal1
 files.readinteger=files.readinteger1
 local skipposition=fio.skipposition
 files.skipposition=skipposition
 files.readbytes=fio.readbytes
 files.readbytetable=fio.readbytetable
 function files.skipshort(f,n)
  skipposition(f,2*(n or 1))
 end
 function files.skiplong(f,n)
  skipposition(f,4*(n or 1))
 end
end
if fio and fio.writecardinal1 then
 files.writecardinal1=fio.writecardinal1
 files.writecardinal2=fio.writecardinal2
 files.writecardinal3=fio.writecardinal3
 files.writecardinal4=fio.writecardinal4
 files.writecardinal1le=fio.writecardinal1le
 files.writecardinal2le=fio.writecardinal2le
 files.writecardinal3le=fio.writecardinal3le
 files.writecardinal4le=fio.writecardinal4le
 files.writeinteger1=fio.writeinteger1 or fio.writecardinal1
 files.writeinteger2=fio.writeinteger2 or fio.writecardinal2
 files.writeinteger3=fio.writeinteger3 or fio.writecardinal3
 files.writeinteger4=fio.writeinteger4 or fio.writecardinal4
 files.writeinteger1le=files.writeinteger1le or fio.writecardinal1le
 files.writeinteger2le=files.writeinteger2le or fio.writecardinal2le
 files.writeinteger3le=files.writeinteger3le or fio.writecardinal3le
 files.writeinteger4le=files.writeinteger4le or fio.writecardinal4le
end
if fio and fio.readcardinaltable then
 files.readcardinaltable=fio.readcardinaltable
 files.readintegertable=fio.readintegertable
else
 local readcardinal1=files.readcardinal1
 local readcardinal2=files.readcardinal2
 local readcardinal3=files.readcardinal3
 local readcardinal4=files.readcardinal4
 function files.readcardinaltable(f,n,b)
  local t={}
   if b==1 then for i=1,n do t[i]=readcardinal1(f) end
  elseif b==2 then for i=1,n do t[i]=readcardinal2(f) end
  elseif b==3 then for i=1,n do t[i]=readcardinal3(f) end
  elseif b==4 then for i=1,n do t[i]=readcardinal4(f) end end
  return t
 end
 local readinteger1=files.readinteger1
 local readinteger2=files.readinteger2
 local readinteger3=files.readinteger3
 local readinteger4=files.readinteger4
 function files.readintegertable(f,n,b)
  local t={}
   if b==1 then for i=1,n do t[i]=readinteger1(f) end
  elseif b==2 then for i=1,n do t[i]=readinteger2(f) end
  elseif b==3 then for i=1,n do t[i]=readinteger3(f) end
  elseif b==4 then for i=1,n do t[i]=readinteger4(f) end end
  return t
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-sac"] = package.loaded["util-sac"] or true

-- original size: 12946, stripped down to: 9507

if not modules then modules={} end modules ['util-sac']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local byte,sub=string.byte,string.sub
local tonumber=tonumber
utilities=utilities or {}
local streams={}
utilities.streams=streams
function streams.open(filename,zerobased)
 local f=filename and io.loaddata(filename)
 if f then
  return { f,1,#f,zerobased or false }
 end
end
function streams.openstring(f,zerobased)
 if f then
  return { f,1,#f,zerobased or false }
 end
end
function streams.getstring(f)
 if f then
  return f[1]
 end
end
function streams.close()
end
function streams.size(f)
 return f and f[3] or 0
end
streams.getsize=streams.size
function streams.setposition(f,i)
 if f[4] then
  if i<=0 then
   f[2]=1
  else
   f[2]=i+1
  end
 else
  if i<=1 then
   f[2]=1
  else
   f[2]=i
  end
 end
end
function streams.getposition(f)
 if f[4] then
  return f[2]-1
 else
  return f[2]
 end
end
function streams.look(f,n,chars)
 local b=f[2]
 local e=b+n-1
 if chars then
  return sub(f[1],b,e)
 else
  return byte(f[1],b,e)
 end
end
function streams.skip(f,n)
 f[2]=f[2]+n
end
function streams.readbyte(f)
 local i=f[2]
 f[2]=i+1
 return byte(f[1],i)
end
function streams.readbytes(f,n)
 local i=f[2]
 local j=i+n
 f[2]=j
 return byte(f[1],i,j-1)
end
function streams.readbytetable(f,n)
 local i=f[2]
 local j=i+n
 f[2]=j
 return { byte(f[1],i,j-1) }
end
function streams.skipbytes(f,n)
 f[2]=f[2]+n
end
function streams.readchar(f)
 local i=f[2]
 f[2]=i+1
 return sub(f[1],i,i)
end
function streams.readstring(f,n)
 local i=f[2]
 local j=i+n
 f[2]=j
 return sub(f[1],i,j-1)
end
function streams.readinteger1(f)  
 local i=f[2]
 f[2]=i+1
 local n=byte(f[1],i)
 if n>=0x80 then
  return n-0x100
 else
  return n
 end
end
streams.readcardinal1=streams.readbyte  
streams.readcardinal=streams.readcardinal1
streams.readinteger=streams.readinteger1
function streams.readcardinal2(f)
 local i=f[2]
 local j=i+1
 f[2]=j+1
 local a,b=byte(f[1],i,j)
 return 0x100*a+b
end
function streams.readcardinal2le(f)
 local i=f[2]
 local j=i+1
 f[2]=j+1
 local b,a=byte(f[1],i,j)
 return 0x100*a+b
end
function streams.readinteger2(f)
 local i=f[2]
 local j=i+1
 f[2]=j+1
 local a,b=byte(f[1],i,j)
 if a>=0x80 then
  return 0x100*a+b-0x10000
 else
  return 0x100*a+b
 end
end
function streams.readinteger2le(f)
 local i=f[2]
 local j=i+1
 f[2]=j+1
 local b,a=byte(f[1],i,j)
 if a>=0x80 then
  return 0x100*a+b-0x10000
 else
  return 0x100*a+b
 end
end
function streams.readcardinal3(f)
 local i=f[2]
 local j=i+2
 f[2]=j+1
 local a,b,c=byte(f[1],i,j)
 return 0x10000*a+0x100*b+c
end
function streams.readcardinal3le(f)
 local i=f[2]
 local j=i+2
 f[2]=j+1
 local c,b,a=byte(f[1],i,j)
 return 0x10000*a+0x100*b+c
end
function streams.readinteger3(f)
 local i=f[2]
 local j=i+3
 f[2]=j+1
 local a,b,c=byte(f[1],i,j)
 if a>=0x80 then
  return 0x10000*a+0x100*b+c-0x1000000
 else
  return 0x10000*a+0x100*b+c
 end
end
function streams.readinteger3le(f)
 local i=f[2]
 local j=i+3
 f[2]=j+1
 local c,b,a=byte(f[1],i,j)
 if a>=0x80 then
  return 0x10000*a+0x100*b+c-0x1000000
 else
  return 0x10000*a+0x100*b+c
 end
end
function streams.readcardinal4(f)
 local i=f[2]
 local j=i+3
 f[2]=j+1
 local a,b,c,d=byte(f[1],i,j)
 return 0x1000000*a+0x10000*b+0x100*c+d
end
function streams.readcardinal4le(f)
 local i=f[2]
 local j=i+3
 f[2]=j+1
 local d,c,b,a=byte(f[1],i,j)
 return 0x1000000*a+0x10000*b+0x100*c+d
end
function streams.readinteger4(f)
 local i=f[2]
 local j=i+3
 f[2]=j+1
 local a,b,c,d=byte(f[1],i,j)
 if a>=0x80 then
  return 0x1000000*a+0x10000*b+0x100*c+d-0x100000000
 else
  return 0x1000000*a+0x10000*b+0x100*c+d
 end
end
function streams.readinteger4le(f)
 local i=f[2]
 local j=i+3
 f[2]=j+1
 local d,c,b,a=byte(f[1],i,j)
 if a>=0x80 then
  return 0x1000000*a+0x10000*b+0x100*c+d-0x100000000
 else
  return 0x1000000*a+0x10000*b+0x100*c+d
 end
end
function streams.readfixed2(f)
 local i=f[2]
 local j=i+1
 f[2]=j+1
 local a,b=byte(f[1],i,j)
 if a>=0x80 then
  return tonumber((a-0x100).."."..b) or 0
 else
  return tonumber((a  ).."."..b) or 0
 end
end
function streams.readfixed4(f)
 local i=f[2]
 local j=i+3
 f[2]=j+1
 local a,b,c,d=byte(f[1],i,j)
 if a>=0x80 then
  return tonumber((0x100*a+b-0x10000).."."..(0x100*c+d)) or 0
 else
  return tonumber((0x100*a+b    ).."."..(0x100*c+d)) or 0
 end
end
if bit32 then
 local extract=bit32.extract
 local band=bit32.band
 function streams.read2dot14(f)
  local i=f[2]
  local j=i+1
  f[2]=j+1
  local a,b=byte(f[1],i,j)
  if a>=0x80 then
   local n=-(0x100*a+b)
   return-(extract(n,14,2)+(band(n,0x3FFF)/16384.0))
  else
   local n=0x100*a+b
   return   (extract(n,14,2)+(band(n,0x3FFF)/16384.0))
  end
 end
end
function streams.skipshort(f,n)
 f[2]=f[2]+2*(n or 1)
end
function streams.skiplong(f,n)
 f[2]=f[2]+4*(n or 1)
end
if sio and sio.readcardinal2 then
 local readcardinal1=sio.readcardinal1
 local readcardinal2=sio.readcardinal2
 local readcardinal3=sio.readcardinal3
 local readcardinal4=sio.readcardinal4
 local readinteger1=sio.readinteger1
 local readinteger2=sio.readinteger2
 local readinteger3=sio.readinteger3
 local readinteger4=sio.readinteger4
 local readfixed2=sio.readfixed2
 local readfixed4=sio.readfixed4
 local read2dot14=sio.read2dot14
 local readbytes=sio.readbytes
 local readbytetable=sio.readbytetable
 function streams.readcardinal1(f)
  local i=f[2]
  f[2]=i+1
  return readcardinal1(f[1],i)
 end
 function streams.readcardinal2(f)
  local i=f[2]
  f[2]=i+2
  return readcardinal2(f[1],i)
 end
 function streams.readcardinal3(f)
  local i=f[2]
  f[2]=i+3
  return readcardinal3(f[1],i)
 end
 function streams.readcardinal4(f)
  local i=f[2]
  f[2]=i+4
  return readcardinal4(f[1],i)
 end
 function streams.readinteger1(f)
  local i=f[2]
  f[2]=i+1
  return readinteger1(f[1],i)
 end
 function streams.readinteger2(f)
  local i=f[2]
  f[2]=i+2
  return readinteger2(f[1],i)
 end
 function streams.readinteger3(f)
  local i=f[2]
  f[2]=i+3
  return readinteger3(f[1],i)
 end
 function streams.readinteger4(f)
  local i=f[2]
  f[2]=i+4
  return readinteger4(f[1],i)
 end
 function streams.readfixed2(f) 
  local i=f[2]
  f[2]=i+2
  return readfixed2(f[1],i)
 end
 function streams.readfixed4(f) 
  local i=f[2]
  f[2]=i+4
  return readfixed4(f[1],i)
 end
 function streams.read2dot4(f)
  local i=f[2]
  f[2]=i+2
  return read2dot4(f[1],i)
 end
 function streams.readbytes(f,n)
  local i=f[2]
  local s=f[3]
  local p=i+n
  if p>s then
   f[2]=s+1
  else
   f[2]=p
  end
  return readbytes(f[1],i,n)
 end
 function streams.readbytetable(f,n)
  local i=f[2]
  local s=f[3]
  local p=i+n
  if p>s then
   f[2]=s+1
  else
   f[2]=p
  end
  return readbytetable(f[1],i,n)
 end
 streams.readbyte=streams.readcardinal1
 streams.readsignedbyte=streams.readinteger1
 streams.readcardinal=streams.readcardinal1
 streams.readinteger=streams.readinteger1
end
if sio and sio.readcardinaltable then
 local readcardinaltable=sio.readcardinaltable
 local readintegertable=sio.readintegertable
 function utilities.streams.readcardinaltable(f,n,b)
  local i=f[2]
  local s=f[3]
  local p=i+n*b
  if p>s then
   f[2]=s+1
  else
   f[2]=p
  end
  return readcardinaltable(f[1],i,n,b)
 end
 function utilities.streams.readintegertable(f,n,b)
  local i=f[2]
  local s=f[3]
  local p=i+n*b
  if p>s then
   f[2]=s+1
  else
   f[2]=p
  end
  return readintegertable(f[1],i,n,b)
 end
else
 local readcardinal1=streams.readcardinal1
 local readcardinal2=streams.readcardinal2
 local readcardinal3=streams.readcardinal3
 local readcardinal4=streams.readcardinal4
 function streams.readcardinaltable(f,n,b)
  local i=f[2]
  local s=f[3]
  local p=i+n*b
  if p>s then
   f[2]=s+1
  else
   f[2]=p
  end
  local t={}
   if b==1 then for i=1,n do t[i]=readcardinal1(f[1],i) end
  elseif b==2 then for i=1,n do t[i]=readcardinal2(f[1],i) end
  elseif b==3 then for i=1,n do t[i]=readcardinal3(f[1],i) end
  elseif b==4 then for i=1,n do t[i]=readcardinal4(f[1],i) end end
  return t
 end
 local readinteger1=streams.readinteger1
 local readinteger2=streams.readinteger2
 local readinteger3=streams.readinteger3
 local readinteger4=streams.readinteger4
 function streams.readintegertable(f,n,b)
  local i=f[2]
  local s=f[3]
  local p=i+n*b
  if p>s then
   f[2]=s+1
  else
   f[2]=p
  end
  local t={}
   if b==1 then for i=1,n do t[i]=readinteger1(f[1],i) end
  elseif b==2 then for i=1,n do t[i]=readinteger2(f[1],i) end
  elseif b==3 then for i=1,n do t[i]=readinteger3(f[1],i) end
  elseif b==4 then for i=1,n do t[i]=readinteger4(f[1],i) end end
  return t
 end
end
do
 local files=utilities.files
 if files then
  local openfile=files.open
  local openstream=streams.open
  local openstring=streams.openstring
  local setmetatable=setmetatable
  function io.newreader(str,method)
   local f,m
   if method=="string" then
    f=openstring(str)
    m=streams
   elseif method=="stream" then
    f=openstream(str)
    m=streams
   else
    f=openfile(str,"rb")
    m=files
   end
   if f then
    local t={}
    setmetatable(t,{
     __index=function(t,k)
      local r=m[k]
      if k=="close" then
       if f then
        m.close(f)
        f=nil
       end
       return function() end
      elseif r then
       local v=function(_,a,b) return r(f,a,b) end
       t[k]=v
       return v
      else
       print("unknown key",k)
      end
     end
    } )
    return t
   end
  end
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-sto"] = package.loaded["util-sto"] or true

-- original size: 6661, stripped down to: 3074

if not modules then modules={} end modules ['util-sto']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local setmetatable,getmetatable,rawset,type=setmetatable,getmetatable,rawset,type
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
local function f_empty ()         return "" end 
local function f_self  (t,k) t[k]=k      return k  end
local function f_table (t,k) local v={} t[k]=v return v  end
local function f_number(t,k) t[k]=0      return 0  end 
local function f_ignore()          end 
local f_index={
 ["empty"]=f_empty,
 ["self"]=f_self,
 ["table"]=f_table,
 ["number"]=f_number,
}
function table.setmetatableindex(t,f)
 if type(t)~="table" then
  f,t=t,{}
 end
 local m=getmetatable(t)
 local i=f_index[f] or f
 if m then
  m.__index=i
 else
  setmetatable(t,{ __index=i })
 end
 return t
end
local f_index={
 ["ignore"]=f_ignore,
}
function table.setmetatablenewindex(t,f)
 if type(t)~="table" then
  f,t=t,{}
 end
 local m=getmetatable(t)
 local i=f_index[f] or f
 if m then
  m.__newindex=i
 else
  setmetatable(t,{ __newindex=i })
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
function table.setmetatableindices(t,f,n,c)
 if type(t)~="table" then
  f,t=t,{}
 end
 local m=getmetatable(t)
 local i=f_index[f] or f
 if m then
  m.__index=i
  m.__newindex=n
  m.__call=c
 else
  setmetatable(t,{
   __index=i,
   __newindex=n,
   __call=c,
  })
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
function table.makeweak(t)
 if not t then
  t={}
 end
 local m=getmetatable(t)
 if m then
  m.__mode="v"
 else
  setmetatable(t,{ __mode="v" })
 end
 return t
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-prs"] = package.loaded["util-prs"] or true

-- original size: 24132, stripped down to: 16210

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
local concat,gmatch,find=table.concat,string.gmatch,string.find
local tostring,type,next,rawset=tostring,type,next,rawset
local mod,div=math.mod,math.div
utilities=utilities or {}
local parsers=utilities.parsers or {}
utilities.parsers=parsers
local patterns=parsers.patterns or {}
parsers.patterns=patterns
local setmetatableindex=table.setmetatableindex
local sortedhash=table.sortedhash
local sortedkeys=table.sortedkeys
local tohash=table.tohash
local hashes={}
parsers.hashes=hashes
local digit=R("09")
local space=P(' ')
local equal=P("=")
local colon=P(":")
local comma=P(",")
local lbrace=P("{")
local rbrace=P("}")
local lparent=P("(")
local rparent=P(")")
local lbracket=P("[")
local rbracket=P("]")
local period=S(".")
local punctuation=S(".,:;")
local spacer=lpegpatterns.spacer
local whitespace=lpegpatterns.whitespace
local newline=lpegpatterns.newline
local anything=lpegpatterns.anything
local endofstring=lpegpatterns.endofstring
local nobrace=1-(lbrace+rbrace )
local noparent=1-(lparent+rparent)
local nobracket=1-(lbracket+rbracket)
local escape,left,right=P("\\"),P('{'),P('}')
lpegpatterns.balanced=P {
 [1]=((escape*(left+right))+(1-(left+right))+V(2))^0,
 [2]=left*V(1)*right
}
local nestedbraces=P { lbrace*(nobrace+V(1))^0*rbrace }
local nestedparents=P { lparent*(noparent+V(1))^0*rparent }
local nestedbrackets=P { lbracket*(nobracket+V(1))^0*rbracket }
local spaces=space^0
local argument=Cs((lbrace/"")*((nobrace+nestedbraces)^0)*(rbrace/""))
local content=(1-endofstring)^0
lpegpatterns.nestedbraces=nestedbraces  
lpegpatterns.nestedparents=nestedparents 
lpegpatterns.nested=nestedbraces  
lpegpatterns.argument=argument   
lpegpatterns.content=content    
local value=lbrace*C((nobrace+nestedbraces)^0)*rbrace+C((nestedbraces+(1-comma))^0)
local key=C((1-equal-comma)^1)
local pattern_a=(space+comma)^0*(key*equal*value+key*C(""))
local pattern_c=(space+comma)^0*(key*equal*value)
local pattern_d=(space+comma)^0*(key*(equal+colon)*value+key*C(""))
local key=C((1-space-equal-comma)^1)
local pattern_b=spaces*comma^0*spaces*(key*((spaces*equal*spaces*value)+C("")))
local hash={}
local function set(key,value)
 hash[key]=value
end
local pattern_a_s=(pattern_a/set)^1
local pattern_b_s=(pattern_b/set)^1
local pattern_c_s=(pattern_c/set)^1
local pattern_d_s=(pattern_d/set)^1
patterns.settings_to_hash_a=pattern_a_s
patterns.settings_to_hash_b=pattern_b_s
patterns.settings_to_hash_c=pattern_c_s
patterns.settings_to_hash_d=pattern_d_s
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
 if not str or str=="" then
  return {}
 elseif type(str)=="table" then
  if existing then
   for k,v in next,str do
    existing[k]=v
   end
   return exiting
  else
   return str
  end
 else
  hash=existing or {}
  lpegmatch(pattern_a_s,str)
  return hash
 end
end
function parsers.settings_to_hash_colon_too(str)
 if not str or str=="" then
  return {}
 elseif type(str)=="table" then
  return str
 else
  hash={}
  lpegmatch(pattern_d_s,str)
  return hash
 end
end
function parsers.settings_to_hash_tolerant(str,existing)
 if not str or str=="" then
  return {}
 elseif type(str)=="table" then
  if existing then
   for k,v in next,str do
    existing[k]=v
   end
   return exiting
  else
   return str
  end
 else
  hash=existing or {}
  lpegmatch(pattern_b_s,str)
  return hash
 end
end
function parsers.settings_to_hash_strict(str,existing)
 if not str or str=="" then
  return nil
 elseif type(str)=="table" then
  if existing then
   for k,v in next,str do
    existing[k]=v
   end
   return exiting
  else
   return str
  end
 elseif str and str~="" then
  hash=existing or {}
  lpegmatch(pattern_c_s,str)
  return next(hash) and hash
 end
end
local separator=comma*space^0
local value=lbrace*C((nobrace+nestedbraces)^0)*rbrace+C((nestedbraces+(1-comma))^0)
local pattern=spaces*Ct(value*(separator*value)^0)
patterns.settings_to_array=pattern
function parsers.settings_to_array(str,strict)
 if not str or str=="" then
  return {}
 elseif type(str)=="table" then
  return str
 elseif strict then
  if find(str,"{",1,true) then
   return lpegmatch(pattern,str)
  else
   return { str }
  end
 elseif find(str,",",1,true) then
  return lpegmatch(pattern,str)
 else
  return { str }
 end
end
function parsers.settings_to_numbers(str)
 if not str or str=="" then
  return {}
 end
 if type(str)=="table" then
 elseif find(str,",",1,true) then
  str=lpegmatch(pattern,str)
 else
  return { tonumber(str) }
 end
 for i=1,#str do
  str[i]=tonumber(str[i])
 end
 return str
end
local value=lbrace*C((nobrace+nestedbraces)^0)*rbrace+C((nestedbraces+nestedbrackets+nestedparents+(1-comma))^0)
local pattern=spaces*Ct(value*(separator*value)^0)
function parsers.settings_to_array_obey_fences(str)
 return lpegmatch(pattern,str)
end
local cache_a={}
local cache_b={}
function parsers.groupedsplitat(symbol,withaction)
 if not symbol then
  symbol=","
 end
 local pattern=(withaction and cache_b or cache_a)[symbol]
 if not pattern then
  local symbols=S(symbol)
  local separator=space^0*symbols*space^0
  local value=lbrace*C((nobrace+nestedbraces)^0)*rbrace+C((nestedbraces+(1-(space^0*(symbols+P(-1)))))^0)
  if withaction then
   local withvalue=Carg(1)*value/function(f,s) return f(s) end
   pattern=spaces*withvalue*(separator*withvalue)^0
   cache_b[symbol]=pattern
  else
   pattern=spaces*Ct(value*(separator*value)^0)
   cache_a[symbol]=pattern
  end
 end
 return pattern
end
local pattern_a=parsers.groupedsplitat(",",false)
local pattern_b=parsers.groupedsplitat(",",true)
function parsers.stripped_settings_to_array(str)
 if not str or str=="" then
  return {}
 else
  return lpegmatch(pattern_a,str)
 end
end
function parsers.process_stripped_settings(str,action)
 if not str or str=="" then
  return {}
 else
  return lpegmatch(pattern_b,str,1,action)
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
  local t={}
  local tn=0
  local s=sortedkeys(h)
  omit=omit and tohash(omit)
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
local pattern=Cf(Ct("")*Cg(C((1-S(", "))^1)*S(", ")^0*Cc(true))^1,rawset)
function parsers.settings_to_set(str)
 return str and lpegmatch(pattern,str) or {}
end
hashes.settings_to_set=table.setmetatableindex(function(t,k) 
 local v=k and lpegmatch(pattern,k) or {}
 t[k]=v
 return v
end)
getmetatable(hashes.settings_to_set).__mode="kv" 
function parsers.simple_hash_to_string(h,separator)
 local t={}
 local tn=0
 for k,v in sortedhash(h) do
  if v then
   tn=tn+1
   t[tn]=k
  end
 end
 return concat(t,separator or ",")
end
local str=Cs(lpegpatterns.unquoted)+C((1-whitespace-equal)^1)
local setting=Cf(Carg(1)*(whitespace^0*Cg(str*whitespace^0*(equal*whitespace^0*str+Cc(""))))^1,rawset)
local splitter=setting^1
function parsers.options_to_hash(str,target)
 return str and lpegmatch(splitter,str,1,target or {}) or {}
end
local splitter=lpeg.tsplitat(" ")
function parsers.options_to_array(str)
 return str and lpegmatch(splitter,str) or {}
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
   local t={}
   local tn=0
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
local pattern=Cf(Ct("")*(Cg(key*equal*value)*separator^0)^1,rawset)^0*P(-1)
function parsers.keq_to_hash(str)
 if str and str~="" then
  return lpegmatch(pattern,str)
 else
  return {}
 end
end
local defaultspecification={ separator=",",quote='"' }
function parsers.csvsplitter(specification)
 specification=specification and setmetatableindex(specification,defaultspecification) or defaultspecification
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
 local parser=Ct((Ct(whatever*(separator*whatever)^0)*S("\n\r")^1)^0 )
 return function(data)
  return lpegmatch(parser,data)
 end
end
function parsers.rfc4180splitter(specification)
 specification=specification and setmetatableindex(specification,defaultspecification) or defaultspecification
 local separator=specification.separator 
 local quotechar=P(specification.quote)  
 local dquotechar=quotechar*quotechar   
/specification.quote
 local separator=S(separator~="" and separator or ",")
 local escaped=quotechar*Cs((dquotechar+(1-quotechar))^0)*quotechar
 local non_escaped=C((1-quotechar-newline-separator)^1)
 local field=escaped+non_escaped+Cc("")
 local record=Ct(field*(separator*field)^1)
 local headerline=record*Cp()
 local morerecords=(newline^(specification.strict and -1 or 1)*record)^0
 local headeryes=Ct(morerecords)
 local headernop=Ct(record*morerecords)
 return function(data,getheader)
  if getheader then
   local header,position=lpegmatch(headerline,data)
   local data=lpegmatch(headeryes,data,position)
   return data,header
  else
   return lpegmatch(headernop,data)
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
local stepper=spacers*(cardinal*(spacers*S(":-")*spacers*(cardinal+Cc(true) )+Cc(false) )*Carg(1)*Carg(2)/ranger*S(", ")^0 )^1
local stepper=spacers*(cardinal*(spacers*S(":-")*spacers*(cardinal+(P("*")+endofstring)*Cc(true) )+Cc(false) )*Carg(1)*Carg(2)/ranger*S(", ")^0 )^1*endofstring 
function parsers.stepper(str,n,action)
 if type(n)=="function" then
  lpegmatch(stepper,str,1,false,n or print)
 else
  lpegmatch(stepper,str,1,n,action or print)
 end
end
local pattern_math=Cs((P("%")/"\\percent "+P("^")*Cc("{")*lpegpatterns.integer*Cc("}")+anything)^0)
local pattern_text=Cs((P("%")/"\\percent "+(P("^")/"\\high")*Cc("{")*lpegpatterns.integer*Cc("}")+anything)^0)
patterns.unittotex=pattern
function parsers.unittotex(str,textmode)
 return lpegmatch(textmode and pattern_text or pattern_math,str)
end
local pattern=Cs((P("^")/"<sup>"*lpegpatterns.integer*Cc("</sup>")+anything)^0)
function parsers.unittoxml(str)
 return lpegmatch(pattern,str)
end
local cache={}
local spaces=lpegpatterns.space^0
local dummy=function() end
setmetatableindex(cache,function(t,k)
 local separator=P(k)
 local value=(1-separator)^0
 local pattern=spaces*C(value)*separator^0*Cp()
 t[k]=pattern
 return pattern
end)
local commalistiterator=cache[","]
function parsers.iterator(str,separator)
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
local function process(result,more)
 for k,v in next,more do
  result[k]=v
 end
 return result
end
local name=C((1-S(", "))^1)
local parser=(Carg(1)*name/initialize)*(S(", ")^1*(Carg(1)*name/fetch))^0
local merge=Cf(parser,process)
function parsers.mergehashes(hash,list)
 return lpegmatch(merge,list,1,hash)
end
function parsers.runtime(time)
 if not time then
  time=os.runtime()
 end
 local days=div(time,24*60*60)
 time=mod(time,24*60*60)
 local hours=div(time,60*60)
 time=mod(time,60*60)
 local minutes=div(time,60)
 local seconds=mod(time,60)
 return days,hours,minutes,seconds
end
local spacing=whitespace^0
local apply=P("->")
local method=C((1-apply)^1)
local token=lbrace*C((1-rbrace)^1)*rbrace+C(anything^1)
local pattern=spacing*(method*spacing*apply+Carg(1))*spacing*token
function parsers.splitmethod(str,default)
 if str then
  return lpegmatch(pattern,str,1,default or false)
 else
  return default or false,""
 end
end
local p_year=lpegpatterns.digit^4/tonumber
local pattern=Cf(Ct("")*(
  (Cg(Cc("year")*p_year)*S("-/")*Cg(Cc("month")*cardinal)*S("-/")*Cg(Cc("day")*cardinal)
  )+(Cg(Cc("day")*cardinal)*S("-/")*Cg(Cc("month")*cardinal)*S("-/")*Cg(Cc("year")*p_year)
  )
 )*P(" ")*Cg(Cc("hour")*cardinal)*P(":")*Cg(Cc("min")*cardinal)*(P(":")*Cg(Cc("sec")*cardinal))^-1
,rawset)
lpegpatterns.splittime=pattern
function parsers.totime(str)
 return lpegmatch(pattern,str)
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-fmt"] = package.loaded["util-fmt"] or true

-- original size: 2541, stripped down to: 1624

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
     rj=tostring(rj)
    elseif tj~="string" then
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

package.loaded["util-soc-imp-reset"] = package.loaded["util-soc-imp-reset"] or true

-- original size: 374, stripped down to: 282

local loaded=package.loaded
loaded["socket"]=nil
loaded["copas"]=nil
loaded["ltn12"]=nil
loaded["mbox"]=nil
loaded["mime"]=nil
loaded["socket.url"]=nil
loaded["socket.headers"]=nil
loaded["socket.tp"]=nil
loaded["socket.http"]=nil
loaded["socket.ftp"]=nil
loaded["socket.smtp"]=nil


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-soc-imp-socket"] = package.loaded["util-soc-imp-socket"] or true

-- original size: 4905, stripped down to: 3562


local type,tostring,setmetatable=type,tostring,setmetatable
local min=math.min
local format=string.format
local socket=socket or package.loaded.socket or require("socket.core")
local connect=socket.connect
local tcp4=socket.tcp4
local tcp6=socket.tcp6
local getaddrinfo=socket.dns.getaddrinfo
local defaulthost="0.0.0.0"
local function report(fmt,first,...)
 if logs then
  report=logs and logs.reporter("socket")
  report(fmt,first,...)
 elseif fmt then
  fmt="socket: "..fmt
  if first then
   print(format(fmt,first,...))
  else
   print(fmt)
  end
 end
end
socket.report=report
function socket.connect4(address,port,laddress,lport)
 return connect(address,port,laddress,lport,"inet")
end
function socket.connect6(address,port,laddress,lport)
 return connect(address,port,laddress,lport,"inet6")
end
function socket.bind(host,port,backlog)
 if host=="*" or host=="" then
  host=defaulthost
 end
 local addrinfo,err=getaddrinfo(host)
 if not addrinfo then
  return nil,err
 end
 for i=1,#addrinfo do
  local alt=addrinfo[i]
  local sock,err=(alt.family=="inet" and tcp4 or tcp6)()
  if not sock then
   return nil,err or "unknown error"
  end
  sock:setoption("reuseaddr",true)
  local res,err=sock:bind(alt.addr,port)
  if res then
   res,err=sock:listen(backlog)
   if res then
    return sock
   else
    sock:close()
   end
  else
   sock:close()
  end
 end
 return nil,"invalid address"
end
socket.try=socket.newtry()
function socket.choose(list)
 return function(name,opt1,opt2)
  if type(name)~="string" then
   name,opt1,opt2="default",name,opt1
  end
  local f=list[name or "nil"]
  if f then
   return f(opt1,opt2)
  else
   report("error: unknown key '%s'",tostring(name))
  end
 end
end
local sourcet={}
local sinkt={}
socket.sourcet=sourcet
socket.sinkt=sinkt
socket.BLOCKSIZE=2048
sinkt["close-when-done"]=function(sock)
 return setmetatable (
  {
   getfd=function() return sock:getfd() end,
   dirty=function() return sock:dirty() end,
  },
  {
   __call=function(self,chunk,err)
    if chunk then
     return sock:send(chunk)
    else
     sock:close()
     return 1 
    end
   end
  }
 )
end
sinkt["keep-open"]=function(sock)
 return setmetatable (
  {
   getfd=function() return sock:getfd() end,
   dirty=function() return sock:dirty() end,
  },{
   __call=function(self,chunk,err)
    if chunk then
     return sock:send(chunk)
    else
     return 1 
    end
   end
  }
 )
end
sinkt["default"]=sinkt["keep-open"]
socket.sink=socket.choose(sinkt)
sourcet["by-length"]=function(sock,length)
 local blocksize=socket.BLOCKSIZE
 return setmetatable (
  {
   getfd=function() return sock:getfd() end,
   dirty=function() return sock:dirty() end,
  },
  {
   __call=function()
    if length<=0 then
     return nil
    end
    local chunk,err=sock:receive(min(blocksize,length))
    if err then
     return nil,err
    end
    length=length-#chunk
    return chunk
   end
  }
 )
end
sourcet["until-closed"]=function(sock)
 local blocksize=socket.BLOCKSIZE
 local done=false
 return setmetatable (
  {
   getfd=function() return sock:getfd() end,
   dirty=function() return sock:dirty() end,
  },{
   __call=function()
    if done then
     return nil
    end
    local chunk,status,partial=sock:receive(blocksize)
    if not status then
     return chunk
    elseif status=="closed" then
     sock:close()
     done=true
     return partial
    else
     return nil,status
    end
   end
  }
 )
end
sourcet["default"]=sourcet["until-closed"]
socket.source=socket.choose(sourcet)
_G.socket=socket 
package.loaded["socket"]=socket


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-soc-imp-copas"] = package.loaded["util-soc-imp-copas"] or true

-- original size: 26186, stripped down to: 14893


local socket=socket or require("socket")
local ssl=ssl or nil 
local WATCH_DOG_TIMEOUT=120
local UDP_DATAGRAM_MAX=8192
local type,next,pcall,getmetatable,tostring=type,next,pcall,getmetatable,tostring
local min,max,random=math.min,math.max,math.random
local find=string.find
local insert,remove=table.insert,table.remove
local gettime=socket.gettime
local selectsocket=socket.select
local createcoroutine=coroutine.create
local resumecoroutine=coroutine.resume
local yieldcoroutine=coroutine.yield
local runningcoroutine=coroutine.running
local function report(fmt,first,...)
 if logs then
  report=logs and logs.reporter("copas")
  report(fmt,first,...)
 elseif fmt then
  fmt="copas: "..fmt
  if first then
   print(format(fmt,first,...))
  else
   print(fmt)
  end
 end
end
local copas={
 _COPYRIGHT="Copyright (C) 2005-2016 Kepler Project",
 _DESCRIPTION="Coroutine Oriented Portable Asynchronous Services",
 _VERSION="Copas 2.0.1",
 autoclose=true,
 running=false,
 report=report,
 trace=false,
}
local function statushandler(status,...)
 if status then
  return...
 end
 local err=(...)
 if type(err)=="table" then
  err=err[1]
 end
 if copas.trace then
  report("error: %s",tostring(err))
 end
 return nil,err
end
function socket.protect(func)
 return function(...)
  return statushandler(pcall(func,...))
 end
end
function socket.newtry(finalizer)
 return function (...)
  local status=(...)
  if not status then
   local detail=select(2,...)
   pcall(finalizer,detail)
   if copas.trace then
    report("error: %s",tostring(detail))
   end
   return
  end
  return...
 end
end
local function newset()
 local reverse={}
 local set={}
 local queue={}
 setmetatable(set,{
  __index={
   insert=function(set,value)
     if not reverse[value] then
      local n=#set+1
      set[n]=value
      reverse[value]=n
     end
    end,
   remove=function(set,value)
     local index=reverse[value]
     if index then
      reverse[value]=nil
      local n=#set
      local top=set[n]
      set[n]=nil
      if top~=value then
       reverse[top]=index
       set[index]=top
      end
     end
    end,
   push=function (set,key,itm)
     local entry=queue[key]
     if entry==nil then 
      queue[key]={ itm }
     else
      entry[#entry+1]=itm
     end
    end,
   pop=function (set,key)
     local top=queue[key]
     if top~=nil then
      local ret=remove(top,1)
      if top[1]==nil then
       queue[key]=nil
      end
      return ret
     end
    end
  }
 } )
 return set
end
local _sleeping={
 times={},
 cos={},
 lethargy={},
 insert=function()
  end,
 remove=function()
  end,
 push=function(self,sleeptime,co)
   if not co then
    return
   end
   if sleeptime<0 then
    self.lethargy[co]=true
    return
   else
    sleeptime=gettime()+sleeptime
   end
   local t=self.times
   local c=self.cos
   local i=1
   local n=#t
   while i<=n and t[i]<=sleeptime do
    i=i+1
   end
   insert(t,i,sleeptime)
   insert(c,i,co)
  end,
 getnext=
  function(self)
   local t=self.times
   local delay=t[1] and t[1]-gettime() or nil
   return delay and max(delay,0) or nil
  end,
 pop=
  function(self,time)
   local t=self.times
   local c=self.cos
   if #t==0 or time<t[1] then
    return
   end
   local co=c[1]
   remove(t,1)
   remove(c,1)
   return co
  end,
  wakeup=function(self,co)
    local let=self.lethargy
    if let[co] then
     self:push(0,co)
     let[co]=nil
    else
     local c=self.cos
     local t=self.times
     for i=1,#c do
      if c[i]==co then
       remove(c,i)
       remove(t,i)
       self:push(0,co)
       return
      end
     end
    end
   end
}
local _servers=newset() 
local _reading=newset() 
local _writing=newset() 
local _reading_log={}
local _writing_log={}
local _is_timeout={  
 timeout=true,
 wantread=true,
 wantwrite=true,
}
local function isTCP(socket)
 return not find(tostring(socket),"^udp")
end
local function copasreceive(client,pattern,part)
 if not pattern or pattern=="" then
  pattern="*l"
 end
 local current_log=_reading_log
 local s,err
 repeat
  s,err,part=client:receive(pattern,part)
  if s or (not _is_timeout[err]) then
   current_log[client]=nil
   return s,err,part
  end
  if err=="wantwrite" then
   current_log=_writing_log
   current_log[client]=gettime()
   yieldcoroutine(client,_writing)
  else
   current_log=_reading_log
   current_log[client]=gettime()
   yieldcoroutine(client,_reading)
  end
 until false
end
local function copasreceivefrom(client,size)
 local s,err,port
 if not size or size==0 then
  size=UDP_DATAGRAM_MAX
 end
 repeat
  s,err,port=client:receivefrom(size)
  if s or err~="timeout" then
   _reading_log[client]=nil
   return s,err,port
  end
  _reading_log[client]=gettime()
  yieldcoroutine(client,_reading)
 until false
end
local function copasreceivepartial(client,pattern,part)
 if not pattern or pattern=="" then
  pattern="*l"
 end
 local logger=_reading_log
 local queue=_reading
 local s,err
 repeat
  s,err,part=client:receive(pattern,part)
  if s or (type(pattern)=="number" and part~="" and part) or not _is_timeout[err] then
    logger[client]=nil
    return s,err,part
  end
  if err=="wantwrite" then
   logger=_writing_log
   queue=_writing
  else
   logger=_reading_log
   queue=_reading
  end
  logger[client]=gettime()
  yieldcoroutine(client,queue)
 until false
end
local function copassend(client,data,from,to)
 if not from then
  from=1
 end
 local lastIndex=from-1
 local logger=_writing_log
 local queue=_writing
 local s,err
 repeat
  s,err,lastIndex=client:send(data,lastIndex+1,to)
  if random(100)>90 then
   logger[client]=gettime()
   yieldcoroutine(client,queue)
  end
  if s or not _is_timeout[err] then
   logger[client]=nil
   return s,err,lastIndex
  end
  if err=="wantread" then
   logger=_reading_log
   queue=_reading
  else
   logger=_writing_log
   queue=_writing
  end
  logger[client]=gettime()
  yieldcoroutine(client,queue)
 until false
end
local function copassendto(client,data,ip,port)
 repeat
  local s,err=client:sendto(data,ip,port)
  if random(100)>90 then
   _writing_log[client]=gettime()
   yieldcoroutine(client,_writing)
  end
  if s or err~="timeout" then
   _writing_log[client]=nil
   return s,err
  end
  _writing_log[client]=gettime()
  yieldcoroutine(client,_writing)
 until false
end
local function copasconnect(skt,host,port)
 skt:settimeout(0)
 local ret,err,tried_more_than_once
 repeat
  ret,err=skt:connect (host,port)
  if ret or (err~="timeout" and err~="Operation already in progress") then
   if not ret and err=="already connected" and tried_more_than_once then
    ret=1
    err=nil
   end
   _writing_log[skt]=nil
   return ret,err
  end
  tried_more_than_once=tried_more_than_once or true
  _writing_log[skt]=gettime()
  yieldcoroutine(skt,_writing)
 until false
end
local function copasdohandshake(skt,sslt) 
 if not ssl then
  ssl=require("ssl")
 end
 if not ssl then
  report("error: no ssl library")
  return
 end
 local nskt,err=ssl.wrap(skt,sslt)
 if not nskt then
  report("error: %s",tostring(err))
  return
 end
 nskt:settimeout(0)
 local queue
 repeat
  local success,err=nskt:dohandshake()
  if success then
   return nskt
  elseif err=="wantwrite" then
   queue=_writing
  elseif err=="wantread" then
   queue=_reading
  else
   report("error: %s",tostring(err))
   return
  end
  yieldcoroutine(nskt,queue)
 until false
end
local function copasflush(client)
end
copas.connect=copassconnect
copas.send=copassend
copas.sendto=copassendto
copas.receive=copasreceive
copas.receivefrom=copasreceivefrom
copas.copasreceivepartial=copasreceivepartial
copas.copasreceivePartial=copasreceivepartial
copas.dohandshake=copasdohandshake
copas.flush=copasflush
local function _skt_mt_tostring(self)
 return tostring(self.socket).." (copas wrapped)"
end
local _skt_mt_tcp_index={
 send=function(self,data,from,to)
   return copassend (self.socket,data,from,to)
  end,
 receive=function (self,pattern,prefix)
   if self.timeout==0 then
    return copasreceivePartial(self.socket,pattern,prefix)
   else
    return copasreceive(self.socket,pattern,prefix)
   end
  end,
 flush=function (self)
   return copasflush(self.socket)
  end,
 settimeout=function (self,time)
   self.timeout=time
   return true
  end,
 connect=function(self,...)
   local res,err=copasconnect(self.socket,...)
   if res and self.ssl_params then
    res,err=self:dohandshake()
   end
   return res,err
  end,
 close=function(self,...)
   return self.socket:close(...)
  end,
 bind=function(self,...)
   return self.socket:bind(...)
  end,
 getsockname=function(self,...)
   return self.socket:getsockname(...)
  end,
 getstats=function(self,...)
   return self.socket:getstats(...)
  end,
 setstats=function(self,...)
   return self.socket:setstats(...)
  end,
 listen=function(self,...)
   return self.socket:listen(...)
  end,
 accept=function(self,...)
   return self.socket:accept(...)
  end,
 setoption=function(self,...)
   return self.socket:setoption(...)
  end,
 getpeername=function(self,...)
   return self.socket:getpeername(...)
  end,
 shutdown=function(self,...)
   return self.socket:shutdown(...)
  end,
 dohandshake=function(self,sslt)
   self.ssl_params=sslt or self.ssl_params
   local nskt,err=copasdohandshake(self.socket,self.ssl_params)
   if not nskt then
    return nskt,err
   end
   self.socket=nskt
   return self
  end,
}
local _skt_mt_tcp={
 __tostring=_skt_mt_tostring,
 __index=_skt_mt_tcp_index,
}
local _skt_mt_udp_index={
 sendto=function (self,...)
   return copassendto(self.socket,...)
  end,
 receive=function (self,size)
   return copasreceive(self.socket,size or UDP_DATAGRAM_MAX)
  end,
 receivefrom=function (self,size)
   return copasreceivefrom(self.socket,size or UDP_DATAGRAM_MAX)
  end,
 setpeername=function(self,...)
   return self.socket:getpeername(...)
  end,
 setsockname=function(self,...)
   return self.socket:setsockname(...)
  end,
 close=function(self,...)
   return true
  end
}
local _skt_mt_udp={
 __tostring=_skt_mt_tostring,
 __index=_skt_mt_udp_index,
}
for k,v in next,_skt_mt_tcp_index do
 if not _skt_mt_udp_index[k] then
  _skt_mt_udp_index[k]=v
 end
end
local function wrap(skt,sslt)
 if getmetatable(skt)==_skt_mt_tcp or getmetatable(skt)==_skt_mt_udp then
  return skt 
 end
 skt:settimeout(0)
 if isTCP(skt) then
  return setmetatable ({ socket=skt,ssl_params=sslt },_skt_mt_tcp)
 else
  return setmetatable ({ socket=skt },_skt_mt_udp)
 end
end
copas.wrap=wrap
function copas.handler(handler,sslparams)
 return function (skt,...)
  skt=wrap(skt)
  if sslparams then
   skt:dohandshake(sslparams)
  end
  return handler(skt,...)
 end
end
local _errhandlers={}
function copas.setErrorHandler(err)
 local co=runningcoroutine()
 if co then
  _errhandlers[co]=err
 end
end
local function _deferror (msg,co,skt)
 report("%s (%s) (%s)",msg,tostring(co),tostring(skt))
end
local function _doTick (co,skt,...)
 if not co then
  return
 end
 local ok,res,new_q=resumecoroutine(co,skt,...)
 if ok and res and new_q then
  new_q:insert(res)
  new_q:push(res,co)
 else
  if not ok then
   pcall(_errhandlers[co] or _deferror,res,co,skt)
  end
  if skt and copas.autoclose and isTCP(skt) then
   skt:close()
  end
  _errhandlers[co]=nil
 end
end
local function _accept(input,handler)
 local client=input:accept()
 if client then
  client:settimeout(0)
  local co=createcoroutine(handler)
  _doTick (co,client)
 end
 return client
end
local function _tickRead(skt)
 _doTick(_reading:pop(skt),skt)
end
local function _tickWrite(skt)
 _doTick(_writing:pop(skt),skt)
end
local function addTCPserver(server,handler,timeout)
 server:settimeout(timeout or 0)
 _servers[server]=handler
 _reading:insert(server)
end
local function addUDPserver(server,handler,timeout)
 server:settimeout(timeout or 0)
 local co=createcoroutine(handler)
 _reading:insert(server)
 _doTick(co,server)
end
function copas.addserver(server,handler,timeout)
 if isTCP(server) then
  addTCPserver(server,handler,timeout)
 else
  addUDPserver(server,handler,timeout)
 end
end
function copas.removeserver(server,keep_open)
 local s=server
 local mt=getmetatable(server)
 if mt==_skt_mt_tcp or mt==_skt_mt_udp then
  s=server.socket
 end
 _servers[s]=nil
 _reading:remove(s)
 if keep_open then
  return true
 end
 return server:close()
end
function copas.addthread(handler,...)
 local thread=createcoroutine(function(_,...) return handler(...) end)
 _doTick(thread,nil,...)
 return thread
end
local _tasks={}
local function addtaskRead(task)
 task.def_tick=_tickRead
 _tasks[task]=true
end
local function addtaskWrite(task)
 task.def_tick=_tickWrite
 _tasks[task]=true
end
local function tasks()
 return next,_tasks
end
local _readable_t={
 events=function(self)
   local i=0
   return function ()
    i=i+1
    return self._evs[i]
   end
  end,
 tick=function(self,input)
   local handler=_servers[input]
   if handler then
    input=_accept(input,handler)
   else
    _reading:remove(input)
    self.def_tick(input)
   end
  end
}
addtaskRead(_readable_t)
local _writable_t={
 events=function(self)
   local i=0
   return function()
    i=i+1
    return self._evs[i]
   end
  end,
 tick=function(self,output)
   _writing:remove(output)
   self.def_tick(output)
  end
}
addtaskWrite(_writable_t)
local _sleeping_t={
 tick=function(self,time,...)
  _doTick(_sleeping:pop(time),...)
 end
}
function copas.sleep(sleeptime)
 yieldcoroutine((sleeptime or 0),_sleeping)
end
function copas.wakeup(co)
 _sleeping:wakeup(co)
end
local last_cleansing=0
local function _select(timeout)
 local now=gettime()
 local r_evs,w_evs,err=selectsocket(_reading,_writing,timeout)
 _readable_t._evs=r_evs
 _writable_t._evs=w_evs
 if (last_cleansing-now)>WATCH_DOG_TIMEOUT then
  last_cleansing=now
  for skt,time in next,_reading_log do
   if not r_evs[skt] and (time-now)>WATCH_DOG_TIMEOUT then
    local n=#r_evs+1
    _reading_log[skt]=nil
    r_evs[n]=skt
    r_evs[skt]=n
   end
  end
  for skt,time in next,_writing_log do
   if not w_evs[skt] and (time-now)>WATCH_DOG_TIMEOUT then
    local n=#w_evs+1
    _writing_log[skt]=nil
    w_evs[n]=skt
    w_evs[skt]=n
   end
  end
 end
 if err=="timeout" and #r_evs+#w_evs>0 then
  return nil
 else
  return err
 end
end
local function copasfinished()
 return not (next(_reading) or next(_writing) or _sleeping:getnext())
end
local function copasstep(timeout)
 _sleeping_t:tick(gettime())
 local nextwait=_sleeping:getnext()
 if nextwait then
  timeout=timeout and min(nextwait,timeout) or nextwait
 elseif copasfinished() then
  return false
 end
 local err=_select(timeout)
 if err then
  if err=="timeout" then
   return false
  end
  return nil,err
 end
 for task in tasks() do
  for event in task:events() do
   task:tick(event)
  end
 end
 return true
end
copas.finished=copasfinished
copas.step=copasstep
function copas.loop(timeout)
 copas.running=true
 while not copasfinished() do
  copasstep(timeout)
 end
 copas.running=false
end
package.loaded["copas"]=copas


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-soc-imp-ltn12"] = package.loaded["util-soc-imp-ltn12"] or true

-- original size: 8709, stripped down to: 5411


local select,unpack=select,unpack
local insert,remove=table.insert,table.remove
local sub=string.sub
local function report(fmt,first,...)
 if logs then
  report=logs and logs.reporter("ltn12")
  report(fmt,first,...)
 elseif fmt then
  fmt="ltn12: "..fmt
  if first then
   print(format(fmt,first,...))
  else
   print(fmt)
  end
 end
end
local filter={}
local source={}
local sink={}
local pump={}
local ltn12={
 _VERSION="LTN12 1.0.3",
 BLOCKSIZE=2048,
 filter=filter,
 source=source,
 sink=sink,
 pump=pump,
 report=report,
}
function filter.cycle(low,ctx,extra)
 if low then
  return function(chunk)
   return (low(ctx,chunk,extra))
  end
 end
end
function filter.chain(...)
 local arg={... }
 local n=select('#',...)
 local top=1
 local index=1
 local retry=""
 return function(chunk)
  retry=chunk and retry
  while true do
   local action=arg[index]
   if index==top then
    chunk=action(chunk)
    if chunk=="" or top==n then
     return chunk
    elseif chunk then
     index=index+1
    else
     top=top+1
     index=top
    end
   else
    chunk=action(chunk or "")
    if chunk=="" then
     index=index-1
     chunk=retry
    elseif chunk then
     if index==n then
      return chunk
     else
      index=index+1
     end
    else
     report("error: filter returned inappropriate 'nil'")
     return
    end
   end
  end
 end
end
local function empty()
 return nil
end
function source.empty()
 return empty
end
local function sourceerror(err)
 return function()
  return nil,err
 end
end
source.error=sourceerror
function source.file(handle,io_err)
 if handle then
  local blocksize=ltn12.BLOCKSIZE
  return function()
   local chunk=handle:read(blocksize)
   if not chunk then
    handle:close()
   end
   return chunk
  end
 else
  return sourceerror(io_err or "unable to open file")
 end
end
function source.simplify(src)
 return function()
  local chunk,err_or_new=src()
  if err_or_new then
   src=err_or_new
  end
  if chunk then
   return chunk
  else
   return nil,err_or_new
  end
 end
end
function source.string(s)
 if s then
  local blocksize=ltn12.BLOCKSIZE
  local i=1
  return function()
   local nexti=i+blocksize
   local chunk=sub(s,i,nexti-1)
   i=nexti
   if chunk~="" then
    return chunk
   else
    return nil
   end
  end
 else return source.empty() end
end
function source.rewind(src)
 local t={}
 return function(chunk)
  if chunk then
   insert(t,chunk)
  else
   chunk=remove(t)
   if chunk then
    return chunk
   else
    return src()
   end
  end
 end
end
function source.chain(src,f,...)
 if... then
  f=filter.chain(f,...)
 end
 local last_in=""
 local last_out=""
 local state="feeding"
 local err
 return function()
  if not last_out then
   report("error: source is empty")
   return
  end
  while true do
   if state=="feeding" then
    last_in,err=src()
    if err then
     return nil,err
    end
    last_out=f(last_in)
    if not last_out then
     if last_in then
      report("error: filter returned inappropriate 'nil'")
     end
     return nil
    elseif last_out~="" then
     state="eating"
     if last_in then
      last_in=""
     end
     return last_out
    end
   else
    last_out=f(last_in)
    if last_out=="" then
     if last_in=="" then
      state="feeding"
     else
      report("error: filter returned nothing")
      return
     end
    elseif not last_out then
     if last_in then
      report("filter returned inappropriate 'nil'")
     end
     return nil
    else
     return last_out
    end
   end
  end
 end
end
function source.cat(...)
 local arg={... }
 local src=remove(arg,1)
 return function()
  while src do
   local chunk,err=src()
   if chunk then
    return chunk
   end
   if err then
    return nil,err
   end
   src=remove(arg,1)
  end
 end
end
function sink.table(t)
 if not t then
  t={}
 end
 local f=function(chunk,err)
  if chunk then
   insert(t,chunk)
  end
  return 1
 end
 return f,t
end
function sink.simplify(snk)
 return function(chunk,err)
  local ret,err_or_new=snk(chunk,err)
  if not ret then
   return nil,err_or_new
  end
  if err_or_new then
   snk=err_or_new
  end
  return 1
 end
end
local function null()
 return 1
end
function sink.null()
 return null
end
local function sinkerror(err)
 return function()
  return nil,err
 end
end
sink.error=sinkerror
function sink.file(handle,io_err)
 if handle then
  return function(chunk,err)
   if not chunk then
    handle:close()
    return 1
   else
    return handle:write(chunk)
   end
  end
 else
  return sinkerror(io_err or "unable to open file")
 end
end
function sink.chain(f,snk,...)
 if... then
  local args={ f,snk,... }
  snk=remove(args,#args)
  f=filter.chain(unpack(args))
 end
 return function(chunk,err)
  if chunk~="" then
   local filtered=f(chunk)
   local done=chunk and ""
   while true do
    local ret,snkerr=snk(filtered,err)
    if not ret then
     return nil,snkerr
    end
    if filtered==done then
     return 1
    end
    filtered=f(done)
   end
  else
   return 1
  end
 end
end
function pump.step(src,snk)
 local chunk,src_err=src()
 local ret,snk_err=snk(chunk,src_err)
 if chunk and ret then
  return 1
 else
  return nil,src_err or snk_err
 end
end
function pump.all(src,snk,step)
 if not step then
  step=pump.step
 end
 while true do
  local ret,err=step(src,snk)
  if not ret then
   if err then
    return nil,err
   else
    return 1
   end
  end
 end
end
package.loaded["ltn12"]=ltn12


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-soc-imp-mime"] = package.loaded["util-soc-imp-mime"] or true

-- original size: 2373, stripped down to: 1931


local type,tostring=type,tostring
local mime=mime  or package.loaded.mime  or require("mime.core")
local ltn12=ltn12 or package.loaded.ltn12 or require("ltn12")
local filtercycle=ltn12.filter.cycle
local function report(fmt,first,...)
 if logs then
  report=logs and logs.reporter("mime")
  report(fmt,first,...)
 elseif fmt then
  fmt="mime: "..fmt
  if first then
   print(format(fmt,first,...))
  else
   print(fmt)
  end
 end
end
mime.report=report
local encodet={}
local decodet={}
local wrapt={}
mime.encodet=encodet
mime.decodet=decodet
mime.wrapt=wrapt
local mime_b64=mime.b64
local mime_qp=mime.qp
local mime_unb64=mime.unb64
local mime_unqp=mime.unqp
local mime_wrp=mime.wrp
local mime_qpwrp=mime.qpwrp
local mime_eol=mime_eol
local mime_dot=mime_dot
encodet['base64']=function()
 return filtercycle(mime_b64,"")
end
encodet['quoted-printable']=function(mode)
 return filtercycle(mime_qp,"",mode=="binary" and "=0D=0A" or "\r\n")
end
decodet['base64']=function()
 return filtercycle(mime_unb64,"")
end
decodet['quoted-printable']=function()
 return filtercycle(mime_unqp,"")
end
local wraptext=function(length)
 if not length then
  length=76
 end
 return filtercycle(mime_wrp,length,length)
end
local wrapquoted=function()
 return filtercycle(mime_qpwrp,76,76)
end
wrapt['text']=wraptext
wrapt['base64']=wraptext
wrapt['default']=wraptext
wrapt['quoted-printable']=wrapquoted
function mime.normalize(marker)
 return filtercycle(mime_eol,0,marker)
end
function mime.stuff()
 return filtercycle(mime_dot,2)
end
local function choose(list)
 return function(name,opt1,opt2)
  if type(name)~="string" then
   name,opt1,opt2="default",name,opt1
  end
  local filter=list[name or "nil"]
  if filter then
   return filter(opt1,opt2)
  else
   report("error: unknown key '%s'",tostring(name))
  end
 end
end
mime.encode=choose(encodet)
mime.decode=choose(decodet)
mime.wrap=choose(wrapt)
package.loaded["mime"]=mime


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-soc-imp-url"] = package.loaded["util-soc-imp-url"] or true

-- original size: 6863, stripped down to: 5269


local tonumber,tostring,type=tonumber,tostring,type
local gsub,sub,match,find,format,byte,char=string.gsub,string.sub,string.match,string.find,string.format,string.byte,string.char
local insert=table.insert
local socket=socket or require("socket")
local url={
 _VERSION="URL 1.0.3",
}
socket.url=url
function url.escape(s)
 return (gsub(s,"([^A-Za-z0-9_])",function(c)
  return format("%%%02x",byte(c))
 end))
end
local function make_set(t) 
 local s={}
 for i=1,#t do
  s[t[i]]=true
 end
 return s
end
local segment_set=make_set {
 "-","_",".","!","~","*","'","(",
 ")",":","@","&","=","+","$",",",
}
local function protect_segment(s)
 return gsub(s,"([^A-Za-z0-9_])",function(c)
  if segment_set[c] then
   return c
  else
   return format("%%%02X",byte(c))
  end
 end)
end
function url.unescape(s)
 return (gsub(s,"%%(%x%x)",function(hex)
  return char(tonumber(hex,16))
 end))
end
local function absolute_path(base_path,relative_path)
 if find(relative_path,"^/") then
  return relative_path
 end
 local path=gsub(base_path,"[^/]*$","")
 path=path..relative_path
 path=gsub(path,"([^/]*%./)",function (s)
  if s~="./" then
   return s
  else
   return ""
  end
 end)
 path=gsub(path,"/%.$","/")
 local reduced
 while reduced~=path do
  reduced=path
  path=gsub(reduced,"([^/]*/%.%./)",function (s)
   if s~="../../" then
    return ""
   else
    return s
   end
  end)
 end
 path=gsub(reduced,"([^/]*/%.%.)$",function (s)
  if s~="../.." then
   return ""
  else
   return s
  end
 end)
 return path
end
function url.parse(url,default)
 local parsed={}
 for k,v in next,default or parsed do
  parsed[k]=v
 end
 if not url or url=="" then
  return nil,"invalid url"
 end
 url=gsub(url,"#(.*)$",function(f)
  parsed.fragment=f
  return ""
 end)
 url=gsub(url,"^([%w][%w%+%-%.]*)%:",function(s)
  parsed.scheme=s
  return ""
 end)
 url=gsub(url,"^//([^/]*)",function(n)
  parsed.authority=n
  return ""
 end)
 url=gsub(url,"%?(.*)",function(q)
  parsed.query=q
  return ""
 end)
 url=gsub(url,"%;(.*)",function(p)
  parsed.params=p
  return ""
 end)
 if url~="" then
  parsed.path=url
 end
 local authority=parsed.authority
 if not authority then
  return parsed
 end
 authority=gsub(authority,"^([^@]*)@",function(u)
  parsed.userinfo=u
  return ""
 end)
 authority=gsub(authority,":([^:%]]*)$",function(p)
  parsed.port=p
  return ""
 end)
 if authority~="" then
  parsed.host=match(authority,"^%[(.+)%]$") or authority
 end
 local userinfo=parsed.userinfo
 if not userinfo then
  return parsed
 end
 userinfo=gsub(userinfo,":([^:]*)$",function(p)
  parsed.password=p
  return ""
 end)
 parsed.user=userinfo
 return parsed
end
function url.build(parsed)
 local url=parsed.path or ""
 if parsed.params then
  url=url..";"..parsed.params
 end
 if parsed.query then
  url=url.."?"..parsed.query
 end
 local authority=parsed.authority
 if parsed.host then
  authority=parsed.host
  if find(authority,":") then 
   authority="["..authority.."]"
  end
  if parsed.port then
   authority=authority..":"..tostring(parsed.port)
  end
  local userinfo=parsed.userinfo
  if parsed.user then
   userinfo=parsed.user
   if parsed.password then
    userinfo=userinfo..":"..parsed.password
   end
  end
  if userinfo then authority=userinfo.."@"..authority end
 end
 if authority then
  url="//"..authority..url
 end
 if parsed.scheme then
  url=parsed.scheme..":"..url
 end
 if parsed.fragment then
  url=url.."#"..parsed.fragment
 end
 return url
end
function url.absolute(base_url,relative_url)
 local base_parsed
 if type(base_url)=="table" then
  base_parsed=base_url
  base_url=url.build(base_parsed)
 else
  base_parsed=url.parse(base_url)
 end
 local relative_parsed=url.parse(relative_url)
 if not base_parsed then
  return relative_url
 elseif not relative_parsed then
  return base_url
 elseif relative_parsed.scheme then
  return relative_url
 else
  relative_parsed.scheme=base_parsed.scheme
  if not relative_parsed.authority then
   relative_parsed.authority=base_parsed.authority
   if not relative_parsed.path then
    relative_parsed.path=base_parsed.path
    if not relative_parsed.params then
     relative_parsed.params=base_parsed.params
     if not relative_parsed.query then
      relative_parsed.query=base_parsed.query
     end
    end
   else
    relative_parsed.path=absolute_path(base_parsed.path or "",relative_parsed.path)
   end
  end
  return url.build(relative_parsed)
 end
end
function url.parse_path(path)
 local parsed={}
 path=path or ""
 gsub(path,"([^/]+)",function (s)
  insert(parsed,s)
 end)
 for i=1,#parsed do
  parsed[i]=url.unescape(parsed[i])
 end
 if sub(path,1,1)=="/" then
  parsed.is_absolute=1
 end
 if sub(path,-1,-1)=="/" then
  parsed.is_directory=1
 end
 return parsed
end
function url.build_path(parsed,unsafe)
 local path=""
 local n=#parsed
 if unsafe then
  for i=1,n-1 do
   path=path..parsed[i].."/"
  end
  if n>0 then
   path=path..parsed[n]
   if parsed.is_directory then
    path=path.."/"
   end
  end
 else
  for i=1,n-1 do
   path=path..protect_segment(parsed[i]).."/"
  end
  if n>0 then
   path=path..protect_segment(parsed[n])
   if parsed.is_directory then
    path=path.."/"
   end
  end
 end
 if parsed.is_absolute then
  path="/"..path
 end
 return path
end
package.loaded["socket.url"]=url


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-soc-imp-headers"] = package.loaded["util-soc-imp-headers"] or true

-- original size: 5721, stripped down to: 3754


local next=next
local lower=string.lower
local concat=table.concat
local socket=socket or require("socket")
local headers={}
socket.headers=headers
local canonic={
 ["accept"]="Accept",
 ["accept-charset"]="Accept-Charset",
 ["accept-encoding"]="Accept-Encoding",
 ["accept-language"]="Accept-Language",
 ["accept-ranges"]="Accept-Ranges",
 ["action"]="Action",
 ["alternate-recipient"]="Alternate-Recipient",
 ["age"]="Age",
 ["allow"]="Allow",
 ["arrival-date"]="Arrival-Date",
 ["authorization"]="Authorization",
 ["bcc"]="Bcc",
 ["cache-control"]="Cache-Control",
 ["cc"]="Cc",
 ["comments"]="Comments",
 ["connection"]="Connection",
 ["content-description"]="Content-Description",
 ["content-disposition"]="Content-Disposition",
 ["content-encoding"]="Content-Encoding",
 ["content-id"]="Content-ID",
 ["content-language"]="Content-Language",
 ["content-length"]="Content-Length",
 ["content-location"]="Content-Location",
 ["content-md5"]="Content-MD5",
 ["content-range"]="Content-Range",
 ["content-transfer-encoding"]="Content-Transfer-Encoding",
 ["content-type"]="Content-Type",
 ["cookie"]="Cookie",
 ["date"]="Date",
 ["diagnostic-code"]="Diagnostic-Code",
 ["dsn-gateway"]="DSN-Gateway",
 ["etag"]="ETag",
 ["expect"]="Expect",
 ["expires"]="Expires",
 ["final-log-id"]="Final-Log-ID",
 ["final-recipient"]="Final-Recipient",
 ["from"]="From",
 ["host"]="Host",
 ["if-match"]="If-Match",
 ["if-modified-since"]="If-Modified-Since",
 ["if-none-match"]="If-None-Match",
 ["if-range"]="If-Range",
 ["if-unmodified-since"]="If-Unmodified-Since",
 ["in-reply-to"]="In-Reply-To",
 ["keywords"]="Keywords",
 ["last-attempt-date"]="Last-Attempt-Date",
 ["last-modified"]="Last-Modified",
 ["location"]="Location",
 ["max-forwards"]="Max-Forwards",
 ["message-id"]="Message-ID",
 ["mime-version"]="MIME-Version",
 ["original-envelope-id"]="Original-Envelope-ID",
 ["original-recipient"]="Original-Recipient",
 ["pragma"]="Pragma",
 ["proxy-authenticate"]="Proxy-Authenticate",
 ["proxy-authorization"]="Proxy-Authorization",
 ["range"]="Range",
 ["received"]="Received",
 ["received-from-mta"]="Received-From-MTA",
 ["references"]="References",
 ["referer"]="Referer",
 ["remote-mta"]="Remote-MTA",
 ["reply-to"]="Reply-To",
 ["reporting-mta"]="Reporting-MTA",
 ["resent-bcc"]="Resent-Bcc",
 ["resent-cc"]="Resent-Cc",
 ["resent-date"]="Resent-Date",
 ["resent-from"]="Resent-From",
 ["resent-message-id"]="Resent-Message-ID",
 ["resent-reply-to"]="Resent-Reply-To",
 ["resent-sender"]="Resent-Sender",
 ["resent-to"]="Resent-To",
 ["retry-after"]="Retry-After",
 ["return-path"]="Return-Path",
 ["sender"]="Sender",
 ["server"]="Server",
 ["smtp-remote-recipient"]="SMTP-Remote-Recipient",
 ["status"]="Status",
 ["subject"]="Subject",
 ["te"]="TE",
 ["to"]="To",
 ["trailer"]="Trailer",
 ["transfer-encoding"]="Transfer-Encoding",
 ["upgrade"]="Upgrade",
 ["user-agent"]="User-Agent",
 ["vary"]="Vary",
 ["via"]="Via",
 ["warning"]="Warning",
 ["will-retry-until"]="Will-Retry-Until",
 ["www-authenticate"]="WWW-Authenticate",
 ["x-mailer"]="X-Mailer",
}
headers.canonic=setmetatable(canonic,{
 __index=function(t,k)
  socket.report("invalid header: %s",k)
  t[k]=k
  return k
 end
})
function headers.normalize(headers)
 if not headers then
  return {}
 end
 local normalized={}
 for k,v in next,headers do
  normalized[#normalized+1]=canonic[k]..": "..v
 end
 normalized[#normalized+1]=""
 normalized[#normalized+1]=""
 return concat(normalized,"\r\n")
end
function headers.lower(lowered,headers)
 if not lowered then
  return {}
 end
 if not headers then
  lowered,headers={},lowered
 end
 for k,v in next,headers do
  lowered[lower(k)]=v
 end
 return lowered
end
socket.headers=headers
package.loaded["socket.headers"]=headers


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-soc-imp-tp"] = package.loaded["util-soc-imp-tp"] or true

-- original size: 3116, stripped down to: 2533


local setmetatable,next,type,tonumber=setmetatable,next,type,tonumber
local find,upper=string.find,string.upper
local socket=socket or require("socket")
local ltn12=ltn12  or require("ltn12")
local skipsocket=socket.skip
local sinksocket=socket.sink
local tcpsocket=socket.tcp
local ltn12pump=ltn12.pump
local pumpall=ltn12pump.all
local pumpstep=ltn12pump.step
local tp={
 TIMEOUT=60,
}
socket.tp=tp
local function get_reply(c)
 local line,err=c:receive()
 local reply=line
 if err then return
  nil,err
 end
 local code,sep=skipsocket(2,find(line,"^(%d%d%d)(.?)"))
 if not code then
  return nil,"invalid server reply"
 end
 if sep=="-" then
  local current
  repeat
   line,err=c:receive()
   if err then
    return nil,err
   end
   current,sep=skipsocket(2,find(line,"^(%d%d%d)(.?)"))
   reply=reply.."\n"..line
  until code==current and sep==" "
 end
 return code,reply
end
local methods={}
local mt={ __index=methods }
function methods.getpeername(self)
 return self.c:getpeername()
end
function methods.getsockname(self)
 return self.c:getpeername()
end
function methods.check(self,ok)
 local code,reply=get_reply(self.c)
 if not code then
  return nil,reply
 end
 local c=tonumber(code)
 local t=type(ok)
 if t=="function" then
  return ok(c,reply)
 elseif t=="table" then
  for i=1,#ok do
   if find(code,ok[i]) then
    return c,reply
   end
  end
  return nil,reply
 elseif find(code,ok) then
  return c,reply
 else
  return nil,reply
 end
end
function methods.command(self,cmd,arg)
 cmd=upper(cmd)
 if arg then
  cmd=cmd.." "..arg.."\r\n"
 else
  cmd=cmd.."\r\n"
 end
 return self.c:send(cmd)
end
function methods.sink(self,snk,pat)
 local chunk,err=self.c:receive(pat)
 return snk(chunk,err)
end
function methods.send(self,data)
 return self.c:send(data)
end
function methods.receive(self,pat)
 return self.c:receive(pat)
end
function methods.getfd(self)
 return self.c:getfd()
end
function methods.dirty(self)
 return self.c:dirty()
end
function methods.getcontrol(self)
 return self.c
end
function methods.source(self,source,step)
 local sink=sinksocket("keep-open",self.c)
 local ret,err=pumpall(source,sink,step or pumpstep)
 return ret,err
end
function methods.close(self)
 self.c:close()
 return 1
end
function tp.connect(host,port,timeout,create)
 local c,e=(create or tcpsocket)()
 if not c then
  return nil,e
 end
 c:settimeout(timeout or tp.TIMEOUT)
 local r,e=c:connect(host,port)
 if not r then
  c:close()
  return nil,e
 end
 return setmetatable({ c=c },mt)
end
package.loaded["socket.tp"]=tp


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-soc-imp-http"] = package.loaded["util-soc-imp-http"] or true

-- original size: 12577, stripped down to: 9577


local tostring,tonumber,setmetatable,next,type=tostring,tonumber,setmetatable,next,type
local find,lower,format,gsub,match=string.find,string.lower,string.format,string.gsub,string.match
local concat=table.concat
local socket=socket   or require("socket")
local url=socket.url  or require("socket.url")
local ltn12=ltn12    or require("ltn12")
local mime=mime     or require("mime")
local headers=socket.headers or require("socket.headers")
local normalizeheaders=headers.normalize
local parseurl=url.parse
local buildurl=url.build
local absoluteurl=url.absolute
local unescapeurl=url.unescape
local skipsocket=socket.skip
local sinksocket=socket.sink
local sourcesocket=socket.source
local trysocket=socket.try
local tcpsocket=socket.tcp
local newtrysocket=socket.newtry
local protectsocket=socket.protect
local emptysource=ltn12.source.empty
local stringsource=ltn12.source.string
local rewindsource=ltn12.source.rewind
local pumpstep=ltn12.pump.step
local pumpall=ltn12.pump.all
local sinknull=ltn12.sink.null
local sinktable=ltn12.sink.table
local lowerheaders=headers.lower
local mimeb64=mime.b64
local http={
 TIMEOUT=60,
 USERAGENT=socket._VERSION,
}
socket.http=http
local PORT=80
local SCHEMES={
 http=true,
}
local function receiveheaders(sock,headers)
 if not headers then
  headers={}
 end
 local line,err=sock:receive()
 if err then
  return nil,err
 end
 while line~="" do
  local name,value=skipsocket(2,find(line,"^(.-):%s*(.*)"))
  if not (name and value) then
   return nil,"malformed reponse headers"
  end
  name=lower(name)
  line,err=sock:receive()
  if err then
   return nil,err
  end
  while find(line,"^%s") do
   value=value..line
   line=sock:receive()
   if err then
    return nil,err
   end
  end
  local found=headers[name]
  if found then
   value=found..", "..value
  end
  headers[name]=value
 end
 return headers
end
socket.sourcet["http-chunked"]=function(sock,headers)
 return setmetatable (
  {
   getfd=function() return sock:getfd() end,
   dirty=function() return sock:dirty() end,
  },{
   __call=function()
    local line,err=sock:receive()
    if err then
     return nil,err
    end
    local size=tonumber(gsub(line,";.*",""),16)
    if not size then
     return nil,"invalid chunk size"
    end
    if size>0 then
     local chunk,err,part=sock:receive(size)
     if chunk then
      sock:receive()
     end
     return chunk,err
    else
     headers,err=receiveheaders(sock,headers)
     if not headers then
      return nil,err
     end
    end
   end
  }
 )
end
socket.sinkt["http-chunked"]=function(sock)
 return setmetatable(
  {
   getfd=function() return sock:getfd() end,
   dirty=function() return sock:dirty() end,
  },
  {
   __call=function(self,chunk,err)
    if not chunk then
     chunk=""
    end
    return sock:send(format("%X\r\n%s\r\n",#chunk,chunk))
   end
 })
end
local methods={}
local mt={ __index=methods }
local function openhttp(host,port,create)
 local c=trysocket((create or tcpsocket)())
 local h=setmetatable({ c=c },mt)
 local try=newtrysocket(function() h:close() end)
 h.try=try
 try(c:settimeout(http.TIMEOUT))
 try(c:connect(host,port or PORT))
 return h
end
http.open=openhttp
function methods.sendrequestline(self,method,uri)
 local requestline=format("%s %s HTTP/1.1\r\n",method or "GET",uri)
 return self.try(self.c:send(requestline))
end
function methods.sendheaders(self,headers)
 self.try(self.c:send(normalizeheaders(headers)))
 return 1
end
function methods.sendbody(self,headers,source,step)
 if not source then
  source=emptysource()
 end
 if not step then
  step=pumpstep
 end
 local mode="http-chunked"
 if headers["content-length"] then
  mode="keep-open"
 end
 return self.try(pumpall(source,sinksocket(mode,self.c),step))
end
function methods.receivestatusline(self)
 local try=self.try
 local status=try(self.c:receive(5))
 if status~="HTTP/" then
  return nil,status 
 end
 status=try(self.c:receive("*l",status))
 local code=skipsocket(2,find(status,"HTTP/%d*%.%d* (%d%d%d)"))
 return try(tonumber(code),status)
end
function methods.receiveheaders(self)
 return self.try(receiveheaders(self.c))
end
function methods.receivebody(self,headers,sink,step)
 if not sink then
  sink=sinknull()
 end
 if not step then
  step=pumpstep
 end
 local length=tonumber(headers["content-length"])
 local encoding=headers["transfer-encoding"] 
 local mode="default" 
 if encoding and encoding~="identity" then
  mode="http-chunked"
 elseif length then
  mode="by-length"
 end
 return self.try(pumpall(sourcesocket(mode,self.c,length),sink,step))
end
function methods.receive09body(self,status,sink,step)
 local source=rewindsource(sourcesocket("until-closed",self.c))
 source(status)
 return self.try(pumpall(source,sink,step))
end
function methods.close(self)
 return self.c:close()
end
local function adjusturi(request)
 if not request.proxy and not http.PROXY then
  request={
     path=trysocket(request.path,"invalid path 'nil'"),
     params=request.params,
     query=request.query,
     fragment=request.fragment,
  }
 end
 return buildurl(request)
end
local function adjustheaders(request)
 local headers={
  ["user-agent"]=http.USERAGENT,
  ["host"]=gsub(request.authority,"^.-@",""),
  ["connection"]="close, TE",
  ["te"]="trailers"
 }
 local username=request.user
 local password=request.password
 if username and password then
  headers["authorization"]="Basic "..(mimeb64(username..":"..unescapeurl(password)))
 end
 local proxy=request.proxy or http.PROXY
 if proxy then
  proxy=parseurl(proxy)
  local username=proxy.user
  local password=proxy.password
  if username and password then
   headers["proxy-authorization"]="Basic "..(mimeb64(username..":"..password))
  end
 end
 local requestheaders=request.headers
 if requestheaders then
  headers=lowerheaders(headers,requestheaders)
 end
 return headers
end
local default={
 host="",
 port=PORT,
 path="/",
 scheme="http"
}
local function adjustrequest(originalrequest)
 local url=originalrequest.url
 local request=url and parseurl(url,default) or {}
 for k,v in next,originalrequest do
  request[k]=v
 end
 local host=request.host
 local port=request.port
 local uri=request.uri
 if not host or host=="" then
  trysocket(nil,"invalid host '"..tostring(host).."'")
 end
 if port=="" then
  request.port=PORT
 end
 if not uri or uri=="" then
  request.uri=adjusturi(request)
 end
 request.headers=adjustheaders(request)
 local proxy=request.proxy or http.PROXY
 if proxy then
  proxy=parseurl(proxy)
  request.host=proxy.host
  request.port=proxy.port or 3128
 end
 return request
end
local maxredericts=4
local validredirects={ [301]=true,[302]=true,[303]=true,[307]=true }
local validmethods={ [false]=true,GET=true,HEAD=true }
local function shouldredirect(request,code,headers)
 local location=headers.location
 if not location then
  return false
 end
 location=gsub(location,"%s","")
 if location=="" then
  return false
 end
 local scheme=match(location,"^([%w][%w%+%-%.]*)%:")
 if scheme and not SCHEMES[scheme] then
  return false
 end
 local method=request.method
 local redirect=request.redirect
 local redirects=request.nredirects or 0
 return redirect and validredirects[code] and validmethods[method] and redirects<=maxredericts
end
local function shouldreceivebody(request,code)
 if request.method=="HEAD" then
  return nil
 end
 if code==204 or code==304 then
  return nil
 end
 if code>=100 and code<200 then
  return nil
 end
 return 1
end
local tredirect,trequest,srequest
tredirect=function(request,location)
 local result,code,headers,status=trequest {
  url=absoluteurl(request.url,location),
  source=request.source,
  sink=request.sink,
  headers=request.headers,
  proxy=request.proxy,
  nredirects=(request.nredirects or 0)+1,
  create=request.create,
 }
 if not headers then
  headers={}
 end
 if not headers.location then
  headers.location=location
 end
 return result,code,headers,status
end
trequest=function(originalrequest)
 local request=adjustrequest(originalrequest)
 local connection=openhttp(request.host,request.port,request.create)
 local headers=request.headers
 connection:sendrequestline(request.method,request.uri)
 connection:sendheaders(headers)
 if request.source then
  connection:sendbody(headers,request.source,request.step)
 end
 local code,status=connection:receivestatusline()
 if not code then
  connection:receive09body(status,request.sink,request.step)
  return 1,200
 end
 while code==100 do
  headers=connection:receiveheaders()
  code,status=connection:receivestatusline()
 end
 headers=connection:receiveheaders()
 if shouldredirect(request,code,headers) and not request.source then
  connection:close()
  return tredirect(originalrequest,headers.location)
 end
 if shouldreceivebody(request,code) then
  connection:receivebody(headers,request.sink,request.step)
 end
 connection:close()
 return 1,code,headers,status
end
local function genericform(url,body)
 local buffer={}
 local request={
  url=url,
  sink=sinktable(buffer),
  target=buffer,
 }
 if body then
  request.source=stringsource(body)
  request.method="POST"
  request.headers={
   ["content-length"]=#body,
   ["content-type"]="application/x-www-form-urlencoded"
  }
 end
 return request
end
http.genericform=genericform
srequest=function(url,body)
 local request=genericform(url,body)
 local _,code,headers,status=trequest(request)
 return concat(request.target),code,headers,status
end
http.request=protectsocket(function(request,body)
 if type(request)=="string" then
  return srequest(request,body)
 else
  return trequest(request)
 end
end)
package.loaded["socket.http"]=http


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-soc-imp-ftp"] = package.loaded["util-soc-imp-ftp"] or true

-- original size: 10357, stripped down to: 8548


local setmetatable,type,next=setmetatable,type,next
local find,format,gsub,match=string.find,string.format,string.gsub,string.match
local concat=table.concat
local mod=math.mod
local socket=socket  or require("socket")
local url=socket.url or require("socket.url")
local tp=socket.tp  or require("socket.tp")
local ltn12=ltn12   or require("ltn12")
local tcpsocket=socket.tcp
local trysocket=socket.try
local skipsocket=socket.skip
local sinksocket=socket.sink
local selectsocket=socket.select
local bindsocket=socket.bind
local newtrysocket=socket.newtry
local sourcesocket=socket.source
local protectsocket=socket.protect
local parseurl=url.parse
local unescapeurl=url.unescape
local pumpall=ltn12.pump.all
local pumpstep=ltn12.pump.step
local sourcestring=ltn12.source.string
local sinktable=ltn12.sink.table
local ftp={
 TIMEOUT=60,
 USER="ftp",
 PASSWORD="anonymous@anonymous.org",
}
socket.ftp=ftp
local PORT=21
local methods={}
local mt={ __index=methods }
function ftp.open(server,port,create)
 local tp=trysocket(tp.connect(server,port or PORT,ftp.TIMEOUT,create))
 local f=setmetatable({ tp=tp },metat)
 f.try=newtrysocket(function() f:close() end)
 return f
end
function methods.portconnect(self)
 local try=self.try
 local server=self.server
 try(server:settimeout(ftp.TIMEOUT))
 self.data=try(server:accept())
 try(self.data:settimeout(ftp.TIMEOUT))
end
function methods.pasvconnect(self)
 local try=self.try
 self.data=try(tcpsocket())
 self(self.data:settimeout(ftp.TIMEOUT))
 self(self.data:connect(self.pasvt.address,self.pasvt.port))
end
function methods.login(self,user,password)
 local try=self.try
 local tp=self.tp
 try(tp:command("user",user or ftp.USER))
 local code,reply=try(tp:check{"2..",331})
 if code==331 then
  try(tp:command("pass",password or ftp.PASSWORD))
  try(tp:check("2.."))
 end
 return 1
end
function methods.pasv(self)
 local try=self.try
 local tp=self.tp
 try(tp:command("pasv"))
 local code,reply=try(self.tp:check("2.."))
 local pattern="(%d+)%D(%d+)%D(%d+)%D(%d+)%D(%d+)%D(%d+)"
 local a,b,c,d,p1,p2=skipsocket(2,find(reply,pattern))
 try(a and b and c and d and p1 and p2,reply)
 local address=format("%d.%d.%d.%d",a,b,c,d)
 local port=p1*256+p2
 local server=self.server
 self.pasvt={
  address=address,
  port=port,
 }
 if server then
  server:close()
  self.server=nil
 end
 return address,port
end
function methods.epsv(self)
 local try=self.try
 local tp=self.tp
 try(tp:command("epsv"))
 local code,reply=try(tp:check("229"))
 local pattern="%((.)(.-)%1(.-)%1(.-)%1%)"
 local d,prt,address,port=match(reply,pattern)
 try(port,"invalid epsv response")
 local address=tp:getpeername()
 local server=self.server
 self.pasvt={
  address=address,
  port=port,
 }
 if self.server then
  server:close()
  self.server=nil
 end
 return address,port
end
function methods.port(self,address,port)
 local try=self.try
 local tp=self.tp
 self.pasvt=nil
 if not address then
  address,port=try(tp:getsockname())
  self.server=try(bindsocket(address,0))
  address,port=try(self.server:getsockname())
  try(self.server:settimeout(ftp.TIMEOUT))
 end
 local pl=mod(port,256)
 local ph=(port-pl)/256
 local arg=gsub(format("%s,%d,%d",address,ph,pl),"%.",",")
 try(tp:command("port",arg))
 try(tp:check("2.."))
 return 1
end
function methods.eprt(self,family,address,port)
 local try=self.try
 local tp=self.tp
 self.pasvt=nil
 if not address then
  address,port=try(tp:getsockname())
  self.server=try(bindsocket(address,0))
  address,port=try(self.server:getsockname())
  try(self.server:settimeout(ftp.TIMEOUT))
 end
 local arg=format("|%s|%s|%d|",family,address,port)
 try(tp:command("eprt",arg))
 try(tp:check("2.."))
 return 1
end
function methods.send(self,sendt)
 local try=self.try
 local tp=self.tp
 try(self.pasvt or self.server,"need port or pasv first")
 if self.pasvt then
  self:pasvconnect()
 end
 local argument=sendt.argument or unescapeurl(gsub(sendt.path or "","^[/\\]",""))
 if argument=="" then
  argument=nil
 end
 local command=sendt.command or "stor"
 try(tp:command(command,argument))
 local code,reply=try(tp:check{"2..","1.."})
 if not self.pasvt then
  self:portconnect()
 end
 local step=sendt.step or pumpstep
 local readt={ tp }
 local checkstep=function(src,snk)
  local readyt=selectsocket(readt,nil,0)
  if readyt[tp] then
   code=try(tp:check("2.."))
  end
  return step(src,snk)
 end
 local sink=sinksocket("close-when-done",self.data)
 try(pumpall(sendt.source,sink,checkstep))
 if find(code,"1..") then
  try(tp:check("2.."))
 end
 self.data:close()
 local sent=skipsocket(1,self.data:getstats())
 self.data=nil
 return sent
end
function methods.receive(self,recvt)
 local try=self.try
 local tp=self.tp
 try(self.pasvt or self.server,"need port or pasv first")
 if self.pasvt then self:pasvconnect() end
 local argument=recvt.argument or unescapeurl(gsub(recvt.path or "","^[/\\]",""))
 if argument=="" then
  argument=nil
 end
 local command=recvt.command or "retr"
 try(tp:command(command,argument))
 local code,reply=try(tp:check{"1..","2.."})
 if code>=200 and code<=299 then
  recvt.sink(reply)
  return 1
 end
 if not self.pasvt then
  self:portconnect()
 end
 local source=sourcesocket("until-closed",self.data)
 local step=recvt.step or pumpstep
 try(pumpall(source,recvt.sink,step))
 if find(code,"1..") then
  try(tp:check("2.."))
 end
 self.data:close()
 self.data=nil
 return 1
end
function methods.cwd(self,dir)
 local try=self.try
 local tp=self.tp
 try(tp:command("cwd",dir))
 try(tp:check(250))
 return 1
end
function methods.type(self,typ)
 local try=self.try
 local tp=self.tp
 try(tp:command("type",typ))
 try(tp:check(200))
 return 1
end
function methods.greet(self)
 local try=self.try
 local tp=self.tp
 local code=try(tp:check{"1..","2.."})
 if find(code,"1..") then
  try(tp:check("2.."))
 end
 return 1
end
function methods.quit(self)
 local try=self.try
 try(self.tp:command("quit"))
 try(self.tp:check("2.."))
 return 1
end
function methods.close(self)
 local data=self.data
 if data then
  data:close()
 end
 local server=self.server
 if server then
  server:close()
 end
 local tp=self.tp
 if tp then
  tp:close()
 end
end
local function override(t)
 if t.url then
  local u=parseurl(t.url)
  for k,v in next,t do
   u[k]=v
  end
  return u
 else
  return t
 end
end
local function tput(putt)
 putt=override(putt)
 local host=putt.host
 trysocket(host,"missing hostname")
 local f=ftp.open(host,putt.port,putt.create)
 f:greet()
 f:login(putt.user,putt.password)
 local typ=putt.type
 if typ then
  f:type(typ)
 end
 f:epsv()
 local sent=f:send(putt)
 f:quit()
 f:close()
 return sent
end
local default={
 path="/",
 scheme="ftp",
}
local function genericform(u)
 local t=trysocket(parseurl(u,default))
 trysocket(t.scheme=="ftp","wrong scheme '"..t.scheme.."'")
 trysocket(t.host,"missing hostname")
 local pat="^type=(.)$"
 if t.params then
  local typ=skipsocket(2,find(t.params,pat))
  t.type=typ
  trysocket(typ=="a" or typ=="i","invalid type '"..typ.."'")
 end
 return t
end
ftp.genericform=genericform
local function sput(u,body)
 local putt=genericform(u)
 putt.source=sourcestring(body)
 return tput(putt)
end
ftp.put=protectsocket(function(putt,body)
 if type(putt)=="string" then
  return sput(putt,body)
 else
  return tput(putt)
 end
end)
local function tget(gett)
 gett=override(gett)
 local host=gett.host
 trysocket(host,"missing hostname")
 local f=ftp.open(host,gett.port,gett.create)
 f:greet()
 f:login(gett.user,gett.password)
 if gett.type then
  f:type(gett.type)
 end
 f:epsv()
 f:receive(gett)
 f:quit()
 return f:close()
end
local function sget(u)
 local gett=genericform(u)
 local t={}
 gett.sink=sinktable(t)
 tget(gett)
 return concat(t)
end
ftp.command=protectsocket(function(cmdt)
 cmdt=override(cmdt)
 local command=cmdt.command
 local argument=cmdt.argument
 local check=cmdt.check
 local host=cmdt.host
 trysocket(host,"missing hostname")
 trysocket(command,"missing command")
 local f=ftp.open(host,cmdt.port,cmdt.create)
 local try=f.try
 local tp=f.tp
 f:greet()
 f:login(cmdt.user,cmdt.password)
 if type(command)=="table" then
  local argument=argument or {}
  for i=1,#command do
   local cmd=command[i]
   try(tp:command(cmd,argument[i]))
   if check and check[i] then
    try(tp:check(check[i]))
   end
  end
 else
  try(tp:command(command,argument))
  if check then
   try(tp:check(check))
  end
 end
 f:quit()
 return f:close()
end)
ftp.get=protectsocket(function(gett)
 if type(gett)=="string" then
  return sget(gett)
 else
  return tget(gett)
 end
end)
package.loaded["socket.ftp"]=ftp


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-soc-imp-smtp"] = package.loaded["util-soc-imp-smtp"] or true

-- original size: 7018, stripped down to: 5883


local type,setmetatable,next=type,setmetatable,next
local find,lower,format=string.find,string.lower,string.format
local osdate,osgetenv=os.date,os.getenv
local random=math.random
local socket=socket   or require("socket")
local headers=socket.headers or require("socket.headers")
local ltn12=ltn12    or require("ltn12")
local tp=socket.tp   or require("socket.tp")
local mime=mime     or require("mime")
local mimeb64=mime.b64
local mimestuff=mime.stuff
local skipsocket=socket.skip
local trysocket=socket.try
local newtrysocket=socket.newtry
local protectsocket=socket.protect
local normalizeheaders=headers.normalize
local lowerheaders=headers.lower
local createcoroutine=coroutine.create
local resumecoroutine=coroutine.resume
local yieldcoroutine=coroutine.resume
local smtp={
 TIMEOUT=60,
 SERVER="localhost",
 PORT=25,
 DOMAIN=osgetenv("SERVER_NAME") or "localhost",
 ZONE="-0000",
}
socket.smtp=smtp
local methods={}
local mt={ __index=methods }
function methods.greet(self,domain)
 local try=self.try
 local tp=self.tp
 try(tp:check("2.."))
 try(tp:command("EHLO",domain or _M.DOMAIN))
 return skipsocket(1,try(tp:check("2..")))
end
function methods.mail(self,from)
 local try=self.try
 local tp=self.tp
 try(tp:command("MAIL","FROM:"..from))
 return try(tp:check("2.."))
end
function methods.rcpt(self,to)
 local try=self.try
 local tp=self.tp
 try(tp:command("RCPT","TO:"..to))
 return try(tp:check("2.."))
end
function methods.data(self,src,step)
 local try=self.try
 local tp=self.tp
 try(tp:command("DATA"))
 try(tp:check("3.."))
 try(tp:source(src,step))
 try(tp:send("\r\n.\r\n"))
 return try(tp:check("2.."))
end
function methods.quit(self)
 local try=self.try
 local tp=self.tp
 try(tp:command("QUIT"))
 return try(tp:check("2.."))
end
function methods.close(self)
 return self.tp:close()
end
function methods.login(self,user,password)
 local try=self.try
 local tp=self.tp
 try(tp:command("AUTH","LOGIN"))
 try(tp:check("3.."))
 try(tp:send(mimeb64(user).."\r\n"))
 try(tp:check("3.."))
 try(tp:send(mimeb64(password).."\r\n"))
 return try(tp:check("2.."))
end
function methods.plain(self,user,password)
 local try=self.try
 local tp=self.tp
 local auth="PLAIN "..mimeb64("\0"..user.."\0"..password)
 try(tp:command("AUTH",auth))
 return try(tp:check("2.."))
end
function methods.auth(self,user,password,ext)
 if not user or not password then
  return 1
 end
 local try=self.try
 if find(ext,"AUTH[^\n]+LOGIN") then
  return self:login(user,password)
 elseif find(ext,"AUTH[^\n]+PLAIN") then
  return self:plain(user,password)
 else
  try(nil,"authentication not supported")
 end
end
function methods.send(self,mail)
 self:mail(mail.from)
 local receipt=mail.rcpt
 if type(receipt)=="table" then
  for i=1,#receipt do
   self:rcpt(receipt[i])
  end
 elseif receipt then
  self:rcpt(receipt)
 end
 self:data(ltn12.source.chain(mail.source,mimestuff()),mail.step)
end
local function opensmtp(self,server,port,create)
 if not server or server=="" then
  server=smtp.SERVER
 end
 if not port or port=="" then
  port=smtp.PORT
 end
 local s={
  tp=trysocket(tp.connect(server,port,smtp.TIMEOUT,create)),
  try=newtrysocket(function()
   s:close()
  end),
 }
 setmetatable(s,mt)
 return s
end
smtp.open=opensmtp
local nofboundaries=0
local function newboundary()
 nofboundaries=nofboundaries+1
 return format('%s%05d==%05u',osdate('%d%m%Y%H%M%S'),random(0,99999),nofboundaries)
end
local send_message
local function send_headers(headers)
 yieldcoroutine(normalizeheaders(headers))
end
local function send_multipart(message)
 local boundary=newboundary()
 local headers=lowerheaders(message.headers)
 local body=message.body
 local preamble=body.preamble
 local epilogue=body.epilogue
 local content=headers['content-type'] or 'multipart/mixed'
 headers['content-type']=content..'; boundary="'..boundary..'"'
 send_headers(headers)
 if preamble then
  yieldcoroutine(preamble)
  yieldcoroutine("\r\n")
 end
 for i=1,#body do
  yieldcoroutine("\r\n--"..boundary.."\r\n")
  send_message(body[i])
 end
 yieldcoroutine("\r\n--"..boundary.."--\r\n\r\n")
 if epilogue then
  yieldcoroutine(epilogue)
  yieldcoroutine("\r\n")
 end
end
local default_content_type='text/plain; charset="UTF-8"'
local function send_source(message)
 local headers=lowerheaders(message.headers)
 if not headers['content-type'] then
  headers['content-type']=default_content_type
 end
 send_headers(headers)
 local getchunk=message.body
 while true do
  local chunk,err=getchunk()
  if err then
   yieldcoroutine(nil,err)
  elseif chunk then
   yieldcoroutine(chunk)
  else
   break
  end
 end
end
local function send_string(message)
 local headers=lowerheaders(message.headers)
 if not headers['content-type'] then
  headers['content-type']=default_content_type
 end
 send_headers(headers)
 yieldcoroutine(message.body)
end
function send_message(message)
 local body=message.body
 if type(body)=="table" then
  send_multipart(message)
 elseif type(body)=="function" then
  send_source(message)
 else
  send_string(message)
 end
end
local function adjust_headers(message)
 local headers=lowerheaders(message.headers)
 if not headers["date"] then
  headers["date"]=osdate("!%a, %d %b %Y %H:%M:%S ")..(message.zone or smtp.ZONE)
 end
 if not headers["x-mailer"] then
  headers["x-mailer"]=socket._VERSION
 end
 headers["mime-version"]="1.0"
 return headers
end
function smtp.message(message)
 message.headers=adjust_headers(message)
 local action=createcoroutine(function()
  send_message(message)
 end)
 return function()
  local ret,a,b=resumecoroutine(action)
  if ret then
   return a,b
  else
   return nil,a
  end
 end
end
smtp.send=protectsocket(function(mail)
 local snd=opensmtp(smtp,mail.server,mail.port,mail.create)
 local ext=snd:greet(mail.domain)
 snd:auth(mail.user,mail.password,ext)
 snd:send(mail)
 snd:quit()
 return snd:close()
end)
package.loaded["socket.smtp"]=smtp


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["trac-set"] = package.loaded["trac-set"] or true

-- original size: 13394, stripped down to: 8882

if not modules then modules={} end modules ['trac-set']={ 
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local type,next,tostring,tonumber=type,next,tostring,tonumber
local print=print
local concat,sortedhash=table.concat,table.sortedhash
local formatters,find,lower,gsub,topattern=string.formatters,string.find,string.lower,string.gsub,string.topattern
local is_boolean=string.is_boolean
local settings_to_hash=utilities.parsers.settings_to_hash
local allocate=utilities.storage.allocate
utilities=utilities or {}
local utilities=utilities
local setters=utilities.setters or {}
utilities.setters=setters
local data={}
local trace_initialize=false 
local frozen=true  
function setters.initialize(filename,name,values) 
 local setter=data[name]
 if setter then
  local data=setter.data
  if data then
   for key,newvalue in sortedhash(values) do
    local newvalue=is_boolean(newvalue,newvalue,true) 
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
  for w,value in sortedhash(what) do
   if value=="" then
    value=newvalue
   elseif not value then
    value=false 
   else
    value=is_boolean(value,value,true) 
   end
   w=topattern(w,true,true)
   for name,functions in sortedhash(data) do
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
  for name,functions in sortedthash(data) do
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
 local list=setters.list(t)
 t.report()
 for k=1,#list do
  local name=list[k]
  local functions=t.data[name]
  if functions then
   local value=functions.value
   local default=functions.default
   local modules=#functions
   if default==nil then
    default="unset"
   elseif type(default)=="table" then
    default=concat(default,"|")
   else
    default=tostring(default)
   end
   if value==nil then
    value="unset"
   elseif type(value)=="table" then
    value=concat(value,"|")
   else
    value=tostring(value)
   end
   t.report(name)
   t.report("    modules : %i",modules)
   t.report("    default : %s",default)
   t.report("    value   : %s",value)
   t.report()
  end
 end
end
local enable,disable,register,list,show=setters.enable,setters.disable,setters.register,setters.list,setters.show
function setters.report(setter,fmt,...)
 print(formatters["%-15s : %s\n"](setter.name,formatters[fmt](...)))
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
  report=function(...) setters.report  (setter,...) end,
  enable=function(...)   enable  (setter,...) end,
  disable=function(...)   disable (setter,...) end,
  reset=function(...)   reset   (setter,...) end,
  register=function(...)   register(setter,...) end,
  list=function(...)  return list (setter,...) end,
  show=function(...)   show (setter,...) end,
  default=function(...)  return default (setter,...) end,
  value=function(...)  return value   (setter,...) end,
 }
 data[name]=setter
 return setter
end
trackers=setters.new("trackers")
directives=setters.new("directives")
experiments=setters.new("experiments")
local t_enable,t_disable=trackers   .enable,trackers   .disable
local d_enable,d_disable=directives .enable,directives .disable
local e_enable,e_disable=experiments.enable,experiments.disable
local trace_directives=false local trace_directives=false  trackers.register("system.directives",function(v) trace_directives=v end)
local trace_experiments=false local trace_experiments=false  trackers.register("system.experiments",function(v) trace_experiments=v end)
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
 directives.register("luatex.expanddepth",function(v) set("expand_depth",v)   end)
 directives.register("luatex.hashextra",function(v) set("hash_extra",v)  end)
 directives.register("luatex.nestsize",function(v) set("nest_size",v)   end)
 directives.register("luatex.maxinopen",function(v) set("max_in_open",v) end)
 directives.register("luatex.maxprintline",function(v) set("max_print_line",v) end)
 directives.register("luatex.maxstrings",function(v) set("max_strings",v) end)
 directives.register("luatex.paramsize",function(v) set("param_size",v)  end)
 directives.register("luatex.savesize",function(v) set("save_size",v)   end)
 directives.register("luatex.stacksize",function(v) set("stack_size",v)  end)
end
local data=table.setmetatableindex("table")
updaters={
 register=function(what,f)
  local d=data[what]
  d[#d+1]=f
 end,
 apply=function(what,...)
  local d=data[what]
  for i=1,#d do
   d[i](...)
  end
 end,
}


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["trac-log"] = package.loaded["trac-log"] or true

-- original size: 33003, stripped down to: 21667

if not modules then modules={} end modules ['trac-log']={
 version=1.001,
 comment="companion to trac-log.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local next,type,select,print=next,type,select,print
local format,gmatch,find=string.format,string.gmatch,string.find
local concat,insert,remove=table.concat,table.insert,table.remove
local topattern=string.topattern
local utfchar=utf.char
local datetime=os.date
local openfile=io.open
local runningtex=tex and (tex.jobname or tex.formatname)
local write_nl=runningtex and texio and texio.write_nl or print
local write=runningtex and texio and texio.write or io.write
local setmetatableindex=table.setmetatableindex
local formatters=string.formatters
local settings_to_hash=utilities.parsers.settings_to_hash
local sortedkeys=table.sortedkeys
local variant="default"
logs=logs or {}
local logs=logs
local moreinfo=[[
More information about ConTeXt and the tools that come with it can be found at:
]].."\n"..[[
maillist : ntg-context@ntg.nl / http://www.ntg.nl/mailman/listinfo/ntg-context
webpage  : http://www.pragma-ade.nl / http://tex.aanhet.net
wiki     : http://contextgarden.net
]]
formatters.add (
 formatters,"unichr",
 [["U+" .. format("%%05X",%s) .. " (" .. utfchar(%s) .. ")"]]
)
formatters.add (
 formatters,"chruni",
 [[utfchar(%s) .. " (U+" .. format("%%05X",%s) .. ")"]]
)
local function ignore() end
setmetatableindex(logs,function(t,k) t[k]=ignore;return ignore end)
local report,subreport,status,settarget,setformats,settranslations
local direct,subdirect,writer,pushtarget,poptarget,setlogfile,settimedlog,setprocessor,setformatters,newline
local function ansisupported(specification)
 if specification~="ansi" and specification~="ansilog" then
  return false
 elseif os and os.enableansi then
  return os.enableansi()
 else
  return false
 end
end
if runningtex and texio then
 if texio.setescape then
  texio.setescape(0) 
 end
 if arg and ansisupported then
  for k,v in next,arg do 
   if v=="--ansi" or v=="--c:ansi" then
    if ansisupported("ansi") then
     variant="ansi"
    end
    break
   elseif v=="--ansilog" or v=="--c:ansilog" then
    if ansisupported("ansilog") then
     variant="ansilog"
    end
    break
   end
  end
 end
 local function useluawrites()
  local texio_write_nl=texio.write_nl
  local texio_write=texio.write
  local io_write=io.write
  write_nl=function(target,...)
   if not io_write then
    io_write=io.write
   end
   if target=="term and log" then
    texio_write_nl("log",...)
    texio_write_nl("term","")
    io_write(...)
   elseif target=="log" then
    texio_write_nl("log",...)
   elseif target=="term" then
    texio_write_nl("term","")
    io_write(...)
   elseif type(target)=="number" then
    texio_write_nl(target,...) 
   elseif target~="none" then
    texio_write_nl("log",target,...)
    texio_write_nl("term","")
    io_write(target,...)
   end
  end
  write=function(target,...)
   if not io_write then
    io_write=io.write
   end
   if target=="term and log" then
    texio_write("log",...)
    io_write(...)
   elseif target=="log" then
    texio_write("log",...)
   elseif target=="term" then
    io_write(...)
   elseif type(target)=="number" then
    texio_write(target,...) 
   elseif target~="none" then
    texio_write("log",target,...)
    io_write(target,...)
   end
  end
  texio.write=write
  texio.write_nl=write_nl
  useluawrites=ignore
 end
 local whereto="both"
 local target=nil
 local targets=nil
 local formats=table.setmetatableindex("self")
 local translations=table.setmetatableindex("self")
 local report_yes,subreport_yes,direct_yes,subdirect_yes,status_yes
 local report_nop,subreport_nop,direct_nop,subdirect_nop,status_nop
 local variants={
  default={
   formats={
    report_yes=formatters["%-15s > %s\n"],
    report_nop=formatters["%-15s >\n"],
    direct_yes=formatters["%-15s > %s"],
    direct_nop=formatters["%-15s >"],
    subreport_yes=formatters["%-15s > %s > %s\n"],
    subreport_nop=formatters["%-15s > %s >\n"],
    subdirect_yes=formatters["%-15s > %s > %s"],
    subdirect_nop=formatters["%-15s > %s >"],
    status_yes=formatters["%-15s : %s\n"],
    status_nop=formatters["%-15s :\n"],
   },
   targets={
    logfile="log",
    log="log",
    file="log",
    console="term",
    terminal="term",
    both="term and log",
   },
  },
  ansi={
   formats={
    report_yes=formatters["[0;33m%-15s [0;1m>[0m %s\n"],
    report_nop=formatters["[0;33m%-15s [0;1m>[0m\n"],
    direct_yes=formatters["[0;33m%-15s [0;1m>[0m %s"],
    direct_nop=formatters["[0;33m%-15s [0;1m>[0m"],
    subreport_yes=formatters["[0;33m%-15s [0;1m>[0;35m %s [0;1m>[0m %s\n"],
    subreport_nop=formatters["[0;33m%-15s [0;1m>[0;35m %s [0;1m>[0m\n"],
    subdirect_yes=formatters["[0;33m%-15s [0;1m>[0;35m %s [0;1m>[0m %s"],
    subdirect_nop=formatters["[0;33m%-15s [0;1m>[0;35m %s [0;1m>[0m"],
    status_yes=formatters["[0;33m%-15s [0;1m:[0m %s\n"],
    status_nop=formatters["[0;33m%-15s [0;1m:[0m\n"],
   },
   targets={
    logfile="none",
    log="none",
    file="none",
    console="term",
    terminal="term",
    both="term",
   },
  }
 }
 variants.ansilog={
  formats=variants.ansi.formats,
  targets=variants.default.targets,
 }
 logs.flush=io.flush
 writer=function(...)
  write_nl(target,...)
 end
 newline=function()
  write_nl(target,"\n")
 end
 report=function(a,b,c,...)
  if c~=nil then
   write_nl(target,report_yes(translations[a],formatters[formats[b]](c,...)))
  elseif b then
   write_nl(target,report_yes(translations[a],formats[b]))
  elseif a then
   write_nl(target,report_nop(translations[a]))
  else
   write_nl(target,"\n")
  end
 end
 direct=function(a,b,c,...)
  if c~=nil then
   return direct_yes(translations[a],formatters[formats[b]](c,...))
  elseif b then
   return direct_yes(translations[a],formats[b])
  elseif a then
   return direct_nop(translations[a])
  else
   return ""
  end
 end
 subreport=function(a,s,b,c,...)
  if c~=nil then
   write_nl(target,subreport_yes(translations[a],translations[s],formatters[formats[b]](c,...)))
  elseif b then
   write_nl(target,subreport_yes(translations[a],translations[s],formats[b]))
  elseif a then
   write_nl(target,subreport_nop(translations[a],translations[s]))
  else
   write_nl(target,"\n")
  end
 end
 subdirect=function(a,s,b,c,...)
  if c~=nil then
   return subdirect_yes(translations[a],translations[s],formatters[formats[b]](c,...))
  elseif b then
   return subdirect_yes(translations[a],translations[s],formats[b])
  elseif a then
   return subdirect_nop(translations[a],translations[s])
  else
   return ""
  end
 end
 status=function(a,b,c,...)
  if c~=nil then
   write_nl(target,status_yes(translations[a],formatters[formats[b]](c,...)))
  elseif b then
   write_nl(target,status_yes(translations[a],formats[b]))
  elseif a then
   write_nl(target,status_nop(translations[a]))
  else
   write_nl(target,"\n")
  end
 end
 settarget=function(askedwhereto)
  whereto=askedwhereto or whereto or "both"
  target=targets[whereto]
  if not target then
   whereto="both"
   target=targets[whereto]
  end
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
 setprocessor=function(f)
  local writeline=write_nl
  write_nl=function(target,...)
   writeline(target,f(...))
  end
 end
 setformatters=function(specification)
  local t=nil
  local f=nil
  local d=variants.default
  if not specification then
  elseif type(specification)=="table" then
   t=specification.targets
   f=specification.formats or specification
  else
   if not ansisupported(specification) then
    specification="default"
   end
   local v=variants[specification]
   if v then
    t=v.targets
    f=v.formats
    variant=specification
   end
  end
  targets=t or d.targets
  target=targets[whereto] or target
  if f then
   d=d.formats
  else
   f=d.formats
   d=f
  end
  setmetatableindex(f,d)
  report_yes=f.report_yes
  report_nop=f.report_nop
  subreport_yes=f.subreport_yes
  subreport_nop=f.subreport_nop
  direct_yes=f.direct_yes
  direct_nop=f.direct_nop
  subdirect_yes=f.subdirect_yes
  subdirect_nop=f.subdirect_nop
  status_yes=f.status_yes
  status_nop=f.status_nop
  if variant=="ansi" or variant=="ansilog" then
   useluawrites() 
  end
  settarget(whereto)
 end
 setformatters(variant)
 setlogfile=ignore
 settimedlog=ignore
else
 local report_yes,subreport_yes,status_yes
 local report_nop,subreport_nop,status_nop
 local variants={
  default={
   formats={
    report_yes=formatters["%-15s | %s"],
    report_nop=formatters["%-15s |"],
    subreport_yes=formatters["%-15s | %s | %s"],
    subreport_nop=formatters["%-15s | %s |"],
    status_yes=formatters["%-15s : %s\n"],
    status_nop=formatters["%-15s :\n"],
   },
  },
  ansi={
   formats={
    report_yes=formatters["[0;32m%-15s [0;1m|[0m %s"],
    report_nop=formatters["[0;32m%-15s [0;1m|[0m"],
    subreport_yes=formatters["[0;32m%-15s [0;1m|[0;31m %s [0;1m|[0m %s"],
    subreport_nop=formatters["[0;32m%-15s [0;1m|[0;31m %s [0;1m|[0m"],
    status_yes=formatters["[0;32m%-15s [0;1m:[0m %s\n"],
    status_nop=formatters["[0;32m%-15s [0;1m:[0m\n"],
   },
  },
 }
 logs.flush=ignore
 writer=function(s)
  write_nl(s)
 end
 newline=function()
  write_nl("\n")
 end
 report=function(a,b,c,...)
  if c then
   write_nl(report_yes(a,formatters[b](c,...)))
  elseif b then
   write_nl(report_yes(a,b))
  elseif a then
   write_nl(report_nop(a))
  else
   write_nl("")
  end
 end
 subreport=function(a,sub,b,c,...)
  if c then
   write_nl(subreport_yes(a,sub,formatters[b](c,...)))
  elseif b then
   write_nl(subreport_yes(a,sub,b))
  elseif a then
   write_nl(subreport_nop(a,sub))
  else
   write_nl("")
  end
 end
 status=function(a,b,c,...) 
  if c then
   write_nl(status_yes(a,formatters[b](c,...)))
  elseif b then
   write_nl(status_yes(a,b)) 
  elseif a then
   write_nl(status_nop(a))
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
 setprocessor=function(f)
  local writeline=write_nl
  write_nl=function(s)
   writeline(f(s))
  end
 end
 setformatters=function(specification)
  local f=nil
  local d=variants.default
  if specification then
   if type(specification)=="table" then
    f=specification.formats or specification
   else
    if not ansisupported(specification) then
     specification="default"
    end
    local v=variants[specification]
    if v then
     f=v.formats
    end
   end
  end
  if f then
   d=d.formats
  else
   f=d.formats
   d=f
  end
  setmetatableindex(f,d)
  report_yes=f.report_yes
  report_nop=f.report_nop
  subreport_yes=f.subreport_yes
  subreport_nop=f.subreport_nop
  status_yes=f.status_yes
  status_nop=f.status_nop
 end
 setformatters(variant)
 setlogfile=function(name,keepopen)
  if name and name~="" then
   local localtime=os.localtime
   local writeline=write_nl
   if keepopen then
    local f=io.open(name,"ab")
    write_nl=function(s)
     writeline(s)
     f:write(localtime()," | ",s,"\n")
    end
   else
    write_nl=function(s)
     writeline(s)
     local f=io.open(name,"ab")
     f:write(localtime()," | ",s,"\n")
     f:close()
    end
   end
  end
  setlogfile=ignore
 end
 settimedlog=function()
  local localtime=os.localtime
  local writeline=write_nl
  write_nl=function(s)
   writeline(localtime().." | "..s)
  end
  settimedlog=ignore
 end
end
logs.report=report
logs.subreport=subreport
logs.status=status
logs.settarget=settarget
logs.pushtarget=pushtarget
logs.poptarget=poptarget
logs.setformats=setformats
logs.settranslations=settranslations
logs.setlogfile=setlogfile
logs.settimedlog=settimedlog
logs.setprocessor=setprocessor
logs.setformatters=setformatters
logs.direct=direct
logs.subdirect=subdirect
logs.writer=writer
logs.newline=newline
local data={}
local states=nil
local force=false
function logs.reporter(category,subcategory)
 local logger=data[category]
 if not logger then
  local state=states==true
  if not state and type(states)=="table" then
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
    if force or not logger.state then
     subreport(category,subcategory,...)
    end
   end
   logger.reporters[subcategory]=reporter
  else
   local tag=category
   reporter=function(...)
    if force or not logger.state then
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
 if category==true or category=="all" then
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
  alllocked=false
  states=settings_to_hash(category,type(states)=="table" and states or nil)
  for c in next,states do
   local v=data[c]
   if v then
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
 return sortedkeys(data)
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
  local subcategories=concat(sortedkeys(reporters),", ")
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
if tex then
 local report=logs.reporter("pages") 
 local texgetcount=tex and tex.getcount
 local real,user,sub=0,0,0
 function logs.start_page_number()
  real=texgetcount("realpageno")
  user=texgetcount("userpageno")
  sub=texgetcount("subpageno")
 end
 local timing=false
 local lasttime=nil
 trackers.register("pages.timing",function(v) 
  timing=""
 end)
 function logs.stop_page_number() 
  if timing then
   local elapsed=statistics.currenttime(statistics)
   local average,page
   if not lasttime or real<2 then
    average=elapsed
    page=elapsed
   else
    average=elapsed/(real-1)
    page=elapsed-lasttime
   end
   lasttime=elapsed
   timing=formatters[", total %0.03f, page %0.03f, average %0.03f"](elapsed,page,average)
  end
  if real<=0 then
   report("flushing page%s",timing)
  elseif user<=0 then
   report("flushing realpage %s%s",real,timing)
  elseif sub<=0 then
   report("flushing realpage %s, userpage %s%s",real,user,timing)
  else
   report("flushing realpage %s, userpage %s, subpage %s%s",real,user,sub,timing)
  end
  logs.flush()
 end
end
local nesting=0
local verbose=false
local hasscheme=url.hasscheme
local simple=logs.reporter("comment")
logs.simple=simple
logs.simpleline=simple
logs.setprogram=ignore 
logs.extendbanner=ignore 
logs.reportlines=ignore 
logs.reportbanner=ignore 
logs.reportline=ignore 
logs.simplelines=ignore 
logs.help=ignore
local Carg,C,lpegmatch=lpeg.Carg,lpeg.C,lpeg.match
local p_newline=lpeg.patterns.newline
local linewise=(
 Carg(1)*C((1-p_newline)^1)/function(t,s) t.report(s) end+Carg(1)*p_newline^2/function(t)   t.report()  end+p_newline
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
 local arguments=environment and environment.arguments
 if arguments then
  local ansi=arguments.ansi or arguments.ansilog
  if ansi then
   logs.setformatters(arguments.ansi and "ansi" or "ansilog")
  end
 end
 t.name=t.name   or "unknown"
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
local f_syslog=formatters["%s %s => %s => %s => %s\r"]
function logs.system(whereto,process,jobname,category,fmt,arg,...)
 local message=f_syslog(datetime("%d/%m/%y %H:%m:%S"),process,jobname,category,arg==nil and fmt or format(fmt,arg,...))
 for i=1,10 do
  local f=openfile(whereto,"a") 
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
  tex.error(format(...))
 end
else
 function logs.texerrormessage(...)
  print(format(...))
 end
end
if package.helpers.report then
 package.helpers.report=logs.reporter("package loader") 
end
if tex then
 local finalactions={}
 local fatalerrors={}
 local possiblefatal={}
 local loggingerrors=false
 function logs.loggingerrors()
  return loggingerrors
 end
 directives.register("logs.errors",function(v)
  loggingerrors=v
  if type(v)=="string" then
   fatalerrors=settings_to_hash(v)
  else
   fatalerrors={}
  end
 end)
 function logs.registerfinalactions(...)
  insert(finalactions,...) 
 end
 local what=nil
 local report=nil
 local state=nil
 local target=nil
 local function startlogging(t,r,w,s)
  target=t
  state=force
  force=true
  report=type(r)=="function" and r or logs.reporter(r)
  what=w
  pushtarget(target)
  newline()
  if s then
   report("start %s: %s",what,s)
  else
   report("start %s",what)
  end
  if target=="logfile" then
   newline()
  end
  return report
 end
 local function stoplogging()
  if target=="logfile" then
   newline()
  end
  report("stop %s",what)
  if target=="logfile" then
   newline()
  end
  poptarget()
  state=oldstate
 end
 function logs.startfilelogging(...)
  return startlogging("logfile",...)
 end
 logs.stopfilelogging=stoplogging
 local done=false
 function logs.starterrorlogging(r,w,...)
  if not done then
   pushtarget("terminal")
   newline()
   logs.report("error logging","start possible issues")
   poptarget()
   done=true
  end
  if fatalerrors[w] then
   possiblefatal[w]=true
  end
  return startlogging("terminal",r,w,...)
 end
 logs.stoperrorlogging=stoplogging
 function logs.finalactions()
  if #finalactions>0 then
   for i=1,#finalactions do
    finalactions[i]()
   end
   if done then
    pushtarget("terminal")
    newline()
    logs.report("error logging","stop possible issues")
    poptarget()
   end
   return next(possiblefatal) and sortedkeys(possiblefatal) or false
  end
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["trac-inf"] = package.loaded["trac-inf"] or true

-- original size: 10184, stripped down to: 7500

if not modules then modules={} end modules ['trac-inf']={
 version=1.001,
 comment="companion to trac-inf.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local type,tonumber,select=type,tonumber,select
local format,lower,find=string.format,string.lower,string.find
local concat=table.concat
local clock=os.gettimeofday or os.clock 
local setmetatableindex=table.setmetatableindex
local serialize=table.serialize
local formatters=string.formatters
statistics=statistics or {}
local statistics=statistics
statistics.enable=true
statistics.threshold=0.01
local statusinfo,n,registered,timers={},0,{},{}
setmetatableindex(timers,function(t,k)
 local v={ timing=0,loadtime=0,offset=0 }
 t[k]=v
 return v
end)
local function hastiming(instance)
 return instance and timers[instance]
end
local function resettiming(instance)
 timers[instance or "notimer"]={ timing=0,loadtime=0,offset=0 }
end
local ticks=clock
local seconds=function(n) return n or 0 end
if os.type~="windows" then
elseif lua.getpreciseticks then
 ticks=lua.getpreciseticks
 seconds=lua.getpreciseseconds
elseif FFISUPPORTED then
 local okay,kernel=pcall(ffi.load,"kernel32")
 if kernel then
  local tonumber=ffi.number or tonumber
  ffi.cdef[[
            int QueryPerformanceFrequency(int64_t *lpFrequency);
            int QueryPerformanceCounter(int64_t *lpPerformanceCount);
        ]]
  local target=ffi.new("__int64[1]")
  ticks=function()
   if kernel.QueryPerformanceCounter(target)==1 then
    return tonumber(target[0])
   else
    return 0
   end
  end
  local target=ffi.new("__int64[1]")
  seconds=function(ticks)
   if kernel.QueryPerformanceFrequency(target)==1 then
    return ticks/tonumber(target[0])
   else
    return 0
   end
  end
 end
else
end
local function starttiming(instance,reset)
 local timer=timers[instance or "notimer"]
 local it=timer.timing
 if reset then
  it=0
  timer.loadtime=0
 end
 if it==0 then
  timer.starttime=ticks()
  if not timer.loadtime then
   timer.loadtime=0
  end
 end
 timer.timing=it+1
end
local function stoptiming(instance)
 local timer=timers[instance or "notimer"]
 local it=timer.timing
 if it>1 then
  timer.timing=it-1
 else
  local starttime=timer.starttime
  if starttime and starttime>0 then
   local stoptime=ticks()
   local loadtime=stoptime-starttime
   timer.stoptime=stoptime
   timer.loadtime=timer.loadtime+loadtime
   timer.timing=0
   timer.starttime=0
   return loadtime
  end
 end
 return 0
end
local function benchmarktimer(instance)
 local timer=timers[instance or "notimer"]
 local it=timer.timing
 if it>1 then
  timer.timing=it-1
 else
  local starttime=timer.starttime
  if starttime and starttime>0 then
   timer.offset=ticks()-starttime
  else
   timer.offset=0
  end
 end
end
local function elapsed(instance)
 if type(instance)=="number" then
  return instance
 else
  local timer=timers[instance or "notimer"]
  return timer and seconds(timer.loadtime-2*(timer.offset or 0)) or 0
 end
end
local function currenttime(instance)
 if type(instance)=="number" then
  return instance
 else
  local timer=timers[instance or "notimer"]
  local it=timer.timing
  if it>1 then
  else
   local starttime=timer.starttime
   if starttime and starttime>0 then
    return seconds(timer.loadtime+ticks()-starttime-2*(timer.offset or 0))
   end
  end
  return 0
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
statistics.currenttime=currenttime
statistics.elapsed=elapsed
statistics.elapsedtime=elapsedtime
statistics.elapsedindeed=elapsedindeed
statistics.elapsedseconds=elapsedseconds
statistics.benchmarktimer=benchmarktimer
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
  register("used platform",function()
   return format("%s, type: %s, binary subtree: %s",
    os.platform or "unknown",os.type or "unknown",environment.texos or "unknown")
  end)
  if LUATEXENGINE=="luametatex" then
   register("used engine",function()
    return format("%s version: %s, functionality level: %s, format id: %s, compiler: %s",
     LUATEXENGINE,LUATEXVERSION,LUATEXFUNCTIONALITY,LUATEXFORMATID,status.used_compiler)
   end)
  else
   register("used engine",function()
    return format("%s version: %s, functionality level: %s, banner: %s",
     LUATEXENGINE,LUATEXVERSION,LUATEXFUNCTIONALITY,lower(status.banner))
   end)
  end
  register("control sequences",function()
   return format("%s of %s + %s",status.cs_count,status.hash_size,status.hash_extra)
  end)
  register("callbacks",statistics.callbacks)
  if JITSUPPORTED then
   local jitstatus=jit.status
   if jitstatus then
    local jitstatus={ jitstatus() }
    if jitstatus[1] then
     register("luajit options",concat(jitstatus," ",2))
    end
   end
  end
  register("lua properties",function()
   local hashchar=tonumber(status.luatex_hashchars)
   local mask=lua.mask or "ascii"
   return format("engine: %s %s, used memory: %s, hash chars: min(%i,40), symbol mask: %s (%s)",
    jit and "luajit" or "lua",
    LUAVERSION,
    statistics.memused(),
    hashchar and 2^hashchar or "unknown",
    mask,
    mask=="utf" and "τεχ" or "tex")
  end)
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
 return format("%s MB, ctx: %s MB, max: %s MB",
  round(collectgarbage("count")/1000),
  round(status.luastate_bytes/1000000),
  status.luastate_bytes_max and round(status.luastate_bytes_max/1000000) or "unknown"
 )
end
starttiming(statistics)
function statistics.formatruntime(runtime) 
 return format("%s seconds",runtime)   
end
function statistics.runtime()
 stoptiming(statistics)
 local runtime=lua.getruntime and lua.getruntime() or elapsedtime(statistics)
 return statistics.formatruntime(runtime)
end
local report=logs.reporter("system")
function statistics.timed(action,all)
 starttiming("run")
 action()
 stoptiming("run")
 local runtime=tonumber(elapsedtime("run"))
 if all then
  local alltime=tonumber(lua.getruntime and lua.getruntime() or elapsedtime(statistics))
  if alltime and alltime>0 then
   report("total runtime: %0.3f seconds of %0.3f seconds",runtime,alltime)
   return
  end
 end
 report("total runtime: %0.3f seconds",runtime)
end
function statistics.tracefunction(base,tag,...)
 for i=1,select("#",...) do
  local name=select(i,...)
  local stat={}
  local func=base[name]
  setmetatableindex(stat,function(t,k) t[k]=0 return 0 end)
  base[name]=function(n,k,v) stat[k]=stat[k]+1 return func(n,k,v) end
  statistics.register(formatters["%s.%s"](tag,name),function() return serialize(stat,"calls") end)
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["trac-pro"] = package.loaded["trac-pro"] or true

-- original size: 5841, stripped down to: 3352

if not modules then modules={} end modules ['trac-pro']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local getmetatable,setmetatable,rawset,type,next=getmetatable,setmetatable,rawset,type,next
local trace_namespaces=false  trackers.register("system.namespaces",function(v) trace_namespaces=v end)
local report_system=logs.reporter("system","protection")
namespaces=namespaces or {}
local namespaces=namespaces
local registered={}
local function report_index(k,name)
 if trace_namespaces then
  report_system("reference to %a in protected namespace %a: %s",k,name)
  debugger.showtraceback(report_system)
 else
  report_system("reference to %a in protected namespace %a",k,name)
 end
end
local function report_newindex(k,name)
 if trace_namespaces then
  report_system("assignment to %a in protected namespace %a: %s",k,name)
  debugger.showtraceback(report_system)
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

-- original size: 7149, stripped down to: 4997

if not modules then modules={} end modules ['util-lua']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 comment="the strip code is written by Peter Cawley",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local rep,sub,byte,dump,format=string.rep,string.sub,string.byte,string.dump,string.format
local load,loadfile,type,collectgarbage=load,loadfile,type,collectgarbage
utilities=utilities or {}
utilities.lua=utilities.lua or {}
local luautilities=utilities.lua
local report_lua=logs.reporter("system","lua")
local report_mem=logs.reporter("system","lua memory")
local tracestripping=false
local tracememory=false
luautilities.stripcode=true  
luautilities.alwaysstripcode=false 
luautilities.nofstrippedchunks=0
luautilities.nofstrippedbytes=0
local strippedchunks={} 
luautilities.strippedchunks=strippedchunks
if not LUATEXENGINE then
 LUATEXENGINE=status.luatex_engine and string.lower(status.luatex_engine)
 JITSUPPORTED=LUATEXENGINE=="luajittex" or jit
 CONTEXTLMTXMODE=CONTEXTLMTXMODE or (LUATEXENGINE=="luametatex" and 1) or 0
end
luautilities.suffixes={
 tma="tma",
 tmc=(CONTEXTLMTXMODE and CONTEXTLMTXMODE>0 and "tmd") or (jit and "tmb") or "tmc",
 lua="lua",
 luc=(CONTEXTLMTXMODE and CONTEXTLMTXMODE>0 and "lud") or (jit and "lub") or "luc",
 lui="lui",
 luv="luv",
 luj="luj",
 tua="tua",
 tuc=(CONTEXTLMTXMODE and CONTEXTLMTXMODE>0 and "tud") or (jit and "tub") or "tuc",
}
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
function luautilities.loadedluacode(fullname,forcestrip,name,macros)
 name=name or fullname
 if macros then
  macros=lua.macros
 end
 local code,message
 if macros then
  code,message=macros.loaded(fullname,true,false)
 else
  code,message=loadfile(fullname)
 end
 if code then
  code()
 else
  report_lua("loading of file %a failed:\n\t%s",fullname,message or "no message")
  code,message=loadfile(fullname)
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
function luautilities.strippedloadstring(code,name,forcestrip) 
 local code,message=load(code)
 if not code then
  report_lua("loading of file %a failed:\n\t%s",name,message or "no message")
 end
 if forcestrip and luautilities.stripcode or luautilities.alwaysstripcode then
  register(name)
  return load(dump(code,true)),0 
 else
  return code,0
 end
end
function luautilities.loadstring(code,name) 
 local code,message=load(code)
 if not code then
  report_lua("loading of file %a failed:\n\t%s",name,message or "no message")
 end
 return code,0
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
local finalizers={}
setmetatable(finalizers,{
 __gc=function(t)
  for i=1,#t do
   pcall(t[i]) 
  end
 end
} )
function luautilities.registerfinalizer(f)
 finalizers[#finalizers+1]=f
end
function luautilities.checkmemory(previous,threshold,trace) 
 local current=collectgarbage("count")
 if previous then
  local checked=(threshold or 64)*1024
  local delta=current-previous
  if current-previous>checked then
   collectgarbage("collect")
   local afterwards=collectgarbage("count")
   if trace or tracememory then
    report_mem("previous %r MB, current %r MB, delta %r MB, threshold %r MB, afterwards %r MB",
     previous/1024,current/1024,delta/1024,threshold,afterwards)
   end
   return afterwards
  elseif trace or tracememory then
   report_mem("previous %r MB, current %r MB, delta %r MB, threshold %r MB",
    previous/1024,current/1024,delta/1024,threshold)
  end
 end
 return current
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-deb"] = package.loaded["util-deb"] or true

-- original size: 10136, stripped down to: 6832

if not modules then modules={} end modules ['util-deb']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local type,next,tostring,tonumber=type,next,tostring,tonumber
local format,find,sub,gsub=string.format,string.find,string.sub,string.gsub
local insert,remove,sort=table.insert,table.remove,table.sort
local setmetatableindex=table.setmetatableindex
utilities=utilities or {}
local debugger=utilities.debugger or {}
utilities.debugger=debugger
local report=logs.reporter("debugger")
local ticks=os.gettimeofday or os.clock
local seconds=function(n) return n or 0 end
local overhead=0
local dummycalls=10*1000
local nesting=0
local names={}
local initialize=false
if lua.getpreciseticks then
 initialize=function()
  ticks=lua.getpreciseticks
  seconds=lua.getpreciseseconds
  initialize=false
 end
elseif not (FFISUPPORTED and ffi) then
elseif os.type=="windows" then
 initialize=function()
  local kernel=ffilib("kernel32","system") 
  if kernel then
   local tonumber=ffi.number or tonumber
   ffi.cdef[[
                int QueryPerformanceFrequency(int64_t *lpFrequency);
                int QueryPerformanceCounter(int64_t *lpPerformanceCount);
            ]]
   local target=ffi.new("__int64[1]")
   ticks=function()
    if kernel.QueryPerformanceCounter(target)==1 then
     return tonumber(target[0])
    else
     return 0
    end
   end
   local target=ffi.new("__int64[1]")
   seconds=function(ticks)
    if kernel.QueryPerformanceFrequency(target)==1 then
     return ticks/tonumber(target[0])
    else
     return 0
    end
   end
  end
  initialize=false
 end
elseif os.type=="unix" then
 initialize=function()
  local C=ffi.C
  local tonumber=ffi.number or tonumber
  ffi.cdef [[
            /* what a mess */
            typedef int clk_id_t;
            typedef enum { CLOCK_REALTIME, CLOCK_MONOTONIC, CLOCK_PROCESS_CPUTIME_ID } clk_id;
            typedef struct timespec { long sec; long nsec; } ctx_timespec;
            int clock_gettime(clk_id_t timerid, struct timespec *t);
        ]]
  local target=ffi.new("ctx_timespec[?]",1)
  local clock=C.CLOCK_PROCESS_CPUTIME_ID
  ticks=function ()
   C.clock_gettime(clock,target)
   return tonumber(target[0].sec*1000000000+target[0].nsec)
  end
  seconds=function(ticks)
   return ticks/1000000000
  end
  initialize=false
 end
end
setmetatableindex(names,function(t,name)
 local v=setmetatableindex(function(t,source)
  local v=setmetatableindex(function(t,line)
   local v={ total=0,count=0,nesting=0 }
   t[line]=v
   return v
  end)
  t[source]=v
  return v
 end)
 t[name]=v
 return v
end)
local getinfo=nil
local sethook=nil
local function hook(where)
 local f=getinfo(2,"nSl")
 if f then
  local source=f.short_src
  if not source then
   return
  end
  local line=f.linedefined or 0
  local name=f.name
  if not name then
   local what=f.what
   if what=="C" then
    name="<anonymous>"
   else
    name=f.namewhat or what or "<unknown>"
   end
  end
  local data=names[name][source][line]
  if where=="call" then
   local nesting=data.nesting
   if nesting==0 then
    data.count=data.count+1
    insert(data,ticks())
    data.nesting=1
   else
    data.nesting=nesting+1
   end
  elseif where=="return" then
   local nesting=data.nesting
   if nesting==1 then
    local t=remove(data)
    if t then
     data.total=data.total+ticks()-t
    end
    data.nesting=0
   else
    data.nesting=nesting-1
   end
  end
 end
end
function debugger.showstats(printer,threshold)
 local printer=printer or report
 local calls=0
 local functions=0
 local dataset={}
 local length=0
 local realtime=0
 local totaltime=0
 local threshold=threshold or 0
 for name,sources in next,names do
  for source,lines in next,sources do
   for line,data in next,lines do
    local count=data.count
    if count>threshold then
     if #name>length then
      length=#name
     end
     local total=data.total
     local real=total
     if real>0 then
      real=total-(count*overhead/dummycalls)
      if real<0 then
       real=0
      end
      realtime=realtime+real
     end
     totaltime=totaltime+total
     if line<0 then
      line=0
     end
     dataset[#dataset+1]={ real,total,count,name,source,line }
    end
   end
  end
 end
 sort(dataset,function(a,b)
  if a[1]==b[1] then
   if a[2]==b[2] then
    if a[3]==b[3] then
     if a[4]==b[4] then
      if a[5]==b[5] then
       return a[6]<b[6]
      else
       return a[5]<b[5]
      end
     else
      return a[4]<b[4]
     end
    else
     return b[3]<a[3]
    end
   else
    return b[2]<a[2]
   end
  else
   return b[1]<a[1]
  end
 end)
 if length>50 then
  length=50
 end
 local fmt=string.formatters["%4.9k s  %3.3k %%  %4.9k s  %3.3k %%  %8i #  %-"..length.."s  %4i  %s"]
 for i=1,#dataset do
  local data=dataset[i]
  local real=data[1]
  local total=data[2]
  local count=data[3]
  local name=data[4]
  local source=data[5]
  local line=data[6]
  calls=calls+count
  functions=functions+1
  name=gsub(name,"%s+"," ")
  if #name>length then
   name=sub(name,1,length)
  end
  printer(fmt(seconds(total),100*total/totaltime,seconds(real),100*real/realtime,count,name,line,source))
 end
 printer("")
 printer(format("functions : %i",functions))
 printer(format("calls     : %i",calls))
 printer(format("overhead  : %f",seconds(overhead/1000)))
end
local function getdebug()
 if sethook and getinfo then
  return
 end
 if not debug then
  local okay
  okay,debug=pcall(require,"debug")
 end
 if type(debug)~="table" then
  return
 end
 getinfo=debug.getinfo
 sethook=debug.sethook
 if type(getinfo)~="function" then
  getinfo=nil
 end
 if type(sethook)~="function" then
  sethook=nil
 end
end
function debugger.savestats(filename,threshold)
 local f=io.open(filename,'w')
 if f then
  debugger.showstats(function(str) f:write(str,"\n") end,threshold)
  f:close()
 end
end
function debugger.enable()
 getdebug()
 if sethook and getinfo and nesting==0 then
  running=true
  if initialize then
   initialize()
  end
  sethook(hook,"cr")
  local function dummy() end
  local t=ticks()
  for i=1,dummycalls do
   dummy()
  end
  overhead=ticks()-t
 end
 if nesting>0 then
  nesting=nesting+1
 end
end
function debugger.disable()
 if nesting>0 then
  nesting=nesting-1
 end
 if sethook and getinfo and nesting==0 then
  sethook()
 end
end
local function showtraceback(rep) 
 getdebug()
 if getinfo then
  local level=2 
  local reporter=rep or report
  while true do
   local info=getinfo(level,"Sl")
   if not info then
    break
   elseif info.what=="C" then
    reporter("%2i : %s",level-1,"C function")
   else
    reporter("%2i : %s : %s",level-1,info.short_src,info.currentline)
   end
   level=level+1
  end
 end
end
debugger.showtraceback=showtraceback


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-tpl"] = package.loaded["util-tpl"] or true

-- original size: 7722, stripped down to: 4212

if not modules then modules={} end modules ['util-tpl']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
utilities.templates=utilities.templates or {}
local templates=utilities.templates
local trace_template=false  trackers.register("templates.trace",function(v) trace_template=v end)
local report_template=logs.reporter("template")
local tostring,next=tostring,next
local format,sub,byte=string.format,string.sub,string.byte
local P,C,R,Cs,Cc,Carg,lpegmatch,lpegpatterns=lpeg.P,lpeg.C,lpeg.R,lpeg.Cs,lpeg.Cc,lpeg.Carg,lpeg.match,lpeg.patterns
local formatters=string.formatters
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
 { "'","''"   },
 { "\\","\\\\" },
 { "\r\n","\\n"  },
 { "\r","\\n"  },
}
local sqlquoted=Cs(Cc("'")*sqlescape*Cc("'"))
lpegpatterns.sqlescape=sqlescape
lpegpatterns.sqlquoted=sqlquoted
local luaescape=lpegpatterns.luaescape
local escapers={
 lua=function(s)
  return lpegmatch(luaescape,s)
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
  return lpegmatch(sqlquoted,s)
 end,
}
local luaescaper=escapers.lua
local quotedluaescaper=quotedescapers.lua
local function replacekeyunquoted(s,t,how,recurse) 
 if how==false then
  return replacekey(s,t,how,recurse)
 else
  local escaper=how and escapers[how] or luaescaper
  return escaper(replacekey(s,t,how,recurse))
 end
end
local function replacekeyquoted(s,t,how,recurse) 
 if how==false then
  return replacekey(s,t,how,recurse)
 else
  local escaper=how and quotedescapers[how] or quotedluaescaper
  return escaper(replacekey(s,t,how,recurse))
 end
end
local function replaceoptional(l,m,r,t,how,recurse)
 local v=t[l]
 return v and v~="" and lpegmatch(replacer,r,1,t,how or "lua",recurse or false) or ""
end
local function replaceformatted(l,m,r,t,how,recurse)
 local v=t[r]
 return v and formatters[l](v)
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
local nolformatted=P(":")/"%%"
local norformatted=P(":")/""
local noloptional=P("%?")/''
local noroptional=P("?%")/''
local nomoptional=P(":")/''
local args=Carg(1)*Carg(2)*Carg(3)
local key=nosingle*((C((1-nosingle)^1)*args)/replacekey)*nosingle
local quoted=nolquotedq*((C((1-norquotedq)^1)*args)/replacekeyquoted)*norquotedq
local unquoted=nolquoted*((C((1-norquoted)^1)*args)/replacekeyunquoted)*norquoted
local optional=noloptional*((C((1-nomoptional)^1)*nomoptional*C((1-noroptional)^1)*args)/replaceoptional)*noroptional
local formatted=nosingle*((Cs(nolformatted*(1-norformatted )^1)*norformatted*C((1-nosingle)^1)*args)/replaceformatted)*nosingle
local any=P(1)
   replacer=Cs((unquoted+quoted+formatted+escape+optional+key+any)^0)
local function replace(str,mapping,how,recurse)
 if mapping and str then
  return lpegmatch(replacer,str,1,mapping,how or "lua",recurse or false) or str
 else
  return str
 end
end
templates.replace=replace
function templates.replacer(str,how,recurse) 
 return function(mapping)
  return lpegmatch(replacer,str,1,mapping,how or "lua",recurse or false) or str
 end
end
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

package.loaded["util-sbx"] = package.loaded["util-sbx"] or true

-- original size: 21084, stripped down to: 13214

if not modules then modules={} end modules ['util-sbx']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
if not sandbox then require("l-sandbox") end 
local next,type=next,type
local replace=utilities.templates.replace
local collapsepath=file.collapsepath
local expandname=dir.expandname
local sortedhash=table.sortedhash
local lpegmatch=lpeg.match
local platform=os.type
local P,S,C=lpeg.P,lpeg.S,lpeg.C
local gsub=string.gsub
local lower=string.lower
local find=string.find
local concat=string.concat
local unquoted=string.unquoted
local optionalquoted=string.optionalquoted
local basename=file.basename
local nameonly=file.nameonly
local sandbox=sandbox
local validroots={}
local validrunners={}
local validbinaries=true 
local validlibraries=true 
local validators={}
local finalized=nil
local trace=false
local p_validroot=nil
local p_split=lpeg.firstofsplit(" ")
local report=logs.reporter("sandbox")
trackers.register("sandbox",function(v) trace=v end) 
sandbox.setreporter(report)
sandbox.finalizer {
 category="files",
 action=function()
  finalized=true
 end
}
local function registerroot(root,what) 
 if finalized then
  report("roots are already finalized")
 else
  if type(root)=="table" then
   root,what=root[1],root[2]
  end
  if type(root)=="string" and root~="" then
   root=collapsepath(expandname(root))
   if what=="r" or what=="ro" or what=="readable" then
    what="read"
   elseif what=="w" or what=="wo" or what=="writable" then
    what="write"
   end
   validroots[root]=what=="write" or false
  end
 end
end
sandbox.finalizer {
 category="files",
 action=function() 
  if p_validroot then
   report("roots are already initialized")
  else
   sandbox.registerroot(".","write")
   for name in sortedhash(validroots) do
    if p_validroot then
     p_validroot=P(name)+p_validroot
    else
     p_validroot=P(name)
    end
   end
   p_validroot=p_validroot/validroots
  end
 end
}
local function registerbinary(name)
 if finalized then
  report("binaries are already finalized")
 elseif type(name)=="string" and name~="" then
  if not validbinaries then
   return
  end
  if validbinaries==true then
   validbinaries={ [name]=true }
  else
   validbinaries[name]=true
  end
 elseif name==true then
  validbinaries={}
 end
end
local function registerlibrary(name)
 if finalized then
  report("libraries are already finalized")
 elseif type(name)=="string" and name~="" then
  if not validlibraries then
   return
  end
  if validlibraries==true then
   validlibraries={ [nameonly(name)]=true }
  else
   validlibraries[nameonly(name)]=true
  end
 elseif name==true then
  validlibraries={}
 end
end
local p_write=S("wa")    p_write=(1-p_write)^0*p_write
local p_path=S("\\/~$%:")  p_path=(1-p_path )^0*p_path  
local function normalized(name) 
 if platform=="windows" then
  name=gsub(name,"/","\\")
 end
 return name
end
function sandbox.possiblepath(name)
 return lpegmatch(p_path,name) and true or false
end
local filenamelogger=false
function sandbox.setfilenamelogger(l)
 filenamelogger=type(l)=="function" and l or false
end
local function validfilename(name,what)
 if p_validroot and type(name)=="string" and lpegmatch(p_path,name) then
  local asked=collapsepath(expandname(name))
  local okay=lpegmatch(p_validroot,asked)
  if okay==true then
   if filenamelogger then
    filenamelogger(name,"w",asked,true)
   end
   return name
  elseif okay==false then
   if not what then
    if filenamelogger then
     filenamelogger(name,"r",asked,true)
    end
    return name
   elseif lpegmatch(p_write,what) then
    if filenamelogger then
     filenamelogger(name,"w",asked,false)
    end
    return 
   else
    if filenamelogger then
     filenamelogger(name,"r",asked,true)
    end
    return name
   end
  elseif filenamelogger then
   filenamelogger(name,"*",name,false)
  end
 else
  return name
 end
end
local function readable(name,finalized)
 return validfilename(name,"r")
end
local function normalizedreadable(name,finalized)
 local valid=validfilename(name,"r")
 if valid then
  return normalized(valid)
 end
end
local function writeable(name,finalized)
 return validfilename(name,"w")
end
local function normalizedwriteable(name,finalized)
 local valid=validfilename(name,"w")
 if valid then
  return normalized(valid)
 end
end
validators.readable=readable
validators.writeable=normalizedwriteable
validators.normalizedreadable=normalizedreadable
validators.normalizedwriteable=writeable
validators.filename=readable
table.setmetatableindex(validators,function(t,k)
 if k then
  t[k]=readable
 end
 return readable
end)
function validators.string(s,finalized)
 if finalized and suspicious(s) then
  return ""
 else
  return s
 end
end
function validators.cache(s)
 if finalized then
  return basename(s)
 else
  return s
 end
end
function validators.url(s)
 if finalized and find("^file:") then
  return ""
 else
  return s
 end
end
local function filehandlerone(action,one,...)
 local checkedone=validfilename(one)
 if checkedone then
  return action(one,...)
 else
 end
end
local function filehandlertwo(action,one,two,...)
 local checkedone=validfilename(one)
 if checkedone then
  local checkedtwo=validfilename(two)
  if checkedtwo then
   return action(one,two,...)
  else
  end
 else
 end
end
local function iohandler(action,one,...)
 if type(one)=="string" then
  local checkedone=validfilename(one)
  if checkedone then
   return action(one,...)
  end
 elseif one then
  return action(one,...)
 else
  return action()
 end
end
local osexecute=sandbox.original(os.execute)
local iopopen=sandbox.original(io.popen)
local reported={}
local function validcommand(name,program,template,checkers,defaults,variables,reporter,strict)
 if validbinaries~=false and (validbinaries==true or validbinaries[program]) then
  local binpath=nil
  if variables then
   for variable,value in next,variables do
    local chktype=checkers[variable]
    if chktype=="verbose" then
    else
     local checker=validators[chktype]
     if checker then
      value=checker(unquoted(value),strict)
      if value then
       variables[variable]=optionalquoted(value)
      else
       report("variable %a with value %a fails the check",variable,value)
       return
      end
     else
      report("variable %a has no checker",variable)
      return
     end
    end
   end
   for variable,default in next,defaults do
    local value=variables[variable]
    if not value or value=="" then
     local chktype=checkers[variable]
     if chktype=="verbose" then
     else
      local checker=validators[chktype]
      if checker then
       default=checker(unquoted(default),strict)
       if default then
        variables[variable]=optionalquoted(default)
       else
        report("variable %a with default %a fails the check",variable,default)
        return
       end
      end
     end
    end
   end
   binpath=variables.binarypath
  end
  if type(binpath)=="string" and binpath~="" then
   program=binpath.."/"..program
  end
  local command=program.." "..replace(template,variables)
  if reporter then
   reporter("executing runner %a: %s",name,command)
  elseif trace then
   report("executing runner %a: %s",name,command)
  end
  return command
 elseif not reported[name] then
  report("executing program %a of runner %a is not permitted",program,name)
  reported[name]=true
 end
end
local runners={
 resultof=function(...)
  local command=validcommand(...)
  if command then
   if trace then
    report("resultof: %s",command)
   end
   local handle=iopopen(command,"r") 
   if handle then
    local result=handle:read("*all") or ""
    handle:close()
    return result
   end
  end
 end,
 execute=function(...)
  local command=validcommand(...)
  if command then
   if trace then
    report("execute: %s",command)
   end
   local okay=osexecute(command)
   return okay
  end
 end,
 pipeto=function(...)
  local command=validcommand(...)
  if command then
   if trace then
    report("pipeto: %s",command)
   end
   return iopopen(command,"w") 
  end
 end,
}
function sandbox.registerrunner(specification)
 if type(specification)=="string" then
  local wrapped=validrunners[specification]
  inspect(table.sortedkeys(validrunners))
  if wrapped then
   return wrapped
  else
   report("unknown predefined runner %a",specification)
   return
  end
 end
 if type(specification)~="table" then
  report("specification should be a table (or string)")
  return
 end
 local name=specification.name
 if type(name)~="string" then
  report("invalid name, string expected",name)
  return
 end
 if validrunners[name] then
  report("invalid name, runner %a already defined",name)
  return
 end
 local program=specification.program
 if type(program)=="string" then
 elseif type(program)=="table" then
  program=program[platform] or program.default or program.unix
 end
 if type(program)~="string" or program=="" then
  report("invalid runner %a specified for platform %a",name,platform)
  return
 end
 local template=specification.template
 if not template then
  report("missing template for runner %a",name)
  return
 end
 local method=specification.method   or "execute"
 local checkers=specification.checkers or {}
 local defaults=specification.defaults or {}
 local runner=runners[method]
 if runner then
  local finalized=finalized 
  local wrapped=function(variables)
   return runner(name,program,template,checkers,defaults,variables,specification.reporter,finalized)
  end
  validrunners[name]=wrapped
  return wrapped
 else
  validrunners[name]=nil
  report("invalid method for runner %a",name)
 end
end
function sandbox.getrunner(name)
 return name and validrunners[name]
end
local function suspicious(str)
 return (find(str,"[/\\]") or find(command,"..",1,true)) and true or false
end
local function binaryrunner(action,command,...)
 if validbinaries==false then
  report("no binaries permitted, ignoring command: %s",command)
  return
 end
 if type(command)~="string" then
  report("command should be a string")
  return
 end
 local program=lpegmatch(p_split,command)
 if not program or program=="" then
  report("unable to filter binary from command: %s",command)
  return
 end
 if validbinaries==true then
 elseif not validbinaries[program] then
  report("binary not permitted, ignoring command: %s",command)
  return
 elseif suspicious(command) then
  report("/ \\ or .. found, ignoring command (use sandbox.registerrunner): %s",command)
  return
 end
 return action(command,...)
end
local function dummyrunner(action,command,...)
 if type(command)=="table" then
  command=concat(command," ",command[0] and 0 or 1)
 end
 report("ignoring command: %s",command)
end
sandbox.filehandlerone=filehandlerone
sandbox.filehandlertwo=filehandlertwo
sandbox.iohandler=iohandler
function sandbox.disablerunners()
 validbinaries=false
end
function sandbox.disablelibraries()
 validlibraries=false
end
if FFISUPPORTED and ffi then
 function sandbox.disablelibraries()
  validlibraries=false
  for k,v in next,ffi do
   if k~="gc" then
    ffi[k]=nil
   end
  end
 end
 local fiiload=ffi.load
 if fiiload then
  local reported={}
  function ffi.load(name,...)
   if validlibraries==false then
   elseif validlibraries==true then
    return fiiload(name,...)
   elseif validlibraries[nameonly(name)] then
    return fiiload(name,...)
   else
   end
   if not reported[name] then
    report("using library %a is not permitted",name)
    reported[name]=true
   end
   return nil
  end
 end
end
local overload=sandbox.overload
local register=sandbox.register
 overload(loadfile,filehandlerone,"loadfile") 
if io then
 overload(io.open,filehandlerone,"io.open")
 overload(io.popen,binaryrunner,"io.popen")
 overload(io.input,iohandler,"io.input")
 overload(io.output,iohandler,"io.output")
 overload(io.lines,filehandlerone,"io.lines")
end
if os then
 overload(os.execute,binaryrunner,"os.execute")
 overload(os.spawn,dummyrunner,"os.spawn") 
 overload(os.exec,dummyrunner,"os.exec")  
 overload(os.resultof,binaryrunner,"os.resultof")
 overload(os.pipeto,binaryrunner,"os.pipeto")
 overload(os.rename,filehandlertwo,"os.rename")
 overload(os.remove,filehandlerone,"os.remove")
end
if lfs then
 overload(lfs.chdir,filehandlerone,"lfs.chdir")
 overload(lfs.mkdir,filehandlerone,"lfs.mkdir")
 overload(lfs.rmdir,filehandlerone,"lfs.rmdir")
 overload(lfs.isfile,filehandlerone,"lfs.isfile")
 overload(lfs.isdir,filehandlerone,"lfs.isdir")
 overload(lfs.attributes,filehandlerone,"lfs.attributes")
 overload(lfs.dir,filehandlerone,"lfs.dir")
 overload(lfs.lock_dir,filehandlerone,"lfs.lock_dir")
 overload(lfs.touch,filehandlerone,"lfs.touch")
 overload(lfs.link,filehandlertwo,"lfs.link")
 overload(lfs.setmode,filehandlerone,"lfs.setmode")
 overload(lfs.readlink,filehandlerone,"lfs.readlink")
 overload(lfs.shortname,filehandlerone,"lfs.shortname")
 overload(lfs.symlinkattributes,filehandlerone,"lfs.symlinkattributes")
end
if zip then
 zip.open=register(zip.open,filehandlerone,"zip.open")
end
sandbox.registerroot=registerroot
sandbox.registerbinary=registerbinary
sandbox.registerlibrary=registerlibrary
sandbox.validfilename=validfilename


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-mrg"] = package.loaded["util-mrg"] or true

-- original size: 7819, stripped down to: 5881

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
local digit=patterns.digit
local emptyline=space^0*eol
local operator1=P("<=")+P(">=")+P("~=")+P("..")+S("/^<>=*+%%")
local operator2=S("*+/")
local operator3=S("-")
local operator4=P("..")
local separator=S(",;")
local ignore=(P("]")*space^1*P("=")*space^1*P("]"))/"]=["+(P("=")*space^1*P("{"))/"={"+(P("(")*space^1)/"("+(P("{")*(space+eol)^1*P("}"))/"{}"
local strings=quoted 
local longcmt=(emptyline^0*P("--")*longstring*emptyline^0)/""
local longstr=longstring
local comment=emptyline^0*P("--")*P("-")^0*(1-eol)^0*emptyline^1/"\n"
local optionalspaces=space^0/""
local mandatespaces=space^1/""
local optionalspacing=(eol+space)^0/""
local mandatespacing=(eol+space)^1/""
local pack=digit*space^1*operator4*optionalspacing+optionalspacing*operator1*optionalspacing+optionalspacing*operator2*optionalspaces+mandatespacing*operator3*mandatespaces+optionalspaces*separator*optionalspaces
local lines=emptyline^2/"\n"
local spaces=(space*space)/" "
local spaces=(space*space*space*space)/" "
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

package.loaded["util-env"] = package.loaded["util-env"] or true

-- original size: 9738, stripped down to: 5531

if not modules then modules={} end modules ['util-env']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local allocate,mark=utilities.storage.allocate,utilities.storage.mark
local format,sub,match,gsub,find=string.format,string.sub,string.match,string.gsub,string.find
local unquoted,quoted,optionalquoted=string.unquoted,string.quoted,string.optionalquoted
local concat,insert,remove=table.concat,table.insert,table.remove
environment=environment or {}
local environment=environment
local setlocale=os.setlocale
setlocale(nil,nil)
local report=logs.reporter("system")
function os.setlocale(a,b)
 if a or b then
  if report then
   report()
   report("You're messing with os.locale in a supposedly locale neutral enviroment. From")
   report("now on are on your own and without support. Crashes or unexpected side effects")
   report("can happen but don't bother the luatex and context developer team with it.")
   report()
   report=nil
  end
  setlocale(a,b)
 end
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
 environment.used_as_library=true
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
 local arguments={}
 local files={}
 environment.arguments=arguments
 environment.files=files
 environment.sortedflags=nil
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
 if not environment.ownname then
  if os.selfpath and os.selfname then
   environment.ownname=file.addsuffix(file.join(os.selfpath,os.selfname),"lua")
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
 local resolveprefix=resolvers.resolve 
 arg=arg or environment.originalarguments
 if noquote and #arg==1 then
  return unquoted(resolveprefix and resolveprefix(arg[1]) or arg[1])
 elseif #arg>0 then
  local result={}
  for i=1,#arg do
   result[i]=optionalquoted(resolveprefix and resolveprefix(arg[i]) or resolveprefix)
  end
  return concat(result," ")
 else
  return ""
 end
end
function environment.relativepath(path,root)
 if not path then
  path=""
 end
 if not file.is_rootbased_path(path) then
  if not root then
   root=file.pathpart(environment.ownscript or environment.ownname or ".")
  end
  if root=="" then
   root="."
  end
  path=root.."/"..path
 end
 return file.collapsepath(path,true)
end
if arg then
 local newarg,instring={},false
 for index=1,#arg do
  local argument=arg[index]
  if find(argument,"^\"") then
   if find(argument,"\"$") then
    newarg[#newarg+1]=gsub(argument,"^\"(.-)\"$","%1")
    instring=false
   else
    newarg[#newarg+1]=gsub(argument,"^\"","")
    instring=true
   end
  elseif find(argument,"\"$") then
   if instring then
    newarg[#newarg]=newarg[#newarg].." "..gsub(argument,"\"$","")
    instring=false
   else
    newarg[#newarg+1]=argument
   end
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

-- original size: 6551, stripped down to: 4315

 if not modules then modules={} end modules ['luat-env']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local rawset,loadfile=rawset,loadfile
local gsub=string.gsub
local trace_locating=false  trackers.register("resolvers.locating",function(v) trace_locating=v end)
local report_lua=logs.reporter("resolvers","lua")
local luautilities=utilities.lua
local luasuffixes=luautilities.suffixes
local texgettoks=tex and tex.gettoks
environment=environment or {}
local environment=environment
local mt={
 __index=function(_,k)
  if k=="version" then
   local version=texgettoks and texgettoks("contextversiontoks")
   if version and version~="" then
    rawset(environment,"version",version)
    return version
   else
    return "unknown"
   end
  elseif k=="kind" then
   local kind=texgettoks and texgettoks("contextkindtoks")
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
 if CONTEXTLMTXMODE and CONTEXTLMTXMODE>0 and file.suffix(filename)=="lua" then
  local resolved=resolvers.findfile(file.replacesuffix(filename,"lmt")) or ""
  if resolved~="" then
   return resolved
  end
 end
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
local stripindeed=false  directives.register("system.compile.strip",function(v) stripindeed=v end)
local function strippable(filename)
 if stripindeed then
  local modu=modules[file.nameonly(filename)]
  return modu and modu.dataonly
 else
  return false
 end
end
function environment.luafilechunk(filename,silent,macros) 
 filename=file.replacesuffix(filename,"lua")
 local fullname=environment.luafile(filename)
 if fullname and fullname~="" then
  local data=luautilities.loadedluacode(fullname,strippable,filename,macros)
  if not silent then
   report_lua("loading file %a %s",fullname,not data and "failed" or "succeeded")
  end
  return data
 else
  if not silent then
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
  luaname=filename 
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
  chunk()
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
   chunk()
   return true
  end
 end
 return false
end
environment.filenames=setmetatable({},{
 __index=function(t,k)
  local v=environment.files[k]
  if v then
   return (gsub(v,"%.+$",""))
  end
 end,
 __newindex=function(t,k)
 end,
 __len=function(t)
  return #environment.files
 end,
} )


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["util-zip"] = package.loaded["util-zip"] or true

-- original size: 19496, stripped down to: 10858

if not modules then modules={} end modules ['util-zip']={
 version=1.001,
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local type,tostring,tonumber=type,tostring,tonumber
local sort=table.sort
local find,format,sub,gsub=string.find,string.format,string.sub,string.gsub
local osdate,ostime,osclock=os.date,os.time,os.clock
local ioopen=io.open
local loaddata,savedata=io.loaddata,io.savedata
local filejoin,isdir,dirname,mkdirs=file.join,lfs.isdir,file.dirname,dir.mkdirs
local files=utilities.files
local openfile=files.open
local closefile=files.close
local readstring=files.readstring
local readcardinal2=files.readcardinal2le
local readcardinal4=files.readcardinal4le
local setposition=files.setposition
local getposition=files.getposition
local band=bit32.band
local rshift=bit32.rshift
local lshift=bit32.lshift
local decompress,expandsize,calculatecrc
 local zlibdecompress=zlib.decompress
 local zlibexpandsize=zlib.expandsize
 local zlibchecksum=zlib.crc32
 decompress=function(source)
  return zlibdecompress(source,-15) 
 end
 expandsize=zlibexpandsize and function(source,targetsize)
  return zlibexpandsize(source,targetsize,-15) 
 end or decompress
 calculatecrc=function(buffer,initial)
  return zlibchecksum(initial or 0,buffer)
 end
local zipfiles={}
utilities.zipfiles=zipfiles
local openzipfile,closezipfile,unzipfile,foundzipfile,getziphash,getziplist  do
 function openzipfile(name)
  return {
   name=name,
   handle=openfile(name,0),
  }
 end
 local function collect(z)
  if not z.list then
   local list={}
   local hash={}
   local position=0
   local index=0
   local handle=z.handle
   while true do
    setposition(handle,position)
    local signature=readstring(handle,4)
    if signature=="PK\3\4" then
     local version=readcardinal2(handle)
     local flag=readcardinal2(handle)
     local method=readcardinal2(handle)
     local filetime=readcardinal2(handle)
     local filedate=readcardinal2(handle)
     local crc32=readcardinal4(handle)
     local compressed=readcardinal4(handle)
     local uncompressed=readcardinal4(handle)
     local namelength=readcardinal2(handle)
     local extralength=readcardinal2(handle)
     local filename=readstring(handle,namelength)
     local descriptor=band(flag,8)~=0
     local encrypted=band(flag,1)~=0
     local acceptable=method==0 or method==8
     local skipped=0
     local size=0
     if encrypted then
      size=readcardinal2(handle)
      skipbytes(size)
      skipped=skipped+size+2
      skipbytes(8)
      skipped=skipped+8
      size=readcardinal2(handle)
      skipbytes(size)
      skipped=skipped+size+2
      size=readcardinal4(handle)
      skipbytes(size)
      skipped=skipped+size+4
      size=readcardinal2(handle)
      skipbytes(size)
      skipped=skipped+size+2
     end
     position=position+30+namelength+extralength+skipped
     if descriptor then
      setposition(handle,position+compressed)
      crc32=readcardinal4(handle)
      compressed=readcardinal4(handle)
      uncompressed=readcardinal4(handle)
     end
     if acceptable then
      index=index+1
      local data={
       filename=filename,
       index=index,
       position=position,
       method=method,
       compressed=compressed,
       uncompressed=uncompressed,
       crc32=crc32,
       encrypted=encrypted,
      }
      hash[filename]=data
      list[index]=data
     else
     end
     position=position+compressed
    else
     break
    end
    z.list=list
    z.hash=hash
   end
  end
 end
 function getziplist(z)
  local list=z.list
  if not list then
   collect(z)
  end
  return z.list
 end
 function getziphash(z)
  local hash=z.hash
  if not hash then
   collect(z)
  end
  return z.hash
 end
 function foundzipfile(z,name)
  return getziphash(z)[name]
 end
 function closezipfile(z)
  local f=z.handle
  if f then
   closefile(f)
   z.handle=nil
  end
 end
 function unzipfile(z,filename,check)
  local hash=z.hash
  if not hash then
   hash=zipfiles.hash(z)
  end
  local data=hash[filename] 
  if not data then
  end
  if data then
   local handle=z.handle
   local position=data.position
   local compressed=data.compressed
   if compressed>0 then
    setposition(handle,position)
    local result=readstring(handle,compressed)
    if data.method==8 then
     if expandsize then
      result=expandsize(result,data.uncompressed)
     else
      result=decompress(result)
     end
    end
    if check and data.crc32~=calculatecrc(result) then
     print("checksum mismatch")
     return ""
    end
    return result
   else
    return ""
   end
  end
 end
 zipfiles.open=openzipfile
 zipfiles.close=closezipfile
 zipfiles.unzip=unzipfile
 zipfiles.hash=getziphash
 zipfiles.list=getziplist
 zipfiles.found=foundzipfile
end
if xzip then 
 local writecardinal1=files.writebyte
 local writecardinal2=files.writecardinal2le
 local writecardinal4=files.writecardinal4le
 local logwriter=logs.writer
 local globpattern=dir.globpattern
 local compress=xzip.compress
 local checksum=xzip.crc32
 local function fromdostime(dostime,dosdate)
  return ostime {
   year=rshift(dosdate,9)+1980,
   month=band(rshift(dosdate,5),0x0F),
   day=band((dosdate   ),0x1F),
   hour=band(rshift(dostime,11)    ),
   min=band(rshift(dostime,5),0x3F),
   sec=band((dostime   ),0x1F),
  }
 end
 local function todostime(time)
  local t=osdate("*t",time)
  return
   lshift(t.year-1980,9)+lshift(t.month,5)+t.day,
   lshift(t.hour,11)+lshift(t.min,5)+rshift(t.sec,1)
 end
 local function openzip(filename,level,comment,verbose)
  local f=ioopen(filename,"wb")
  if f then
   return {
    filename=filename,
    handle=f,
    list={},
    level=tonumber(level) or 3,
    comment=tostring(comment),
    verbose=verbose,
    uncompressed=0,
    compressed=0,
   }
  end
 end
 local function writezip(z,name,data,level,time)
  local f=z.handle
  local list=z.list
  local level=tonumber(level) or z.level or 3
  local method=8
  local zipped=compress(data,level)
  local checksum=checksum(data)
  local verbose=z.verbose
  if not zipped then
   method=0
   zipped=data
  end
  local start=f:seek()
  local compressed=#zipped
  local uncompressed=#data
  z.compressed=z.compressed+compressed
  z.uncompressed=z.uncompressed+uncompressed
  if verbose then
   local pct=100*compressed/uncompressed
   if pct>=100 then
    logwriter(format("%10i        %s",uncompressed,name))
   else
    logwriter(format("%10i  %02.1f  %s",uncompressed,pct,name))
   end
  end
  f:write("\x50\x4b\x03\x04")
  writecardinal2(f,0)   
  writecardinal2(f,0)   
  writecardinal2(f,method)    
  writecardinal2(f,0)   
  writecardinal2(f,0)   
  writecardinal4(f,checksum)  
  writecardinal4(f,compressed)   
  writecardinal4(f,uncompressed) 
  writecardinal2(f,#name)  
  writecardinal2(f,0)
  f:write(name)      
  f:write(zipped)
  list[#list+1]={ #zipped,#data,name,checksum,start,time or 0 }
 end
 local function closezip(z)
  local f=z.handle
  local list=z.list
  local comment=z.comment
  local verbose=z.verbose
  local count=#list
  local start=f:seek()
  for i=1,count do
   local l=list[i]
   local compressed=l[1]
   local uncompressed=l[2]
   local name=l[3]
   local checksum=l[4]
   local start=l[5]
   local time=l[6]
   local date,time=todostime(time)
   f:write('\x50\x4b\x01\x02')
   writecardinal2(f,0)   
   writecardinal2(f,0)   
   writecardinal2(f,0)   
   writecardinal2(f,8)   
   writecardinal2(f,time)   
   writecardinal2(f,date)   
   writecardinal4(f,checksum)  
   writecardinal4(f,compressed)   
   writecardinal4(f,uncompressed) 
   writecardinal2(f,#name)  
   writecardinal2(f,0)   
   writecardinal2(f,0)   
   writecardinal2(f,0)   
   writecardinal2(f,0)   
   writecardinal4(f,0)   
   writecardinal4(f,start)  
   f:write(name)      
  end
  local stop=f:seek()
  local size=stop-start
  f:write('\x50\x4b\x05\x06')
  writecardinal2(f,0)   
  writecardinal2(f,0)   
  writecardinal2(f,count)  
  writecardinal2(f,count)  
  writecardinal4(f,size)   
  writecardinal4(f,start)  
  if type(comment)=="string" and comment~="" then
   writecardinal2(f,#comment) 
   f:write(comment)     
  else
   writecardinal2(f,0)
  end
  if verbose then
   local compressed=z.compressed
   local uncompressed=z.uncompressed
   local filename=z.filename
   local pct=100*compressed/uncompressed
   logwriter("")
   if pct>=100 then
    logwriter(format("%10i        %s",uncompressed,filename))
   else
    logwriter(format("%10i  %02.1f  %s",uncompressed,pct,filename))
   end
  end
  f:close()
 end
 local function zipdir(zipname,path,level,verbose)
  if type(zipname)=="table" then
   verbose=zipname.verbose
   level=zipname.level
   path=zipname.path
   zipname=zipname.zipname
  end
  if not zipname or zipname=="" then
   return
  end
  if not path or path=="" then
   path="."
  end
  if not isdir(path) then
   return
  end
  path=gsub(path,"\\+","/")
  path=gsub(path,"/+","/")
  local list={}
  local count=0
  globpattern(path,"",true,function(name,size,time)
   count=count+1
   list[count]={ name,time }
  end)
  sort(list,function(a,b)
   return a[1]<b[1]
  end)
  local zipf=openzip(zipname,level,comment,verbose)
  if zipf then
   local p=#path+2
   for i=1,count do
    local li=list[i]
    local name=li[1]
    local time=li[2]
    local data=loaddata(name)
    local name=sub(name,p,#name)
    writezip(zipf,name,data,level,time,verbose)
   end
   closezip(zipf)
  end
 end
 local function unzipdir(zipname,path,verbose)
  if type(zipname)=="table" then
   verbose=zipname.verbose
   path=zipname.path
   zipname=zipname.zipname
  end
  if not zipname or zipname=="" then
   return
  end
  if not path or path=="" then
   path="."
  end
  local z=openzipfile(zipname)
  if z then
   local list=getziplist(z)
   if list then
    local total=0
    local count=#list
    local step=number.idiv(count,10)
    local done=0
    local steps=verbose=="steps"
    local time=steps and osclock()
    for i=1,count do
     local l=list[i]
     local n=l.filename
     local d=unzipfile(z,n) 
     if d then
      local p=filejoin(path,n)
      if mkdirs(dirname(p)) then
       if steps then
        total=total+#d
        done=done+1
        if done>=step then
         done=0
         logwriter(format("%4i files of %4i done, %10i bytes, %0.3f seconds",i,count,total,osclock()-time))
        end
       elseif verbose then
        logwriter(n)
       end
       savedata(p,d)
      end
     else
      logwriter(format("problem with file %s",n))
     end
    end
    if steps then
     logwriter(format("%4i files of %4i done, %10i bytes, %0.3f seconds",count,count,total,osclock()-time))
    end
    closezipfile(z)
    return true
   else
    closezipfile(z)
   end
  end
 end
 zipfiles.zipdir=zipdir
 zipfiles.unzipdir=unzipdir
end
zipfiles.gunzipfile=gzip.load


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["lxml-tab"] = package.loaded["lxml-tab"] or true

-- original size: 61191, stripped down to: 35864

if not modules then modules={} end modules ['lxml-tab']={
 version=1.001,
 comment="this module is the basis for the lxml-* ones",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local trace_entities=false  trackers.register("xml.entities",function(v) trace_entities=v end)
local report_xml=logs and logs.reporter("xml","core") or function(...) print(string.format(...)) end
if lpeg.setmaxstack then lpeg.setmaxstack(1000) end 
xml=xml or {}
local xml=xml
local concat,remove,insert=table.concat,table.remove,table.insert
local type,next,setmetatable,getmetatable,tonumber,rawset,select=type,next,setmetatable,getmetatable,tonumber,rawset,select
local lower,find,match,gsub=string.lower,string.find,string.match,string.gsub
local sort=table.sort
local utfchar=utf.char
local lpegmatch,lpegpatterns=lpeg.match,lpeg.patterns
local P,S,R,C,V,C,Cs=lpeg.P,lpeg.S,lpeg.R,lpeg.C,lpeg.V,lpeg.C,lpeg.Cs
local formatters=string.formatters
do 
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
end
local nsremap,resolvens=xml.xmlns,xml.resolvens
local stack,level,top,at,xmlnms,errorstr
local entities,parameters
local strip,utfize,resolve,cleanup,resolve_predefined,unify_predefined
local dcache,hcache,acache
local mt,dt,nt
local currentfilename,currentline,linenumbers
local grammar_parsed_text_one
local grammar_parsed_text_two
local grammar_unparsed_text
local handle_hex_entity
local handle_dec_entity
local handle_any_entity_dtd
local handle_any_entity_text
local function preparexmlstate(settings)
 if settings then
  linenumbers=settings.linenumbers
  stack={}
  level=0
  top={}
  at={}
  mt={}
  dt={}
  nt=0   
  xmlns={}
  errorstr=nil
  strip=settings.strip_cm_and_dt
  utfize=settings.utfize_entities
  resolve=settings.resolve_entities   
  resolve_predefined=settings.resolve_predefined_entities 
  unify_predefined=settings.unify_predefined_entities   
  cleanup=settings.text_cleanup
  entities=settings.entities or {}
  currentfilename=settings.currentresource
  currentline=1
  parameters={}
  reported_at_errors={}
  dcache={}
  hcache={}
  acache={}
  if utfize==nil then
   settings.utfize_entities=true
   utfize=true
  end
  if resolve_predefined==nil then
   settings.resolve_predefined_entities=true
   resolve_predefined=true
  end
 else
  linenumbers=false
  stack=nil
  level=nil
  top=nil
  at=nil
  mt=nil
  dt=nil
  nt=nil
  xmlns=nil
  errorstr=nil
  strip=nil
  utfize=nil
  resolve=nil
  resolve_predefined=nil
  unify_predefined=nil
  cleanup=nil
  entities=nil
  parameters=nil
  reported_at_errors=nil
  dcache=nil
  hcache=nil
  acache=nil
  currentfilename=nil
  currentline=1
 end
end
local function initialize_mt(root)
 mt={ __index=root } 
end
function xml.setproperty(root,k,v)
 getmetatable(root).__index[k]=v
end
function xml.checkerror(top,toclose)
 return "" 
end
local checkns=xml.checkns
local function add_attribute(namespace,tag,value)
 if cleanup and value~="" then
  value=cleanup(value) 
 end
 if tag=="xmlns" then
  xmlns[#xmlns+1]=resolvens(value)
  at[tag]=value
 elseif namespace=="" then
  at[tag]=value
 elseif namespace=="xmlns" then
  checkns(tag,value)
  at["xmlns:"..tag]=value
 else
  at[namespace..":"..tag]=value
 end
end
local function add_empty(spacing,namespace,tag)
 if spacing~="" then
  nt=nt+1
  dt[nt]=spacing
 end
 local resolved=namespace=="" and xmlns[#xmlns] or nsremap[namespace] or namespace
 top=stack[level]
 dt=top.dt
 nt=#dt+1
 local t=linenumbers and {
  ns=namespace or "",
  rn=resolved,
  tg=tag,
  at=at,
  dt={},
  ni=nt,
  cf=currentfilename,
  cl=currentline,
  __p__=top,
 } or {
  ns=namespace or "",
  rn=resolved,
  tg=tag,
  at=at,
  dt={},
  ni=nt,
  __p__=top,
 }
 dt[nt]=t
 setmetatable(t,mt)
 if at.xmlns then
  remove(xmlns)
 end
 at={}
end
local function add_begin(spacing,namespace,tag)
 if spacing~="" then
  nt=nt+1
  dt[nt]=spacing
 end
 local resolved=namespace=="" and xmlns[#xmlns] or nsremap[namespace] or namespace
 dt={}
 top=linenumbers and {
  ns=namespace or "",
  rn=resolved,
  tg=tag,
  at=at,
  dt=dt,
  ni=nil,
  cf=currentfilename,
  cl=currentline,
  __p__=stack[level],
 } or {
  ns=namespace or "",
  rn=resolved,
  tg=tag,
  at=at,
  dt=dt,
  ni=nil,
  __p__=stack[level],
 }
 setmetatable(top,mt)
 nt=0
 level=level+1
 stack[level]=top
 at={}
end
local function add_end(spacing,namespace,tag)
 if spacing~="" then
  nt=nt+1
  dt[nt]=spacing
 end
 local toclose=stack[level]
 level=level-1
 top=stack[level]
 if level<1 then
  errorstr=formatters["unable to close %s %s"](tag,xml.checkerror(top,toclose) or "")
  report_xml(errorstr)
 elseif toclose.tg~=tag then 
  errorstr=formatters["unable to close %s with %s %s"](toclose.tg,tag,xml.checkerror(top,toclose) or "")
  report_xml(errorstr)
 end
 dt=top.dt
 nt=#dt+1
 dt[nt]=toclose
 toclose.ni=nt 
 if toclose.at.xmlns then
  remove(xmlns)
 end
end
local function add_text(text)
 if text=="" then
  return
 end
 if cleanup then
  if nt>0 then
   local s=dt[nt]
   if type(s)=="string" then
    dt[nt]=s..cleanup(text)
   else
    nt=nt+1
    dt[nt]=cleanup(text)
   end
  else
   nt=1
   dt[1]=cleanup(text)
  end
 else
  if nt>0 then
   local s=dt[nt]
   if type(s)=="string" then
    dt[nt]=s..text
   else
    nt=nt+1
    dt[nt]=text
   end
  else
   nt=1
   dt[1]=text
  end
 end
end
local function add_special(what,spacing,text)
 if spacing~="" then
  nt=nt+1
  dt[nt]=spacing
 end
 if strip and (what=="@cm@" or what=="@dt@") then
 else
  nt=nt+1
  dt[nt]=linenumbers and {
   special=true,
   ns="",
   tg=what,
   ni=nil,
   dt={ text },
   cf=currentfilename,
   cl=currentline,
  } or {
   special=true,
   ns="",
   tg=what,
   ni=nil,
   dt={ text },
  }
 end
end
local function set_message(txt)
 errorstr="garbage at the end of the file: "..gsub(txt,"([ \n\r\t]*)","")
end
local function attribute_value_error(str)
 if not reported_at_errors[str] then
  report_xml("invalid attribute value %a",str)
  reported_at_errors[str]=true
  at._error_=str
 end
 return str
end
local function attribute_specification_error(str)
 if not reported_at_errors[str] then
  report_xml("invalid attribute specification %a",str)
  reported_at_errors[str]=true
  at._error_=str
 end
 return str
end
do
 local badentity="&" 
 xml.placeholders={
  unknown_dec_entity=function(str) return str=="" and badentity or formatters["&%s;"](str) end,
  unknown_hex_entity=function(str) return formatters["&#x%s;"](str) end,
  unknown_any_entity=function(str) return formatters["&#x%s;"](str) end,
 }
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
 local p_rest=(1-P(";"))^0
 local p_many=P(1)^0
 local parsedentity=P("&#")*(P("x")*(p_rest/fromhex)+(p_rest/fromdec))*P(";")*P(-1)+P ("#")*(P("x")*(p_many/fromhex)+(p_many/fromdec))
 xml.parsedentitylpeg=parsedentity
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
 local privates_p={ 
 }
 local privates_s={ 
  [ [["]] ]="&U+22;",
  [ [[#]] ]="&U+23;",
  [ [[$]] ]="&U+24;",
  [ [[%]] ]="&U+25;",
  [ [[&]] ]="&U+26;",
  [ [[']] ]="&U+27;",
  [ [[<]] ]="&U+3C;",
  [ [[>]] ]="&U+3E;",
  [ [[\]] ]="&U+5C;",
  [ [[{]] ]="&U+7B;",
  [ [[|]] ]="&U+7C;",
  [ [[}]] ]="&U+7D;",
  [ [[~]] ]="&U+7E;",
 }
 local privates_x={ 
  [ [["]] ]="&U+22;",
  [ [[#]] ]="&U+23;",
  [ [[$]] ]="&U+24;",
  [ [[%]] ]="&U+25;",
  [ [[']] ]="&U+27;",
  [ [[\]] ]="&U+5C;",
  [ [[{]] ]="&U+7B;",
  [ [[|]] ]="&U+7C;",
  [ [[}]] ]="&U+7D;",
  [ [[~]] ]="&U+7E;",
 }
 local privates_n={ 
 }
 local escaped=utf.remapper(privates_u,"dynamic")
 local unprivatized=utf.remapper(privates_p,"dynamic")
 local unspecialized=utf.remapper(privates_s,"dynamic")
 local despecialized=utf.remapper(privates_x,"dynamic")
 xml.unprivatized=unprivatized
 xml.unspecialized=unspecialized
 xml.despecialized=despecialized
 xml.escaped=escaped
 local function unescaped(s)
  local p=privates_n[s]
  if not p then
   nofprivates=nofprivates+1
   p=utfchar(nofprivates)
   privates_n[s]=p
   s="&"..s..";" 
   privates_u[p]=s
   privates_p[p]=s
   privates_s[p]=s
  end
  return p
 end
 xml.privatetoken=unescaped
 xml.privatecodes=privates_n
 xml.specialcodes=privates_s
 function xml.addspecialcode(key,value)
  privates_s[key]=value or "&"..s..";"
 end
 handle_hex_entity=function(str)
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
 handle_dec_entity=function(str)
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
 handle_any_entity_dtd=function(str)
  if resolve then
   local a=resolve_predefined and predefined_simplified[str] 
   if a then
    if trace_entities then
     report_xml("resolving entity &%s; to predefined %a",str,a)
    end
   else
    if type(resolve)=="function" then
     a=resolve(str,entities) or entities[str]
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
       a=badentity
      else
       a="&"..str..";"
      end
     end
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
     a=badentity
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
 handle_any_entity_text=function(str)
  if resolve then
   local a=resolve_predefined and predefined_simplified[str]
   if a then
    if trace_entities then
     report_xml("resolving entity &%s; to predefined %a",str,a)
    end
   else
    if type(resolve)=="function" then
     a=resolve(str,entities) or entities[str]
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
     a=lpegmatch(grammar_parsed_text_two,a) or a
     if type(a)=="number" then
      return ""
     else
      a=lpegmatch(parsedentity,a) or a 
      if trace_entities then
       report_xml("resolving entity &%s; to internal %a",str,a)
      end
     end
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
       a=badentity
      else
       a="&"..str..";"
      end
     end
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
     a=badentity
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
 local p_rest=(1-P(";"))^1
 local spec={
  [0x23]="\\Ux{23}",
  [0x24]="\\Ux{24}",
  [0x25]="\\Ux{25}",
  [0x5C]="\\Ux{5C}",
  [0x7B]="\\Ux{7B}",
  [0x7C]="\\Ux{7C}",
  [0x7D]="\\Ux{7D}",
  [0x7E]="\\Ux{7E}",
 }
 local hash=table.setmetatableindex(spec,function(t,k)
  local v=utfchar(k)
  t[k]=v
  return v
 end)
 local function fromuni(s)
  local n=tonumber(s,16)
  if n then
   return hash[n]
  else
   return formatters["u:%s"](s),true
  end
 end
 local function fromhex(s)
  local n=tonumber(s,16)
  if n then
   return hash[n]
  else
   return formatters["h:%s"](s),true
  end
 end
 local function fromdec(s)
  local n=tonumber(s)
  if n then
   return hash[n]
  else
   return formatters["d:%s"](s),true
  end
 end
 local reparsedentity=P("U+")*(p_rest/fromuni)+P("#")*(
   P("x")*(p_rest/fromhex)+p_rest/fromdec
  )
 local hash=table.setmetatableindex(function(t,k)
  local v=utfchar(k)
  t[k]=v
  return v
 end)
 local function fromuni(s)
  local n=tonumber(s,16)
  if n then
   return hash[n]
  else
   return formatters["u:%s"](s),true
  end
 end
 local function fromhex(s)
  local n=tonumber(s,16)
  if n then
   return hash[n]
  else
   return formatters["h:%s"](s),true
  end
 end
 local function fromdec(s)
  local n=tonumber(s)
  if n then
   return hash[n]
  else
   return formatters["d:%s"](s),true
  end
 end
 local unescapedentity=P("U+")*(p_rest/fromuni)+P("#")*(
   P("x")*(p_rest/fromhex)+p_rest/fromdec
  )
 xml.reparsedentitylpeg=reparsedentity   
 xml.unescapedentitylpeg=unescapedentity  
end
local escaped=xml.escaped
local unescaped=xml.unescaped
local placeholders=xml.placeholders
local function handle_end_entity(str)
 report_xml("error in entity, %a found without ending %a",str,";")
 return str
end
local function handle_crap_error(chr)
 report_xml("error in parsing, unexpected %a found ",chr)
 add_text(chr)
 return chr
end
local function handlenewline()
 currentline=currentline+1
end
local spacetab=S(' \t')
local space=S(' \r\n\t')
local newline=lpegpatterns.newline/handlenewline
local anything=P(1)
local open=P('<')
local close=P('>')
local squote=S("'")
local dquote=S('"')
local equal=P('=')
local slash=P('/')
local colon=P(':')
local semicolon=P(';')
local ampersand=P('&')
local valid_0=R("\128\255") 
local valid_1=R('az','AZ')+S('_')+valid_0
local valid_2=valid_1+R('09')+S('-.')
local valid=valid_1*valid_2^0
local name_yes=C(valid^1)*colon*C(valid^1)
local name_nop=C(P(true))*C(valid^1)
local name=name_yes+name_nop
local utfbom=lpegpatterns.utfbom 
local spacing=C(space^0)
local space_nl=spacetab+newline
local spacing_nl=Cs((space_nl)^0)
local anything_nl=newline+P(1)
local function weirdentity(k,v)
 if trace_entities then
  report_xml("registering %s entity %a as %a","weird",k,v)
 end
 parameters[k]=v
end
local function normalentity(k,v)
 if trace_entities then
  report_xml("registering %s entity %a as %a","normal",k,v)
 end
 entities[k]=v
end
local function systementity(k,v,n)
 if trace_entities then
  report_xml("registering %s entity %a as %a","system",k,v)
 end
 entities[k]=v
end
local function publicentity(k,v,n)
 if trace_entities then
  report_xml("registering %s entity %a as %a","public",k,v)
 end
 entities[k]=v
end
local function entityfile(pattern,k,v,n)
 if n then
  local okay,data
  local loadbinfile=resolvers and resolvers.loadbinfile
  if loadbinfile then
   okay,data=loadbinfile(n)
  else
   data=io.loaddata(n)
   okay=data and data~=""
  end
  if okay then
   if trace_entities then
    report_xml("loading public entities %a as %a from %a",k,v,n)
   end
   lpegmatch(pattern,data)
   return
  end
 end
 report_xml("ignoring public entities %a as %a from %a",k,v,n)
end
local function install(spacenewline,spacing,anything)
 local anyentitycontent=(1-open-semicolon-space-close-ampersand)^0
 local hexentitycontent=R("AF","af","09")^1
 local decentitycontent=R("09")^1
 local parsedentity=P("#")/""*(
         P("x")/""*(hexentitycontent/handle_hex_entity)+(decentitycontent/handle_dec_entity)
        )+(anyentitycontent/handle_any_entity_dtd) 
 local parsedentity_text=P("#")/""*(
         P("x")/""*(hexentitycontent/handle_hex_entity)+(decentitycontent/handle_dec_entity)
        )+(anyentitycontent/handle_any_entity_text) 
 local entity=(ampersand/"")*parsedentity*(semicolon/"")+ampersand*(anyentitycontent/handle_end_entity)
 local entity_text=(ampersand/"")*parsedentity_text*(semicolon/"")+ampersand*(anyentitycontent/handle_end_entity)
 local text_unparsed=Cs((anything-open)^1)
 local text_parsed=(Cs((anything-open-ampersand)^1)/add_text+Cs(entity_text)/add_text)^1
 local somespace=(spacenewline)^1
 local optionalspace=(spacenewline)^0
 local value=(squote*Cs((entity+(anything-squote))^0)*squote)+(dquote*Cs((entity+(anything-dquote))^0)*dquote) 
 local endofattributes=slash*close+close 
 local whatever=space*name*optionalspace*equal
 local wrongvalue=Cs(P(entity+(1-space-endofattributes))^1)/attribute_value_error
 local attributevalue=value+wrongvalue
 local attribute=(somespace*name*optionalspace*equal*optionalspace*attributevalue)/add_attribute
 local attributes=(attribute+somespace^-1*(((anything-endofattributes)^1)/attribute_specification_error))^0
 local parsedtext=text_parsed   
 local unparsedtext=text_unparsed/add_text
 local balanced=P { "["*((anything-S"[]")+V(1))^0*"]" } 
 local emptyelement=(spacing*open*name*attributes*optionalspace*slash*close)/add_empty
 local beginelement=(spacing*open*name*attributes*optionalspace*close)/add_begin
 local endelement=(spacing*open*slash*name*optionalspace*close)/add_end
 local begincomment=open*P("!--")
 local endcomment=P("--")*close
 local begininstruction=open*P("?")
 local endinstruction=P("?")*close
 local begincdata=open*P("![CDATA[")
 local endcdata=P("]]")*close
 local someinstruction=C((anything-endinstruction)^0)
 local somecomment=C((anything-endcomment )^0)
 local somecdata=C((anything-endcdata   )^0)
 local begindoctype=open*P("!DOCTYPE")
 local enddoctype=close
 local beginset=P("[")
 local endset=P("]")
 local wrdtypename=C((anything-somespace-P(";"))^1)
 local doctypename=C((anything-somespace-close)^0)
 local elementdoctype=optionalspace*P("<!ELEMENT")*(anything-close)^0*close
 local basiccomment=begincomment*((anything-endcomment)^0)*endcomment
 local weirdentitytype=P("%")*(somespace*doctypename*somespace*value)/weirdentity
 local normalentitytype=(doctypename*somespace*value)/normalentity
 local publicentitytype=(doctypename*somespace*P("PUBLIC")*somespace*value)/publicentity
 local systementitytype=(doctypename*somespace*P("SYSTEM")*somespace*value*somespace*P("NDATA")*somespace*doctypename)/systementity
 local entitydoctype=optionalspace*P("<!ENTITY")*somespace*(systementitytype+publicentitytype+normalentitytype+weirdentitytype)*optionalspace*close
 local publicentityfile=(doctypename*somespace*P("PUBLIC")*somespace*value*(somespace*value)^0)/function(...)
  entityfile(entitydoctype,...)
 end
 local function weirdresolve(s)
  lpegmatch(entitydoctype,parameters[s])
 end
 local function normalresolve(s)
  lpegmatch(entitydoctype,entities[s])
 end
 local entityresolve=P("%")*(wrdtypename/weirdresolve )*P(";")+P("&")*(wrdtypename/normalresolve)*P(";")
 entitydoctype=entitydoctype+entityresolve
 local doctypeset=beginset*optionalspace*P(elementdoctype+entitydoctype+entityresolve+basiccomment+space)^0*optionalspace*endset
 local definitiondoctype=doctypename*somespace*doctypeset
 local publicdoctype=doctypename*somespace*P("PUBLIC")*somespace*value*somespace*value*somespace*doctypeset
 local systemdoctype=doctypename*somespace*P("SYSTEM")*somespace*value*somespace*doctypeset
 local simpledoctype=(anything-close)^1 
 local somedoctype=C((somespace*(
publicentityfile+publicdoctype+systemdoctype+definitiondoctype+simpledoctype)*optionalspace)^0)
 local instruction=(spacing*begininstruction*someinstruction*endinstruction)/function(...) add_special("@pi@",...) end
 local comment=(spacing*begincomment*somecomment*endcomment )/function(...) add_special("@cm@",...) end
 local cdata=(spacing*begincdata*somecdata*endcdata   )/function(...) add_special("@cd@",...) end
 local doctype=(spacing*begindoctype*somedoctype*enddoctype )/function(...) add_special("@dt@",...) end
 local crap_parsed=anything-beginelement-endelement-emptyelement-begininstruction-begincomment-begincdata-ampersand
 local crap_unparsed=anything-beginelement-endelement-emptyelement-begininstruction-begincomment-begincdata
 local parsedcrap=Cs((crap_parsed^1+entity_text)^1)/handle_crap_error
 local parsedcrap=Cs((crap_parsed^1+entity_text)^1)/handle_crap_error
 local unparsedcrap=Cs((crap_unparsed     )^1)/handle_crap_error
 local trailer=space^0*(text_unparsed/set_message)^0
 local grammar_parsed_text_one=P { "preamble",
  preamble=utfbom^0*instruction^0*(doctype+comment+instruction)^0,
 }
 local grammar_parsed_text_two=P { "followup",
  followup=V("parent")*trailer,
  parent=beginelement*V("children")^0*endelement,
  children=parsedtext+V("parent")+emptyelement+comment+cdata+instruction+parsedcrap,
 }
 local grammar_unparsed_text=P { "preamble",
  preamble=utfbom^0*instruction^0*(doctype+comment+instruction)^0*V("parent")*trailer,
  parent=beginelement*V("children")^0*endelement,
  children=unparsedtext+V("parent")+emptyelement+comment+cdata+instruction+unparsedcrap,
 }
 return grammar_parsed_text_one,grammar_parsed_text_two,grammar_unparsed_text
end
local
 grammar_parsed_text_one_nop,
 grammar_parsed_text_two_nop,
 grammar_unparsed_text_nop=install(space,spacing,anything)
local
 grammar_parsed_text_one_yes,
 grammar_parsed_text_two_yes,
 grammar_unparsed_text_yes=install(space_nl,spacing_nl,anything_nl)
local function _xmlconvert_(data,settings,detail)
 settings=settings or {} 
 preparexmlstate(settings)
 if settings.linenumbers then
  grammar_parsed_text_one=grammar_parsed_text_one_yes
  grammar_parsed_text_two=grammar_parsed_text_two_yes
  grammar_unparsed_text=grammar_unparsed_text_yes
 else
  grammar_parsed_text_one=grammar_parsed_text_one_nop
  grammar_parsed_text_two=grammar_parsed_text_two_nop
  grammar_unparsed_text=grammar_unparsed_text_nop
 end
 local preprocessor=settings.preprocessor
 if data and data~="" and type(preprocessor)=="function" then
  data=preprocessor(data,settings) or data 
 end
 if settings.parent_root then
  mt=getmetatable(settings.parent_root)
 else
  initialize_mt(top)
 end
 level=level+1
 stack[level]=top
 top.dt={}
 dt=top.dt
 nt=0
 if not data or data=="" then
  errorstr="empty xml file"
 elseif data==true then
  errorstr=detail or "problematic xml file"
 elseif utfize or resolve then
  local m=lpegmatch(grammar_parsed_text_one,data)
  if m then
   m=lpegmatch(grammar_parsed_text_two,data,m)
  end
  if m then
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
  setmetatable(result,mt)
  setmetatable(result.dt[1],mt)
  setmetatable(stack,mt)
  local errorhandler=settings.error_handler
  if errorhandler==false then
  else
   errorhandler=errorhandler or xml.errorhandler
   if errorhandler then
    local currentresource=settings.currentresource
    if currentresource and currentresource~="" then
     xml.errorhandler(formatters["load error in [%s]: %s"](currentresource,errorstr),currentresource)
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
 else
  errorstr=nil
 end
 result.statistics={
  errormessage=errorstr,
  entities={
   decimals=dcache,
   hexadecimals=hcache,
   names=acache,
   intermediates=parameters,
  }
 }
 preparexmlstate() 
 return result
end
local function xmlconvert(data,settings)
 local ok,result=pcall(function() return _xmlconvert_(data,settings) end)
 if ok then
  return result
 elseif type(result)=="string" then
  return _xmlconvert_(true,settings,result)
 else
  return _xmlconvert_(true,settings)
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
local function copy(old,p)
 if old then
  local new={}
  for k,v in next,old do
   local t=type(v)=="table"
   if k=="at" then
    local t={}
    for k,v in next,v do
     t[k]=v
    end
    new[k]=t
   elseif k=="dt" then
    v.__p__=nil
    v=copy(v,new)
    new[k]=v
    v.__p__=p
   else
    new[k]=v 
   end
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
local f_attribute=formatters['%s=%q']
local function verbose_element(e,handlers,escape) 
 local handle=handlers.handle
 local serialize=handlers.serialize
 local ens,etg,eat,edt,ern=e.ns,e.tg,e.at,e.dt,e.rn
 local ats=eat and next(eat) and {}
 if ats then
  local n=0
  for k in next,eat do
   n=n+1
   ats[n]=k
  end
  if n==1 then
   local k=ats[1]
   ats=f_attribute(k,escaped(eat[k]))
  else
   sort(ats)
   for i=1,n do
    local k=ats[i]
    ats[i]=f_attribute(k,escaped(eat[k]))
   end
   ats=concat(ats," ")
  end
 end
 if ern and trace_entities and ern~=ens then
  ens=ern
 end
 local n=edt and #edt
 if ens~="" then
  if n and n>0 then
   if ats then
    handle("<",ens,":",etg," ",ats,">")
   else
    handle("<",ens,":",etg,">")
   end
   for i=1,n do
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
    handle("<",ens,":",etg," ",ats,"/>")
   else
    handle("<",ens,":",etg,"/>")
   end
  end
 else
  if n and n>0 then
   if ats then
    handle("<",etg," ",ats,">")
   else
    handle("<",etg,">")
   end
   for i=1,n do
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
    handle("<",etg," ",ats,"/>")
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
 handlers.handle("<!DOCTYPE",e.dt[1],">") 
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
 if e then
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
end
local function xserialize(e,handlers)
 if e then
  local functions=handlers.functions
  local etg=e.tg
  if etg then
   (functions[etg] or functions["@el@"])(e,handlers)
  else
   functions["@dc@"](e,handlers)
  end
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
local result,r,threshold={},0,512
local xmlstringhandler=newhandlers {
 name="string",
 initialize=function()
  r=0
  return result
 end,
 finalize=function()
  local done=concat(result,"",1,r)
  r=0
  if r>threshold then
   result={}
  end
  return done
 end,
 handle=function(...)
  for i=1,select("#",...) do
   r=r+1
   result[r]=select(i,...)
  end
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

-- original size: 54626, stripped down to: 31255

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
local trace_lpath=false
local trace_lparse=false
local trace_lprofile=false
local report_lpath=logs.reporter("xml","lpath")
if trackers then
 trackers.register("xml.path",function(v)
  trace_lpath=v
 end)
 trackers.register("xml.parse",function(v)
  trace_lparse=v
 end)
 trackers.register("xml.profile",function(v)
  trace_lpath=v
  trace_lparse=v
  trace_lprofile=v
 end)
end
local xml=xml
local lpathcalls=0  function xml.lpathcalls () return lpathcalls  end
local lpathcached=0  function xml.lpathcached() return lpathcached end
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
 local collected={}
 local c=0
 for l=1,#list do
  local ll=list[l]
  local dt=ll.dt
  if dt then 
   local n=#dt
   if n==0 then
    ll.en=0
   elseif n==1 then
    local dk=dt[1]
    if dk.tg then
     c=c+1
     collected[c]=dk
     dk.ni=1 
     dk.ei=1
     ll.en=1
    end
   else
    local en=0
    for k=1,#dt do
     local dk=dt[k]
     if dk.tg then
      c=c+1
      en=en+1
      collected[c]=dk
      dk.ni=k 
      dk.ei=en
     end
    end
    ll.en=en
   end
  end
 end
 return collected
end
local function collect(list,collected,c)
 local dt=list.dt
 if dt then
  local n=#dt
  if n==0 then
   list.en=0
  elseif n==1 then
   local dk=dt[1]
   if dk.tg then
    c=c+1
    collected[c]=dk
    dk.ni=1 
    dk.ei=1
    c=collect(dk,collected,c)
    list.en=1
   else
    list.en=0
   end
  else
   local en=0
   for k=1,n do
    local dk=dt[k]
    if dk.tg then
     c=c+1
     en=en+1
     collected[c]=dk
     dk.ni=k 
     dk.ei=en
     c=collect(dk,collected,c)
    end
   end
   list.en=en
  end
 end
 return c
end
apply_axis['descendant']=function(list)
 local collected={}
 local c=0
 for l=1,#list do
  c=collect(list[l],collected,c)
 end
 return collected
end
local function collect(list,collected,c)
 local dt=list.dt
 if dt then
  local n=#dt
  if n==0 then
   list.en=0
  elseif n==1 then
   local dk=dt[1]
   if dk.tg then
    c=c+1
    collected[c]=dk
    dk.ni=1 
    dk.ei=1
    c=collect(dk,collected,c)
    list.en=1
   end
  else
   local en=0
   for k=1,#dt do
    local dk=dt[k]
    if dk.tg then
     c=c+1
     en=en+1
     collected[c]=dk
     dk.ni=k 
     dk.ei=en
     c=collect(dk,collected,c)
    end
   end
   list.en=en
  end
 end
 return c
end
apply_axis['descendant-or-self']=function(list)
 local collected={}
 local c=0
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
 local collected={}
 local c=0
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
 local collected={}
 local c=0
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
 local collected={}
 local c=0
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
 local collected={}
 local c=0
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
 local collected={}
 local c=0
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
 local collected={}
 local c=0
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
  local nns=nodes[2]
  local ntg=nodes[3]
  if not nns and not ntg then 
   if directive then
    return list
   else
    return {}
   end
  else
   local collected={}
   local c=0
   local m=0
   local p=nil
   if not nns then 
    for l=1,#list do
     local ll=list[l]
     local ltg=ll.tg
     if ltg then
      if directive then
       if ntg==ltg then
        local llp=ll.__p__;if llp~=p then p=llp;m=1 else m=m+1 end
        c=c+1
        collected[c]=ll
        ll.mi=m
       end
      elseif ntg~=ltg then
       local llp=ll.__p__;if llp~=p then p=llp;m=1 else m=m+1 end
       c=c+1
       collected[c]=ll
       ll.mi=m
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
        local llp=ll.__p__;if llp~=p then p=llp;m=1 else m=m+1 end
        c=c+1
        collected[c]=ll
        ll.mi=m
       end
      elseif lns~=nns then
       local llp=ll.__p__;if llp~=p then p=llp;m=1 else m=m+1 end
       c=c+1
       collected[c]=ll
       ll.mi=m
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
        local llp=ll.__p__;if llp~=p then p=llp;m=1 else m=m+1 end
        c=c+1
        collected[c]=ll
        ll.mi=m
       end
      elseif not ok then
       local llp=ll.__p__;if llp~=p then p=llp;m=1 else m=m+1 end
       c=c+1
       collected[c]=ll
       ll.mi=m
      end
     end
    end
   end
   return collected
  end
 else
  local collected={}
  local c=0
  local m=0
  local p=nil
  for l=1,#list do
   local ll=list[l]
   local ltg=ll.tg
   if ltg then
    local lns=ll.rn or ll.ns
    local ok=false
    for n=1,maxn,3 do
     local nns=nodes[n+1]
     local ntg=nodes[n+2]
     ok=(not ntg or ltg==ntg) and (not nns or lns==nns)
     if ok then
      break
     end
    end
    if directive then
     if ok then
      local llp=ll.__p__;if llp~=p then p=llp;m=1 else m=m+1 end
      c=c+1
      collected[c]=ll
      ll.mi=m
     end
    elseif not ok then
     local llp=ll.__p__;if llp~=p then p=llp;m=1 else m=m+1 end
     c=c+1
     collected[c]=ll
     ll.mi=m
    end
   end
  end
  return collected
 end
end
local quit_expression=false
local function apply_expression(list,expression,order)
 local collected={}
 local c=0
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
local function apply_selector(list,specification)
 if xml.applyselector then
  apply_selector=xml.applyselector
  return apply_selector(list,specification)
 else
  return list
 end
end
local P,V,C,Cs,Cc,Ct,R,S,Cg,Cb=lpeg.P,lpeg.V,lpeg.C,lpeg.Cs,lpeg.Cc,lpeg.Ct,lpeg.R,lpeg.S,lpeg.Cg,lpeg.Cb
local spaces=S(" \n\r\t\f")^0
local lp_space=S(" \n\r\t\f")
local lp_any=P(1)
local lp_noequal=P("!=")/"~="+P("<=")+P(">=")+P("==")
local lp_doequal=P("=")/"=="
local lp_or=P("|")/" or "
local lp_and=P("&")/" and "
local builtin={
 text="(ll.dt[1] or '')",
 content="ll.dt",
 name="((ll.ns~='' and ll.ns..':'..ll.tg) or ll.tg)",
 tag="ll.tg",
 position="l",
 firstindex="1",
 firstelement="1",
 first="1",
 lastindex="(#ll.__p__.dt or 1)",
 lastelement="(ll.__p__.en or 1)",
 last="#list",
 list="list",
 self="ll",
 rootposition="order",
 order="order",
 element="(ll.ei or 1)",
 index="(ll.ni or 1)",
 match="(ll.mi or 1)",
 namespace="ll.ns",
 ns="ll.ns",
}
local lp_builtin=lpeg.utfchartabletopattern(builtin)/builtin*((spaces*P("(")*spaces*P(")"))/"")
local lp_attribute=(P("@")+P("attribute::"))/""*Cc("(ll.at and ll.at['")*((R("az","AZ")+S("-_:"))^1)*Cc("'])")
local lp_fastpos_p=P("+")^0*R("09")^1*P(-1)/"l==%0"
local lp_fastpos_n=P("-")*R("09")^1*P(-1)/"(%0<0 and (#list+%0==l))"
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
local lp_child=Cc("expr.child(ll,'")*R("az","AZ")*R("az","AZ","--","__")^0*Cc("')")
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
local register_last_match={ kind="axis",axis="last-match"     } 
local register_self={ kind="axis",axis="self"     } 
local register_parent={ kind="axis",axis="parent"      } 
local register_descendant={ kind="axis",axis="descendant"     } 
local register_child={ kind="axis",axis="child"       } 
local register_descendant_or_self={ kind="axis",axis="descendant-or-self"   } 
local register_root={ kind="axis",axis="root"     } 
local register_ancestor={ kind="axis",axis="ancestor"    } 
local register_ancestor_or_self={ kind="axis",axis="ancestor-or-self"  } 
local register_attribute={ kind="axis",axis="attribute"      } 
local register_namespace={ kind="axis",axis="namespace"      } 
local register_following={ kind="axis",axis="following"      } 
local register_following_sibling={ kind="axis",axis="following-sibling"    } 
local register_preceding={ kind="axis",axis="preceding"      } 
local register_preceding_sibling={ kind="axis",axis="preceding-sibling"    } 
local register_reverse_sibling={ kind="axis",axis="reverse-sibling"   } 
local register_auto_descendant_or_self={ kind="axis",axis="auto-descendant-or-self" } 
local register_auto_descendant={ kind="axis",axis="auto-descendant"   } 
local register_auto_self={ kind="axis",axis="auto-self"      } 
local register_auto_child={ kind="axis",axis="auto-child"     } 
local register_initial_child={ kind="axis",axis="initial-child"     } 
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
local function register_selector(specification)
 return { kind="selector",specification=specification }
end
local function register_expression(expression)
 local converted=lpegmatch(converter,expression)
 local wrapped=format(template_e,converted)
 local runner=load(wrapped)
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
         (V("special")*spaces*P(-1)               )+(V("initial")*spaces*V("step")*spaces*(P("/")*spaces*V("step")*spaces)^0 )
         ),
 protocol=Cg(V("letters"),"protocol")*P("://")+Cg(Cc(nil),"protocol"),
 step=((V("shortcuts")+V("selector")+P("/")+V("axis"))*spaces*V("nodes")^0+V("error"))*spaces*V("expressions")^0*spaces*V("finalizer")^0,
 axis=V("last_match")+V("descendant")+V("child")+V("parent")+V("self")+V("root")+V("ancestor")+V("descendant_or_self")+V("following_sibling")+V("following")+V("reverse_sibling")+V("preceding_sibling")+V("preceding")+V("ancestor_or_self")+#(1-P(-1))*Cc(register_auto_child),
 special=special_1+special_2+special_3,
 initial=(P("/")*spaces*Cc(register_initial_child))^-1,
 error=(P(1)^1)/register_error,
 shortcuts_a=V("s_descendant_or_self")+V("s_descendant")+V("s_child")+V("s_parent")+V("s_self")+V("s_root")+V("s_ancestor")+V("s_lastmatch"),
 shortcuts=V("shortcuts_a")*(spaces*"/"*spaces*V("shortcuts_a"))^0,
 s_descendant_or_self=(P("***/")+P("/"))*Cc(register_descendant_or_self),
 s_descendant=P("**")*Cc(register_descendant),
 s_child=P("*")*no_nextcolon*Cc(register_child),
 s_parent=P("..")*Cc(register_parent),
 s_self=P("." )*Cc(register_self),
 s_root=P("^^")*Cc(register_root),
 s_ancestor=P("^")*Cc(register_ancestor),
 s_lastmatch=P("=")*Cc(register_last_match),
 descendant=P("descendant::")*Cc(register_descendant),
 child=P("child::")*Cc(register_child),
 parent=P("parent::")*Cc(register_parent),
 self=P("self::")*Cc(register_self),
 root=P('root::')*Cc(register_root),
 ancestor=P('ancestor::')*Cc(register_ancestor),
 descendant_or_self=P('descendant-or-self::')*Cc(register_descendant_or_self),
 ancestor_or_self=P('ancestor-or-self::')*Cc(register_ancestor_or_self),
 following=P('following::')*Cc(register_following),
 following_sibling=P('following-sibling::')*Cc(register_following_sibling),
 preceding=P('preceding::')*Cc(register_preceding),
 preceding_sibling=P('preceding-sibling::')*Cc(register_preceding_sibling),
 reverse_sibling=P('reverse-sibling::')*Cc(register_reverse_sibling),
 last_match=P('last-match::')*Cc(register_last_match),
 selector=P("{")*C((1-P("}"))^1)*P("}")/register_selector,
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
  t[#t+1]=(directive and tg) or format("not(%s)",tg)
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
   local ns=li.ns
   local tg=li.tg
   if not ns or ns=="" then ns="*" end
   if not tg or tg=="" then tg="*" end
   t[i]=(tg=="@rt@" and "[root]") or format("%s:%s",ns,tg)
  end
  return concat(t," ")
 end
end
xml.nodesettostring=nodesettostring
local lpath 
local function lshow(parsed)
 if type(parsed)=="string" then
  parsed=lpath(parsed)
 end
 report_lpath("%s://%s => %s",parsed.protocol or xml.defaultprotocol,parsed.pattern,
  table.serialize(parsed,false))
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
do
 local profiled={}
 xml.profiled=profiled
 local lastmatch=nil  
 local keepmatch=nil  
 if directives then
  directives.register("xml.path.keeplastmatch",function(v)
   keepmatch=v
   lastmatch=nil
  end)
 end
 apply_axis["last-match"]=function()
  return lastmatch or {}
 end
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
   elseif kind=="selector" then
    collected=apply_selector(collected,pi.specification)
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
   elseif kind=="selector" then
    collected=apply_selector(collected,pi.specification)
    report_lpath("% 10i : se : %s ",(collected and #collected) or 0,pi.specification)
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
   elseif kind=="selector" then
    collected=apply_selector(collected,pi.specification)
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
 local apply=normal_apply
 if trackers then
  trackers.register("xml.path,xml.parse,xml.profile",function()
   if trace_lprofile then
    apply=profiled_apply
   elseif trace_lpath then
    apply=traced_apply
   else
    apply=normal_apply
   end
  end)
 end
 function xml.applylpath(list,pattern)
  if not list then
   lastmatch=nil
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
   lastmatch=nil
   return
  end
  local nofparsed=#parsed
  if nofparsed==0 then
   lastmatch=nil
   return 
  end
  local collected=apply({ list },parsed,nofparsed,list.mi)
  lastmatch=keepmatch and collected or nil
  return collected
 end
 function xml.lastmatch()
  return lastmatch
 end
 local stack={}
 function xml.pushmatch()
  insert(stack,lastmatch)
 end
 function xml.popmatch()
  lastmatch=remove(stack)
 end
end
local applylpath=xml.applylpath
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
expressions.attribute=function(e,name,value)
 if type(e)=="table" and name then
  local a=e.at
  if a then
   local v=a[name]
   if value then
    return v==value
   else
    return v
   end
  end
 end
 return nil
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
function expressions.idstring(str)
 return type(str)=="string" and gsub(str,"^#","") or ""
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
  local d=e.__p__.dt
  local k=e.ni
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
  local d=e.__p__.dt
  local k=e.ni
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
  local ns=found.rn or found.ns or ""
  local tg=found.tg
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
   local d=e.__p__.dt
   local k=e.ni
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
   local d=e.__p__.dt
   local k=e.ni
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
 end
 local n=#collected
 if n==0 then
  return dummy
 end
 if reverse then
  local c=n+1
  return function()
   if c>1 then
    c=c-1
    local e=collected[c]
    local r=e.__p__
    return r,r.dt,e.ni
   end
  end
 else
  local c=0
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
 end
 local n=#collected
 if n==0 then
  return dummy
 end
 if reverse then
  local c=n+1
  return function()
   if c>1 then
    c=c-1
    return collected[c]
   end
  end
 else
  local c=0
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

-- original size: 3574, stripped down to: 1808

if not modules then modules={} end modules ['lxml-mis']={
 version=1.001,
 comment="this module is the basis for the lxml-* ones",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local xml,lpeg,string=xml,lpeg,string
local type=type
local concat=table.concat
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
function xml.escaped  (str) return lpegmatch(escaped,str)   end
function xml.unescaped(str) return lpegmatch(unescaped,str) end
function xml.cleansed (str) return lpegmatch(cleansed,str)  end
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

-- original size: 30771, stripped down to: 19680

if not modules then modules={} end modules ['lxml-aux']={
 version=1.001,
 comment="this module is the basis for the lxml-* ones",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local trace_manipulations=false  trackers.register("lxml.manipulations",function(v) trace_manipulations=v end)
local trace_inclusions=false  trackers.register("lxml.inclusions",function(v) trace_inclusions=v end)
local report_xml=logs.reporter("xml")
local xml=xml
local xmlcopy,xmlname=xml.copy,xml.name
local xmlinheritedconvert=xml.inheritedconvert
local xmlapplylpath=xml.applylpath
local type,next,setmetatable,getmetatable=type,next,setmetatable,getmetatable
local insert,remove,fastcopy,concat=table.insert,table.remove,table.fastcopy,table.concat
local gmatch,gsub,format,find,strip=string.gmatch,string.gsub,string.format,string.find,string.strip
local utfbyte=utf.byte
local lpegmatch,lpegpatterns=lpeg.match,lpeg.patterns
local striplinepatterns=utilities.strings.striplinepatterns
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
  if handle then
   if reverse then
    for c=#collected,1,-1 do
     handle(collected[c])
    end
   else
    for c=1,#collected do
     handle(collected[c])
    end
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
  local t={}
  local n=0
  for c=1,#collected do
   local e=collected[c]
   local ns=e.ns
   local tg=e.tg
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
xml.reindex=redo_ni
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
     local ni=e.ni
     if ni<=#d then
      if false then
       p.dt[ni]=""
      else
       remove(d,ni)
       redo_ni(d) 
      end
     else
     end
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
    local n=e.ni
    local t=copiedelement(element,p)
    if type(t)=="table" then
     d[n]=t[1]
     for i=2,#t do
      n=n+1
      insert(d,n,t[i])
     end
    else
     d[n]=t
    end
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
  local d=r.dt
  local k=e.ni
  local rri=r.ri
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
  local d=r.dt
  local k=e.ni
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
local function include(xmldata,pattern,attribute,recursive,loaddata,level)
 pattern=pattern or 'include'
 loaddata=loaddata or io.loaddata
 local collected=xmlapplylpath(xmldata,pattern)
 if collected then
  if not level then
   level=1
  end
  for c=1,#collected do
   local ek=collected[c]
   local name=nil
   local ekdt=ek.dt
   if ekdt then
    local ekat=ek.at
    local ekrt=ek.__p__
    if ekrt then
     local epdt=ekrt.dt
     if not attribute or attribute=="" then
      name=(type(ekdt)=="table" and ekdt[1]) or ekdt 
     end
     if not name then
      for a in gmatch(attribute or "href","([^|]+)") do
       name=ekat[a]
       if name then
        break
       end
      end
     end
     local data=nil
     if name and name~="" then
      local d,n=loaddata(name)
      data=d or ""
      name=n or name
      if trace_inclusions then
       report_xml("including %s bytes from %a at level %s by pattern %a and attribute %a (%srecursing)",#data,name,level,pattern,attribute or "",recursive and "" or "not ")
      end
     end
     if not data or data=="" then
      epdt[ek.ni]="" 
     elseif ekat["parse"]=="text" then
      epdt[ek.ni]=xml.escaped(data) 
     else
local settings=xmldata.settings
local savedresource=settings.currentresource
settings.currentresource=name
      local xi=xmlinheritedconvert(data,xmldata)
      if not xi then
       epdt[ek.ni]="" 
      else
       if recursive then
        include(xi,pattern,attribute,recursive,loaddata,level+1)
       end
       local child=xml.body(xi) 
       child.__p__=ekrt
       child.__f__=name 
child.cf=name
       epdt[ek.ni]=child
       local settings=xmldata.settings
       local inclusions=settings and settings.inclusions
       if inclusions then
        inclusions[#inclusions+1]=name
       elseif settings then
        settings.inclusions={ name }
       else
        settings={ inclusions={ name } }
        xmldata.settings=settings
       end
       if child.er then
        local badinclusions=settings.badinclusions
        if badinclusions then
         badinclusions[#badinclusions+1]=name
        else
         settings.badinclusions={ name }
        end
       end
      end
settings.currentresource=savedresource
     end
    end
   end
  end
 end
end
xml.include=include
function xml.inclusion(e,default)
 while e do
  local f=e.__f__
  if f then
   return f
  else
   e=e.__p__
  end
 end
 return default
end
local function getinclusions(key,e,sorted)
 while e do
  local settings=e.settings
  if settings then
   local inclusions=settings[key]
   if inclusions then
    inclusions=table.unique(inclusions) 
    if sorted then
     table.sort(inclusions) 
    end
    return inclusions 
   else
    e=e.__p__
   end
  else
   e=e.__p__
  end
 end
end
function xml.inclusions(e,sorted)
 return getinclusions("inclusions",e,sorted)
end
function xml.badinclusions(e,sorted)
 return getinclusions("badinclusions",e,sorted)
end
local b_collapser=lpegpatterns.b_collapser
local m_collapser=lpegpatterns.m_collapser
local e_collapser=lpegpatterns.e_collapser
local b_stripper=lpegpatterns.b_stripper
local m_stripper=lpegpatterns.m_stripper
local e_stripper=lpegpatterns.e_stripper
local function stripelement(e,nolines,anywhere)
 local edt=e.dt
 if edt then
  local n=#edt
  if n==0 then
   return e 
  elseif anywhere then
   local t={}
   local m=0
   for e=1,n do
    local str=edt[e]
    if type(str)~="string" then
     m=m+1
     t[m]=str
    elseif str~="" then
     if nolines then
      str=lpegmatch((n==1 and b_collapser) or (n==m and e_collapser) or m_collapser,str)
     else
      str=lpegmatch((n==1 and b_stripper) or (n==m and e_stripper) or m_stripper,str)
     end
     if str~="" then
      m=m+1
      t[m]=str
     end
    end
   end
   e.dt=t
  else
   local str=edt[1]
   if type(str)=="string" then
    if str~="" then
     str=lpegmatch(nolines and b_collapser or b_stripper,str)
    end
    if str=="" then
     remove(edt,1)
     n=n-1
    else
     edt[1]=str
    end
   end
   if n>0 then
    str=edt[n]
    if type(str)=="string" then
     if str=="" then
      remove(edt)
     else
      str=lpegmatch(nolines and e_collapser or e_stripper,str)
      if str=="" then
       remove(edt)
      else
       edt[n]=str
      end
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
xml.strip_whitespace=xml.strip     obsolete.strip_whitespace=xml.strip
xml.collect_elements=xml.collect      obsolete.collect_elements=xml.collect
xml.delete_element=xml.delete    obsolete.delete_element=xml.delete
xml.replace_element=xml.replace      obsolete.replace_element=xml.replace
xml.each_element=xml.each      obsolete.each_element=xml.each
xml.process_elements=xml.process      obsolete.process_elements=xml.process
xml.insert_element_after=xml.insertafter     obsolete.insert_element_after=xml.insertafter
xml.insert_element_before=xml.insertbefore    obsolete.insert_element_before=xml.insertbefore
xml.inject_element_after=xml.injectafter     obsolete.inject_element_after=xml.injectafter
xml.inject_element_before=xml.injectbefore    obsolete.inject_element_before=xml.injectbefore
xml.process_attributes=xml.processattributes  obsolete.process_attributes=xml.processattributes
xml.collect_texts=xml.collecttexts    obsolete.collect_texts=xml.collecttexts
xml.inject_element=xml.inject    obsolete.inject_element=xml.inject
xml.remap_tag=xml.remaptag     obsolete.remap_tag=xml.remaptag
xml.remap_name=xml.remapname    obsolete.remap_name=xml.remapname
xml.remap_namespace=xml.remapnamespace  obsolete.remap_namespace=xml.remapnamespace
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
 insert(e.dt,n or 1,{
  tg="@cm@",
  ns="",
  special=true,
  at={},
  dt={ str },
 })
end
function xml.insertcdata(e,str,n)
 insert(e.dt,n or 1,{
  tg="@cd@",
  ns="",
  special=true,
  at={},
  dt={ str },
 })
end
function xml.setcomment(e,str,n)
 e.dt={ {
  tg="@cm@",
  ns="",
  special=true,
  at={},
  dt={ str },
 } }
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
   local t={ "\n" }
   local n=1
   local i=1
   local nd=#d
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
    recurse(str,action) 
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
local specials={
 ["@rt@"]="root",
 ["@pi@"]="instruction",
 ["@cm@"]="comment",
 ["@dt@"]="declaration",
 ["@cd@"]="cdata",
}
local function convert(x,strip,flat)
 local ns=x.ns
 local tg=x.tg
 local at=x.at
 local dt=x.dt
 local node=flat and {
  [0]=(not x.special and (ns~="" and ns..":"..tg or tg)) or nil,
 } or {
  _namespace=ns~="" and ns or nil,
  _tag=not x.special and tg or nil,
  _type=specials[tg] or "_element",
 }
 if at then
  for k,v in next,at do
   node[k]=v
  end
 end
 local n=0
 for i=1,#dt do
  local di=dt[i]
  if type(di)=="table" then
   if flat and di.special then
   else
    di=convert(di,strip,flat)
    if di then
     n=n+1
     node[n]=di
    end
   end
  elseif strip then
   di=lpegmatch(strip,di)
   if di~="" then
    n=n+1
    node[n]=di
   end
  else
   n=n+1
   node[n]=di
  end
 end
 if next(node) then
  return node
 end
end
function xml.totable(x,strip,flat)
 if type(x)=="table" then
  if strip then
   strip=striplinepatterns[strip]
  end
  return convert(x,strip,flat)
 end
end
function xml.rename(e,namespace,name,attributes)
 if type(e)~="table" or not e.tg then
  return
 end
 if type(name)=="table" then
  attributes=name
  name=namespace
  namespace=""
 elseif type(name)~="string" then
  attributes={}
  name=namespace
  namespace=""
 end
 if type(attributes)~="table" then
  attributes={}
 end
 e.ns=namespace
 e.rn=namespace
 e.tg=name
 e.at=attributes
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["lxml-xml"] = package.loaded["lxml-xml"] or true

-- original size: 11096, stripped down to: 7702

if not modules then modules={} end modules ['lxml-xml']={
 version=1.001,
 comment="this module is the basis for the lxml-* ones",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local tonumber,next=tonumber,next
local concat=table.concat
local find,lower,upper=string.find,string.lower,string.upper
local xml=xml
local finalizers=xml.finalizers.xml
local xmlfilter=xml.filter 
local xmltostring=xml.tostring
local xmlserialize=xml.serialize
local xmlcollected=xml.collected
local xmlnewhandlers=xml.newhandlers
local reparsedentity=xml.reparsedentitylpeg   
local unescapedentity=xml.unescapedentitylpeg
local parsedentity=reparsedentity
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
function xml.serializetotext(root)
 return root and xmlserialize(root,xmltexthandler) or ""
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
function xml.pure(id,pattern)
 if pattern then
  local collected=xmlfilter(id,pattern)
  if collected and #collected>0 then
   parsedentity=unescapedentity
   local s=collected and #collected>0 and xmltotext(collected[1]) or ""
   parsedentity=reparsedentity
   return s
  else
   return ""
  end
 else
  parsedentity=unescapedentity
  local s=xmltotext(id) or ""
  parsedentity=reparsedentity
  return s
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

-- original size: 6407, stripped down to: 4640

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
     dir.mkdirs(file.pathpart(fullname))
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

-- original size: 10847, stripped down to: 7086

if not modules then modules={} end modules ['data-ini']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files",
}
local next,type,getmetatable,rawset=next,type,getmetatable,rawset
local gsub,find,gmatch,char=string.gsub,string.find,string.gmatch,string.char
local filedirname,filebasename,filejoin=file.dirname,file.basename,file.join
local ostype,osname,osuname,ossetenv,osgetenv=os.type,os.name,os.uname,os.setenv,os.getenv
local sortedpairs=table.sortedpairs
local P,S,R,C,Cs,Cc,lpegmatch=lpeg.P,lpeg.S,lpeg.R,lpeg.C,lpeg.Cs,lpeg.Cc,lpeg.match
local trace_locating=false  trackers.register("resolvers.locating",function(v) trace_locating=v end)
local trace_expansions=false  trackers.register("resolvers.expansions",function(v) trace_expansions=v end)
local report_initialization=logs.reporter("resolvers","initialization")
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
 local ownbin=environment.ownbin  or args[-2] or arg[-2] or args[-1] or arg[-1] or arg[0] or "luatex"
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
local texos=environment.texos   or osgetenv("TEXOS")
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
local prefixes=utilities.storage.allocate()
resolvers.prefixes=prefixes
local resolved={}
local abstract={}
local dynamic={}
function resolvers.resetresolve(str)
 resolved,abstract={},{}
end
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
function resolvers.unresolve(str)
 return abstract[str] or str
end
function resolvers.setdynamic(str)
 dynamic[str]=true
end
local pattern=Cs((C(R("az")^2)*P(":")*C((1-S(" \"\';,"))^1)/_resolve_+P(1))^0)
local prefix=C(R("az")^2)*P(":")
local target=C((1-S(" \"\';,"))^1)
local notarget=(#S(";,")+P(-1))*Cc("")
local p_resolve=Cs(((prefix*(target+notarget))/_resolve_+P(1))^0)
local p_simple=prefix*P(-1)
local function resolve(str) 
 if type(str)=="table" then
  local res={}
  for i=1,#str do
   res[i]=resolve(str[i])
  end
  return res
 end
 local res=resolved[str]
 if res then
  return res
 end
 local simple=lpegmatch(p_simple,str)
 local action=prefixes[simple]
 if action then
  local res=action(res)
  if not dynamic[simple] then
   resolved[simple]=res
   abstract[res]=simple
  end
  return res
 end
 res=lpegmatch(p_resolve,str)
 resolved[str]=res
 abstract[res]=str
 return res
end
resolvers.resolve=resolve
if type(osuname)=="function" then
 for k,v in next,osuname() do
  if not prefixes[k] then
   prefixes[k]=function() return v end
  end
 end
end
if ostype=="unix" then
 local pattern
 local function makepattern(t,k,v)
  if t then
   rawset(t,k,v)
  end
  local colon=P(":")
  for k,v in sortedpairs(prefixes) do
   if p then
    p=P(k)+p
   else
    p=P(k)
   end
  end
  pattern=Cs((p*colon+colon/";"+P(1))^0)
 end
 makepattern()
 table.setmetatablenewindex(prefixes,makepattern)
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

package.loaded["data-exp"] = package.loaded["data-exp"] or true

-- original size: 18179, stripped down to: 10432

if not modules then modules={} end modules ['data-exp']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files",
}
local format,find,gmatch,lower,char,sub=string.format,string.find,string.gmatch,string.lower,string.char,string.sub
local concat,sort=table.concat,table.sort
local sortedkeys=table.sortedkeys
local lpegmatch,lpegpatterns=lpeg.match,lpeg.patterns
local Ct,Cs,Cc,Carg,P,C,S=lpeg.Ct,lpeg.Cs,lpeg.Cc,lpeg.Carg,lpeg.P,lpeg.C,lpeg.S
local type,next=type,next
local isdir=lfs.isdir
local collapsepath,joinpath,basename=file.collapsepath,file.join,file.basename
local trace_locating=false  trackers.register("resolvers.locating",function(v) trace_locating=v end)
local trace_expansions=false  trackers.register("resolvers.expansions",function(v) trace_expansions=v end)
local trace_globbing=true   trackers.register("resolvers.globbing",function(v) trace_globbing=v end)
local report_expansions=logs.reporter("resolvers","expansions")
local report_globbing=logs.reporter("resolvers","globbing")
local resolvers=resolvers
local resolveprefix=resolvers.resolve
local function f_both(a,b)
 local t,n={},0
 for sb in gmatch(b,"[^,]+") do     
  for sa in gmatch(a,"[^,]+") do    
   n=n+1;t[n]=sa..sb
  end
 end
 return concat(t,",")
end
local comma=P(",")
local nocomma=(1-comma)^1
local docomma=comma^1/","
local before=Cs((nocomma*Carg(1)+docomma)^0)
local after=Cs((Carg(1)*nocomma+docomma)^0)
local both=Cs(((C(nocomma)*Carg(1))/function(a,b) return lpegmatch(before,b,1,a) end+docomma)^0)
local function f_first (a,b) return lpegmatch(after,b,1,a) end
local function f_second(a,b) return lpegmatch(before,a,1,b) end
local function f_both  (a,b) return lpegmatch(both,b,1,a) end
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
local usedhomedir=nil
local donegation=(P("!")/""  )^0
local doslashes=(P("\\")/"/"+1)^0
local function expandedhome()
 if not usedhomedir then
  usedhomedir=lpegmatch(Cs(donegation*doslashes),environment.homedir or "")
  if usedhomedir=="~" or usedhomedir=="" or not isdir(usedhomedir) then
   if trace_expansions then
    report_expansions("no home dir set, ignoring dependent path using current path")
   end
   usedhomedir="."
  end
 end
 return usedhomedir
end
local dohome=((P("~")+P("$HOME")+P("%HOME%"))/expandedhome)^0
local cleanup=Cs(donegation*dohome*doslashes)
resolvers.cleanpath=function(str)
 return str and lpegmatch(cleanup,str) or ""
end
local expandhome=P("~")/"$HOME"
local dodouble=P('"')/""*(expandhome+(1-P('"')))^0*P('"')/""
local dosingle=P("'")/""*(expandhome+(1-P("'")))^0*P("'")/""
local dostring=(expandhome+1     )^0
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
  return joinpath(str)
 else
  return str
 end
end
local attributes,directory=lfs.attributes,lfs.dir
local weird=P(".")^1+lpeg.anywhere(S("~`!#$%^&*()={}[]:;\"\'||<>,?\n\r\t"))
local lessweird=P(".")^1+lpeg.anywhere(S("~`#$%^&*:;\"\'||<>,?\n\r\t"))
local timer={}
local scanned={}
local nofscans=0
local scancache={}
local fullcache={}
local nofsharedscans=0
local addcasecraptoo=true
local function scan(files,remap,spec,path,n,m,r,onlyone,tolerant)
 local full=path=="" and spec or (spec..path..'/')
 local dirlist={}
 local nofdirs=0
 local pattern=tolerant and lessweird or weird
 local filelist={}
 local noffiles=0
 for name,mode in directory(full) do
  if not lpegmatch(pattern,name) then
   if not mode then
    mode=attributes(full..name,"mode")
   end
   if mode=="file" then
    n=n+1
    noffiles=noffiles+1
    filelist[noffiles]=name
   elseif mode=="directory" then
    m=m+1
    nofdirs=nofdirs+1
    if path~="" then
     dirlist[nofdirs]=path.."/"..name
    else
     dirlist[nofdirs]=name
    end
   end
  end
 end
 if noffiles>0 then
  sort(filelist)
  for i=1,noffiles do
   local name=filelist[i]
   local lower=lower(name)
   local paths=files[lower]
   if paths then
    if onlyone then
    else
     if name~=lower then
      local rl=remap[lower]
      if not rl then
       remap[lower]=name
       r=r+1
      elseif trace_globbing and rl~=name then
       report_globbing("confusing filename, name: %a, lower: %a, already: %a",name,lower,rl)
      end
      if addcasecraptoo then
       local paths=files[name]
       if not paths then
        files[name]=path
       elseif type(paths)=="string" then
        files[name]={ paths,path }
       else
        paths[#paths+1]=path
       end
      end
     end
     if type(paths)=="string" then
      files[lower]={ paths,path }
     else
      paths[#paths+1]=path
     end
    end
   else 
    files[lower]=path
    if name~=lower then
     local rl=remap[lower]
     if not rl then
      remap[lower]=name
      r=r+1
     elseif trace_globbing and rl~=name then
      report_globbing("confusing filename, name: %a, lower: %a, already: %a",name,lower,rl)
     end
    end
   end
  end
 end
 if nofdirs>0 then
  sort(dirlist)
  for i=1,nofdirs do
   files,remap,n,m,r=scan(files,remap,spec,dirlist[i],n,m,r,onlyonce,tolerant)
  end
 end
 scancache[sub(full,1,-2)]=files
 return files,remap,n,m,r
end
local function scanfiles(path,branch,usecache,onlyonce,tolerant)
 local realpath=resolveprefix(path)
 if usecache then
  local content=fullcache[realpath]
  if content then
   if trace_locating then
    report_expansions("using cached scan of path %a, branch %a",path,branch or path)
   end
   nofsharedscans=nofsharedscans+1
   return content
  end
 end
 statistics.starttiming(timer)
 if trace_locating then
  report_expansions("scanning path %a, branch %a",path,branch or path)
 end
 local content
 if isdir(realpath) then
  local files,remap,n,m,r=scan({},{},realpath..'/',"",0,0,0,onlyonce,tolerant)
  content={
   metadata={
    path=path,
    files=n,
    directories=m,
    remappings=r,
   },
   files=files,
   remap=remap,
  }
  if trace_locating then
   report_expansions("%s files found on %s directories with %s uppercase remappings",n,m,r)
  end
 else
  content={
   metadata={
    path=path,
    files=0,
    directories=0,
    remappings=0,
   },
   files={},
   remap={},
  }
  if trace_locating then
   report_expansions("invalid path %a",realpath)
  end
 end
 if usecache then
  scanned[#scanned+1]=realpath
  fullcache[realpath]=content
 end
 nofscans=nofscans+1
 statistics.stoptiming(timer)
 return content
end
resolvers.scanfiles=scanfiles
function resolvers.simplescanfiles(path,branch,usecache)
 return scanfiles(path,branch,usecache,true,true) 
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
function resolvers.get_from_content(content,path,name) 
 if not content then
  return
 end
 local files=content.files
 if not files then
  return
 end
 local remap=content.remap
 if not remap then
  return
 end
 if name then
  local used=lower(name)
  return path,remap[used] or used
 else
  local name=path
  local used=lower(name)
  local path=files[used]
  if path then
   return path,remap[used] or used
  end
 end
end
local nothing=function() end
function resolvers.filtered_from_content(content,pattern)
 if content and type(pattern)=="string" then
  local pattern=lower(pattern)
  local files=content.files 
  local remap=content.remap
  if files and remap then
   local f=sortedkeys(files)
   local n=#f
   local i=0
   local function iterator()
    while i<n do
     i=i+1
     local k=f[i]
     if find(k,pattern) then
      return files[k],remap and remap[k] or k
     end
    end
   end
   return iterator
  end
 end
 return nothing
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-env"] = package.loaded["data-env"] or true

-- original size: 9400, stripped down to: 6347

if not modules then modules={} end modules ['data-env']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files",
}
local lower,gsub=string.lower,string.gsub
local next,rawget=next,rawget
local resolvers=resolvers
local allocate=utilities.storage.allocate
local setmetatableindex=table.setmetatableindex
local suffixonly=file.suffixonly
local formats=allocate()
local suffixes=allocate()
local dangerous=allocate()
local suffixmap=allocate()
local usertypes=allocate()
resolvers.formats=formats
resolvers.suffixes=suffixes
resolvers.dangerous=dangerous
resolvers.suffixmap=suffixmap
resolvers.usertypes=usertypes
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
   suffixes={ 'mp','mpvi','mpiv','mpxl','mpii' },
   usertype=true,
  },
  tex={
   names={ "tex" },
   variable='TEXINPUTS',
   suffixes={ "tex","mkiv","mkvi","mkxl","mklx","mkii","cld","lfg","xml" },
   usertype=true,
  },
  icc={
   names={ "icc","icc profile","icc profiles" },
   variable='ICCPROFILES',
   suffixes={ 'icc' },
  },
  texmfscripts={
   names={ "texmfscript","texmfscripts","script","scripts" },
   variable='TEXMFSCRIPTS',
   suffixes={ 'lua','rb','pl','py' },
  },
  lua={
   names={ "lua" },
   variable='LUAINPUTS',
   suffixes={ luasuffixes.lua,luasuffixes.luc,luasuffixes.tma,luasuffixes.tmc },
   usertype=true,
  },
  lib={
   names={ "lib" },
   variable='CLUAINPUTS',
   suffixes=os.libsuffix and { os.libsuffix } or { 'dll','so' },
  },
  bib={
   names={ 'bib' },
   variable='BIBINPUTS',
   suffixes={ 'bib' },
   usertype=true,
  },
  bst={
   names={ 'bst' },
   variable='BSTINPUTS',
   suffixes={ 'bst' },
   usertype=true,
  },
  fontconfig={
   names={ 'fontconfig','fontconfig file','fontconfig files' },
   variable='FONTCONFIG_PATH',
  },
  pk={
   names={ "pk" },
   variable='PKFONTS',
   suffixes={ 'pk' },
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
   if rn and rv then
    local rs=relation.suffixes
    local ru=relation.usertype
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
    if ru then
     usertypes[name]=true
    end
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

-- original size: 16099, stripped down to: 11379

if not modules then modules={} end modules ['data-tmp']={
 version=1.100,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local next,type=next,type
local pcall,loadfile,collectgarbage=pcall,loadfile,collectgarbage
local format,lower,gsub=string.format,string.lower,string.gsub
local concat,serialize,fastserialize,serializetofile=table.concat,table.serialize,table.fastserialize,table.tofile
local mkdirs,expanddirname,isdir,isfile=dir.mkdirs,dir.expandname,lfs.isdir,lfs.isfile
local is_writable,is_readable=file.is_writable,file.is_readable
local collapsepath,joinfile,addsuffix,dirname=file.collapsepath,file.join,file.addsuffix,file.dirname
local savedata=file.savedata
local formatters=string.formatters
local osexit,osdate,osuuid=os.exit,os.date,os.uuid
local removefile=os.remove
local md5hex=md5.hex
local trace_locating=false  trackers.register("resolvers.locating",function(v) trace_locating=v end)
local trace_cache=false  trackers.register("resolvers.cache",function(v) trace_cache=v end)
local report_caches=logs.reporter("resolvers","caches")
local report_resolvers=logs.reporter("resolvers","caching")
local resolvers=resolvers
local cleanpath=resolvers.cleanpath
local resolvepath=resolvers.resolve
local luautilities=utilities.lua
do
 local directive_cleanup=false  directives.register("system.compile.cleanup",function(v) directive_cleanup=v end)
 local directive_strip=false  directives.register("system.compile.strip",function(v) directive_strip=v end)
 local compilelua=luautilities.compile
 function luautilities.compile(luafile,lucfile,cleanup,strip)
  if cleanup==nil then cleanup=directive_cleanup end
  if strip==nil then strip=directive_strip   end
  return compilelua(luafile,lucfile,cleanup,strip)
 end
end
caches=caches or {}
local caches=caches
local writable=nil
local readables={}
local usedreadables={}
local compilelua=luautilities.compile
local luasuffixes=luautilities.suffixes
caches.base=caches.base or "luatex-cache"  
caches.more=caches.more or "context"    
caches.defaults={ "TMPDIR","TEMPDIR","TMP","TEMP","HOME","HOMEPATH" }
local direct_cache=false 
local fast_cache=false
local cache_tree=false
directives.register("system.caches.direct",function(v) direct_cache=true end)
directives.register("system.caches.fast",function(v) fast_cache=true end)
local function configfiles()
 return concat(resolvers.configurationfiles(),";")
end
local function hashed(tree)
 tree=gsub(tree,"[\\/]+$","")
 tree=lower(tree)
 local hash=md5hex(tree)
 if trace_cache or trace_locating then
  report_caches("hashing tree %a, hash %a",tree,hash)
 end
 return hash
end
local function treehash()
 local tree=configfiles()
 if not tree or tree=="" then
  return false
 else
  return hashed(tree)
 end
end
caches.hashed=hashed
caches.treehash=treehash
caches.configfiles=configfiles
local function identify()
 local texmfcaches=resolvers.cleanpathlist("TEXMFCACHE") 
 if texmfcaches then
  for k=1,#texmfcaches do
   local cachepath=texmfcaches[k]
   if cachepath~="" then
    cachepath=resolvepath(cachepath)
    cachepath=cleanpath(cachepath)
    cachepath=collapsepath(cachepath)
    local valid=isdir(cachepath)
    if valid then
     if is_readable(cachepath) then
      readables[#readables+1]=cachepath
      if not writable and is_writable(cachepath) then
       writable=cachepath
      end
     end
    elseif not writable then
     local cacheparent=dirname(cachepath)
     if is_writable(cacheparent) then 
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
 local texmfcaches=caches.defaults
 if texmfcaches then
  for k=1,#texmfcaches do
   local cachepath=texmfcaches[k]
   cachepath=resolvers.expansion(cachepath) 
   if cachepath~="" then
    cachepath=resolvepath(cachepath)
    cachepath=cleanpath(cachepath)
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
  osexit()
 elseif #readables==0 then
  report_caches("fatal error: there is no valid readable cache path defined")
  osexit()
 end
 writable=expanddirname(cleanpath(writable))
 local base=caches.base
 local more=caches.more
 local tree=cache_tree or treehash() 
 if tree then
  cache_tree=tree
  writable=mkdirs(writable,base,more,tree)
  for i=1,#readables do
   readables[i]=joinfile(readables[i],base,more,tree)
  end
 else
  writable=mkdirs(writable,base,more)
  for i=1,#readables do
   readables[i]=joinfile(readables[i],base,more)
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
function caches.usedpaths(separator)
 local writable,readables=identify()
 if #readables>1 then
  local result={}
  local done={}
  for i=1,#readables do
   local readable=readables[i]
   if readable==writable then
    done[readable]=true
    result[#result+1]=formatters["readable+writable: %a"](readable)
   elseif usedreadables[i] then
    done[readable]=true
    result[#result+1]=formatters["readable: %a"](readable)
   end
  end
  if not done[writable] then
   result[#result+1]=formatters["writable: %a"](writable)
  end
  return concat(result,separator or " | ")
 else
  return writable or "?"
 end
end
local r_cache={}
local w_cache={}
local function getreadablepaths(...)
 local tags={... }
 local hash=concat(tags,"/")
 local done=r_cache[hash]
 if not done then
  local writable,readables=identify() 
  if #tags>0 then
   done={}
   for i=1,#readables do
    done[i]=joinfile(readables[i],...)
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
local function setfirstwritablefile(filename,...)
 local wr=getwritablepath(...)
 local fullname=joinfile(wr,filename)
 return fullname,wr
end
local function setluanames(path,name)
 return
  format("%s/%s.%s",path,name,luasuffixes.tma),
  format("%s/%s.%s",path,name,luasuffixes.tmc)
end
local function getfirstreadablefile(filename,...)
 local fullname,path=setfirstwritablefile(filename,...)
 if is_readable(fullname) then
  return fullname,path 
 end
 local rd=getreadablepaths(...)
 for i=1,#rd do
  local path=rd[i]
  local fullname=joinfile(path,filename)
  if is_readable(fullname) then
   usedreadables[i]=true
   return fullname,path 
  end
 end
 return fullname,path 
end
caches.getreadablepaths=getreadablepaths
caches.getwritablepath=getwritablepath
caches.setfirstwritablefile=setfirstwritablefile
caches.getfirstreadablefile=getfirstreadablefile
caches.setluanames=setluanames
function caches.loaddata(readables,name,writable)
 if type(readables)=="string" then
  readables={ readables }
 end
 for i=1,#readables do
  local path=readables[i]
  local loader=false
  local state=false
  local tmaname,tmcname=setluanames(path,name)
  if isfile(tmcname) then
   state,loader=pcall(loadfile,tmcname)
  end
  if not loader and isfile(tmaname) then
   local tmacrap,tmcname=setluanames(writable,name)
   if isfile(tmcname) then
    state,loader=pcall(loadfile,tmcname)
   end
   compilelua(tmaname,tmcname)
   if isfile(tmcname) then
    state,loader=pcall(loadfile,tmcname)
   end
   if not loader then
    state,loader=pcall(loadfile,tmaname)
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
 local tmaname,tmcname=setluanames(filepath,filename)
 return is_writable(tmaname)
end
local saveoptions={ compact=true,accurate=not JITSUPPORTED }
function caches.savedata(filepath,filename,data,fast)
 local tmaname,tmcname=setluanames(filepath,filename)
 data.cache_uuid=osuuid()
 if fast or fast_cache then
  savedata(tmaname,fastserialize(data,true))
 elseif direct_cache then
  savedata(tmaname,serialize(data,true,saveoptions))
 else
  serializetofile(tmaname,data,true,saveoptions)
 end
 compilelua(tmaname,tmcname)
end
local content_state={}
function caches.contentstate()
 return content_state or {}
end
function caches.loadcontent(cachename,dataname,filename)
 if not filename then
  local name=hashed(cachename)
  local full,path=getfirstreadablefile(addsuffix(name,luasuffixes.lua),"trees")
  filename=joinfile(path,name)
 end
 local state,blob=pcall(loadfile,addsuffix(filename,luasuffixes.luc))
 if not blob then
  state,blob=pcall(loadfile,addsuffix(filename,luasuffixes.lua))
 end
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
function caches.savecontent(cachename,dataname,content,filename)
 if not filename then
  local name=hashed(cachename)
  local full,path=setfirstwritablefile(addsuffix(name,luasuffixes.lua),"trees")
  filename=joinfile(path,name) 
 end
 local luaname=addsuffix(filename,luasuffixes.lua)
 local lucname=addsuffix(filename,luasuffixes.luc)
 if trace_locating then
  report_resolvers("preparing %a for %a",dataname,cachename)
 end
 local data={
  type=dataname,
  root=cachename,
  version=resolvers.cacheversion,
  date=osdate("%Y-%m-%d"),
  time=osdate("%H:%M:%S"),
  content=content,
  uuid=osuuid(),
 }
 local ok=savedata(luaname,serialize(data,true))
 if ok then
  if trace_locating then
   report_resolvers("category %a, cachename %a saved in %a",dataname,cachename,luaname)
  end
  if compilelua(luaname,lucname) then
   if trace_locating then
    report_resolvers("%a compiled to %a",dataname,lucname)
   end
   return true
  else
   if trace_locating then
    report_resolvers("compiling failed for %a, deleting file %a",dataname,lucname)
   end
   removefile(lucname)
  end
 elseif trace_locating then
  report_resolvers("unable to save %a in %a (access error)",dataname,luaname)
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-met"] = package.loaded["data-met"] or true

-- original size: 5518, stripped down to: 3854

if not modules then modules={} end modules ['data-met']={
 version=1.100,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local type=type
local find=string.find
local addurlscheme,urlhashed=url.addscheme,url.hashed
local collapsepath,joinfile=file.collapsepath,file.join
local report_methods=logs.reporter("resolvers","methods")
local trace_locating=false
local trace_methods=false
trackers.register("resolvers.locating",function(v) trace_methods=v end)
trackers.register("resolvers.methods",function(v) trace_methods=v end)
local allocate=utilities.storage.allocate
local resolvers=resolvers
local registered={}
local function splitmethod(filename) 
 if not filename then
  return {
   scheme="unknown",
   original=filename,
  }
 end
 if type(filename)=="table" then
  return filename 
 end
 filename=collapsepath(filename,".") 
 if not find(filename,"://",1,true) then
  return {
   scheme="file",
   path=filename,
   original=filename,
   filename=filename,
  }
 end
 local specification=urlhashed(filename)
 if not specification.scheme or specification.scheme=="" then
  return {
   scheme="file",
   path=filename,
   original=filename,
   filename=filename,
  }
 else
  return specification
 end
end
resolvers.splitmethod=splitmethod
local function methodhandler(what,first,...) 
 local method=registered[what]
 if method then
  local how=method.how
  local namespace=method.namespace
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
 registered[name]={
  how=how or "tag",
  namespace=namespace
 }
 namespace["byscheme"]=function(scheme,filename,...)
  if scheme=="file" then
   return methodhandler(name,filename,...)
  else
   return methodhandler(name,addurlscheme(filename,scheme),...)
  end
 end
end
local concatinators=allocate { notfound=joinfile  }  
local locators=allocate { notfound=function() end  }  
local hashers=allocate { notfound=function() end  }  
local generators=allocate { notfound=function() end  }  
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

-- original size: 69576, stripped down to: 44470

if not modules then modules={} end modules ['data-res']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files",
}
local gsub,find,lower,upper,match,gmatch=string.gsub,string.find,string.lower,string.upper,string.match,string.gmatch
local concat,insert,remove=table.concat,table.insert,table.remove
local next,type,rawget,loadfile=next,type,rawget,loadfile
local mergedtable=table.merged
local os=os
local P,S,R,C,Cc,Cs,Ct,Carg=lpeg.P,lpeg.S,lpeg.R,lpeg.C,lpeg.Cc,lpeg.Cs,lpeg.Ct,lpeg.Carg
local lpegmatch,lpegpatterns=lpeg.match,lpeg.patterns
local formatters=string.formatters
local filedirname=file.dirname
local filebasename=file.basename
local suffixonly=file.suffixonly
local addsuffix=file.addsuffix
local removesuffix=file.removesuffix
local filejoin=file.join
local collapsepath=file.collapsepath
local joinpath=file.joinpath
local is_qualified_path=file.is_qualified_path
local allocate=utilities.storage.allocate
local settings_to_array=utilities.parsers.settings_to_array
local urlhasscheme=url.hasscheme
local getcurrentdir=lfs.currentdir
local isfile=lfs.isfile
local isdir=lfs.isdir
local setmetatableindex=table.setmetatableindex
local luasuffixes=utilities.lua.suffixes
local trace_locating=false  trackers  .register("resolvers.locating",function(v) trace_locating=v end)
local trace_details=false  trackers  .register("resolvers.details",function(v) trace_details=v end)
local trace_expansions=false  trackers  .register("resolvers.expansions",function(v) trace_expansions=v end)
local trace_paths=false  trackers  .register("resolvers.paths",function(v) trace_paths=v end)
local resolve_otherwise=true   directives.register("resolvers.otherwise",function(v) resolve_otherwise=v end)
local report_resolving=logs.reporter("resolvers","resolving")
local resolvers=resolvers
local expandedpathfromlist=resolvers.expandedpathfromlist
local checkedvariable=resolvers.checkedvariable
local splitconfigurationpath=resolvers.splitconfigurationpath
local methodhandler=resolvers.methodhandler
local filtered=resolvers.filtered_from_content
local lookup=resolvers.get_from_content
local cleanpath=resolvers.cleanpath
local resolveprefix=resolvers.resolve
local initializesetter=utilities.setters.initialize
local ostype,osname,osenv,ossetenv,osgetenv=os.type,os.name,os.env,os.setenv,os.getenv
resolvers.cacheversion="1.100"
resolvers.configbanner=""
resolvers.homedir=environment.homedir
resolvers.luacnfname="texmfcnf.lua"
resolvers.luacnffallback="contextcnf.lua"
resolvers.luacnfstate="unknown"
local criticalvars={
 "SELFAUTOLOC",
 "SELFAUTODIR",
 "SELFAUTOPARENT",
 "TEXMFCNF",
 "TEXMF",
 "TEXOS",
}
if environment.default_texmfcnf then
 resolvers.luacnfspec="home:texmf/web2c;"..environment.default_texmfcnf 
else
 resolvers.luacnfspec=concat ({
  "home:texmf/web2c",
  "selfautoparent:/texmf-local/web2c",
  "selfautoparent:/texmf-context/web2c",
  "selfautoparent:/texmf-dist/web2c",
  "selfautoparent:/texmf/web2c",
 },";")
end
local unset_variable="unset"
local formats=resolvers.formats
local suffixes=resolvers.suffixes
local usertypes=resolvers.usertypes
local dangerous=resolvers.dangerous
local suffixmap=resolvers.suffixmap
resolvers.defaultsuffixes={ "tex" } 
local instance=nil
local variable
local expansion
local setenv
local getenv
local formatofsuffix=resolvers.formatofsuffix
local splitpath=resolvers.splitpath
local splitmethod=resolvers.splitmethod
setenv=function(key,value,raw)
 if instance then
  instance.environment[key]=value
  ossetenv(key,raw and value or resolveprefix(value))
 end
end
getenv=function(key)
 local value=rawget(instance.environment,key)
 if value and value~="" then
  return value
 else
  local e=osgetenv(key)
  return e~=nil and e~="" and checkedvariable(e) or ""
 end
end
resolvers.getenv=getenv
resolvers.setenv=setenv
local dollarstripper=lpeg.stripper("$")
local inhibitstripper=P("!")^0*Cs(P(1)^0)
local expandedvariable,resolvedvariable  do
 local function resolveinstancevariable(k)
  return instance.expansions[k]
 end
 local p_variable=P("$")/""
 local p_key=C(R("az","AZ","09","__","--")^1)
 local p_whatever=P(";")*((1-S("!{}/\\"))^1*P(";")/"")+P(";")*(P(";")/"")+P(1)
 local variableexpander=Cs((p_variable*(p_key/resolveinstancevariable)+p_whatever)^1 )
 local p_cleaner=P("\\")/"/"+P(";")*S("!{}/\\")^0*P(";")^1/";"
 local variablecleaner=Cs((p_cleaner+P(1))^0)
 local p_variable=R("az","AZ","09","__","--")^1/resolveinstancevariable
 local p_variable=(P("$")/"")*(p_variable+(P("{")/"")*p_variable*(P("}")/""))
 local variableresolver=Cs((p_variable+P(1))^0)
 expandedvariable=function(var)
  return lpegmatch(variableexpander,var) or var
 end
 function resolvers.reset()
  if trace_locating then
   report_resolving("creating instance")
  end
  local environment={}
  local variables={}
  local expansions={}
  local order={}
  instance={
   environment=environment,
   variables=variables,
   expansions=expansions,
   order=order,
   files={},
   setups={},
   found={},
   foundintrees={},
   hashes={},
   hashed={},
   pathlists=false,
   specification={},
   lists={},
   data={},
   fakepaths={},
   remember=true,
   diskcache=true,
   renewcache=false,
   renewtree=false,
   loaderror=false,
   savelists=true,
   pattern=nil,
   force_suffixes=true,
   pathstack={},
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
  local repath=resolvers.repath
  setmetatableindex(environment,function(t,k)
   local v=osgetenv(k)
   if v==nil then
    v=variables[k]
   end
   if v~=nil then
    v=checkedvariable(v) or ""
   end
   v=repath(v) 
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
 end
end
function resolvers.initialized()
 return instance~=nil
end
local function reset_hashes()
 instance.lists={}
 instance.pathlists=false
 instance.found={}
end
local function reset_caches()
 instance.lists={}
 instance.pathlists=false
end
local makepathexpression  do
 local slash=P("/")
 local pathexpressionpattern=Cs (
  Cc("^")*(
   Cc("%")*S(".-")+slash^2*P(-1)/"/.*"
+slash^2/"/"+(1-slash)*P(-1)*Cc("/")+P(1)
  )^1*Cc("$") 
 )
 local cache={}
 makepathexpression=function(str)
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
end
local function reportcriticalvariables(cnfspec)
 if trace_locating then
  for i=1,#criticalvars do
   local k=criticalvars[i]
   local v=getenv(k) or "unknown" 
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
  local cnfpaths=expandedpathfromlist(splitpath(cnfspec))
  local function locatecnf(luacnfname,kind)
   for i=1,#cnfpaths do
    local filepath=cnfpaths[i]
    local filename=collapsepath(filejoin(filepath,luacnfname))
    local realname=resolveprefix(filename)
    if trace_locating then
     local fullpath=gsub(resolveprefix(collapsepath(filepath)),"//","/")
     local weirdpath=find(fullpath,"/texmf.+/texmf") or not find(fullpath,"/web2c",1,true)
     report_resolving("looking for %s %a on %s path %a from specification %a",
      kind,luacnfname,weirdpath and "weird" or "given",fullpath,filepath)
    end
    if isfile(realname) then
     specification[#specification+1]=filename 
     if trace_locating then
      report_resolving("found %s configuration file %a",kind,realname)
     end
    end
   end
  end
  locatecnf(resolvers.luacnfname,"regular")
  if #specification==0 then
   locatecnf(resolvers.luacnffallback,"fallback")
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
 local setups=instance.setups
 local order=instance.order
 if #specification>0 then
  local luacnfname=resolvers.luacnfname
  for i=1,#specification do
   local filename=specification[i]
   local pathname=filedirname(filename)
   local filename=filejoin(pathname,luacnfname)
   local realname=resolveprefix(filename) 
   local blob=loadfile(realname)
   if blob then
    local data=blob()
    local parent=data and data.parent
    if parent then
     local filename=filejoin(pathname,parent)
     local realname=resolveprefix(filename) 
     local blob=loadfile(realname)
     if blob then
      local parentdata=blob()
      if parentdata then
       report_resolving("loading configuration file %a",filename)
       data=mergedtable(parentdata,data)
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
         k,resolveprefix(filename))
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
       setenv("TEXMFCNF",cnfspec)
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
   order[#order+1]=setups[pathname]
   if instance.loaderror then
    break
   end
  end
 elseif trace_locating then
  report_resolving("warning: no lua configuration files found")
 end
end
local expandedpathlist
local unexpandedpathlist
function resolvers.configurationfiles()
 return instance.specification or {}
end
local function load_file_databases()
 instance.loaderror=false
 instance.files={}
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
 local texmfpaths=expandedpathlist("TEXMF")
 if #texmfpaths>0 then
  for i=1,#texmfpaths do
   local path=collapsepath(texmfpaths[i])
   path=gsub(path,"/+$","") 
   local stripped=lpegmatch(inhibitstripper,path) 
   if stripped~="" then
    local runtime=stripped==path
    path=cleanpath(path)
    local spec=splitmethod(stripped)
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
 local hashes=instance.hashes
 local files=instance.files
 for i=1,#hashes do
  local hash=hashes[i]
  local cachename=hash.name
  if hash.cache then
   local content=files[cachename]
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
 local files=instance.files
 if hashname and hashname~="" then
  local expanded=expansion(hashname) or ""
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
  local realpath=resolveprefix(hashname)
  if isdir(realpath) then
   if trace_locating then
    report_resolving("using path %a",realpath)
   end
   methodhandler('generators',hashname)
   local content=files[hashname]
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
 local hashed=instance.hashed
 local hashes=instance.hashes
 if hashed[name] then
 else
  if trace_locating then
   report_resolving("hash %a appended",name)
  end
  insert(hashes,{ type=type,name=name,cache=cache } )
  hashed[name]=cache
 end
end
function resolvers.prependhash(type,name,cache)
 local hashed=instance.hashed
 local hashes=instance.hashes
 if hashed[name] then
 else
  if trace_locating then
   report_resolving("hash %a prepended",name)
  end
  insert(hashes,1,{ type=type,name=name,cache=cache } )
  hashed[name]=cache
 end
end
function resolvers.extendtexmfvariable(specification) 
 local environment=instance.environment
 local variables=instance.variables
 local texmftrees=splitpath(getenv("TEXMF")) 
 insert(texmftrees,1,specification)
 texmftrees=concat(texmftrees,",") 
 if environment["TEXMF"] then
  environment["TEXMF"]=texmftrees
 elseif variables["TEXMF"] then
  variables["TEXMF"]=texmftrees
 else
 end
 reset_hashes()
end
function resolvers.splitexpansions()
 local expansions=instance.expansions
 for k,v in next,expansions do
  local t,tn,h,p={},0,{},splitconfigurationpath(v)
  for kk=1,#p do
   local vv=p[kk]
   if vv~="" and not h[vv] then
    tn=tn+1
    t[tn]=vv
    h[vv]=true
   end
  end
  if tn>1 then
   expansions[k]=t
  else
   expansions[k]=t[1]
  end
 end
end
function resolvers.datastate()
 return caches.contentstate()
end
variable=function(name)
 local variables=instance.variables
 local name=name and lpegmatch(dollarstripper,name)
 local result=name and variables[name]
 return result~=nil and result or ""
end
expansion=function(name)
 local expansions=instance.expansions
 local name=name and lpegmatch(dollarstripper,name)
 local result=name and expansions[name]
 return result~=nil and result or ""
end
resolvers.variable=variable
resolvers.expansion=expansion
unexpandedpathlist=function(str)
 local pth=variable(str)
 local lst=splitpath(pth)
 return expandedpathfromlist(lst)
end
function resolvers.unexpandedpath(str)
 return joinpath(unexpandedpathlist(str))
end
function resolvers.pushpath(name)
 local pathstack=instance.pathstack
 local lastpath=pathstack[#pathstack]
 local pluspath=filedirname(name)
 if lastpath then
  lastpath=collapsepath(filejoin(lastpath,pluspath))
 else
  lastpath=collapsepath(pluspath)
 end
 insert(pathstack,lastpath)
 if trace_paths then
  report_resolving("pushing path %a",lastpath)
 end
end
function resolvers.poppath()
 local pathstack=instance.pathstack
 if trace_paths and #pathstack>0 then
  report_resolving("popping path %a",pathstack[#pathstack])
 end
 remove(pathstack)
end
function resolvers.stackpath()
 local pathstack=instance.pathstack
 local currentpath=pathstack[#pathstack]
 return currentpath~="" and currentpath or nil
end
local done={}
function resolvers.resetextrapaths()
 local extra_paths=instance.extra_paths
 if not extra_paths then
  done={}
  instance.extra_paths={}
 elseif #ep>0 then
  done={}
  reset_caches()
 end
end
function resolvers.getextrapaths()
 return instance.extra_paths or {}
end
function resolvers.registerextrapath(paths,subpaths)
 if not subpaths or subpaths=="" then
  if not paths or path=="" then
   return 
  elseif done[paths] then
   return 
  end
 end
 local paths=settings_to_array(paths)
 local subpaths=settings_to_array(subpaths)
 local extra_paths=instance.extra_paths or {}
 local oldn=#extra_paths
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
      extra_paths[newn]=cleanpath(ps)
      done[ps]=true
     end
    end
   end
  else
   for i=1,nofpaths do
    local p=paths[i]
    if not done[p] then
     newn=newn+1
     extra_paths[newn]=cleanpath(p)
     done[p]=true
    end
   end
  end
 elseif nofsubpaths>0 then
  for i=1,oldn do
   for j=1,nofsubpaths do
    local s=subpaths[j]
    local ps=extra_paths[i].."/"..s
    if not done[ps] then
     newn=newn+1
     extra_paths[newn]=cleanpath(ps)
     done[ps]=true
    end
   end
  end
 end
 if newn>0 then
  instance.extra_paths=extra_paths 
 end
 if newn~=oldn then
  reset_caches()
 end
end
function resolvers.pushextrapath(path)
 local paths=settings_to_array(path)
 local extra_stack=instance.extra_stack
 if extra_stack then
  insert(extra_stack,1,paths)
 else
  instance.extra_stack={ paths }
 end
 reset_caches()
end
function resolvers.popextrapath()
 local extra_stack=instance.extra_stack
 if extra_stack then
  reset_caches()
  return remove(extra_stack,1)
 end
end
local function made_list(instance,list,extra_too)
 local done={}
 local new={}
 local newn=0
 local function add(p)
  for k=1,#p do
   local v=p[k]
   if not done[v] then
    done[v]=true
    newn=newn+1
    new[newn]=v
   end
  end
 end
 for k=1,#list do
  local v=list[k]
  if done[v] then
  elseif find(v,"^[%.%/]$") then
   done[v]=true
   newn=newn+1
   new[newn]=v
  else
   break
  end
 end
 if extra_too then
  local extra_stack=instance.extra_stack
  local extra_paths=instance.extra_paths
  if extra_stack and #extra_stack>0 then
   for k=1,#extra_stack do
    add(extra_stack[k])
   end
  end
  if extra_paths and #extra_paths>0 then
   add(extra_paths)
  end
 end
 add(list)
 return new
end
expandedpathlist=function(str,extra_too)
 if not str then
  return {}
 elseif instance.savelists then 
  str=lpegmatch(dollarstripper,str)
  local lists=instance.lists
  local lst=lists[str]
  if not lst then
   local l=made_list(instance,splitpath(expansion(str)),extra_too)
   lst=expandedpathfromlist(l)
   lists[str]=lst
  end
  return lst
 else
  local lst=splitpath(expansion(str))
  return made_list(instance,expandedpathfromlist(lst),extra_too)
 end
end
resolvers.expandedpathlist=expandedpathlist
resolvers.unexpandedpathlist=unexpandedpathlist
function resolvers.cleanpathlist(str)
 local t=expandedpathlist(str)
 if t then
  for i=1,#t do
   t[i]=collapsepath(cleanpath(t[i]))
  end
 end
 return t
end
function resolvers.expandpath(str)
 return joinpath(expandedpathlist(str))
end
local function expandedpathlistfromvariable(str) 
 str=lpegmatch(dollarstripper,str)
 local tmp=resolvers.variableofformatorsuffix(str)
 return expandedpathlist(tmp~="" and tmp or str)
end
function resolvers.expandpathfromvariable(str)
 return joinpath(expandedpathlistfromvariable(str))
end
resolvers.expandedpathlistfromvariable=expandedpathlistfromvariable
function resolvers.cleanedpathlist(v) 
 local t=expandedpathlist(v)
 for i=1,#t do
  t[i]=resolveprefix(cleanpath(t[i]))
 end
 return t
end
function resolvers.expandbraces(str) 
 local pth=expandedpathfromlist(splitpath(str))
 return joinpath(pth)
end
function resolvers.registerfilehash(name,content,someerror)
 local files=instance.files
 if content then
  files[name]=content
 else
  files[name]={}
  if somerror==true then 
   instance.loaderror=someerror
  end
 end
end
function resolvers.getfilehashes()
 return instance and instance.files or {}
end
function resolvers.gethashes()
 return instance and instance.hashes or {}
end
function resolvers.renewcache()
 if instance then
  instance.renewcache=true
 end
end
local function isreadable(name)
 local readable=isfile(name) 
 if trace_details then
  if readable then
   report_resolving("file %a is readable",name)
  else
   report_resolving("file %a is not readable",name)
  end
 end
 return readable
end
local function collect_files(names) 
 local filelist={}   
 local noffiles=0
 local function check(hash,root,pathname,path,basename,name)
  if not pathname or find(path,pathname) then
   local variant=hash.type
   local search=filejoin(root,path,name) 
   local result=methodhandler('concatinators',variant,root,path,name)
   if trace_details then
    report_resolving("match: variant %a, search %a, result %a",variant,search,result)
   end
   noffiles=noffiles+1
   filelist[noffiles]={ variant,search,result }
  end
 end
 for k=1,#names do
  local filename=names[k]
  if trace_details then
   report_resolving("checking name %a",filename)
  end
  local basename=filebasename(filename)
  local pathname=filedirname(filename)
  if pathname=="" or find(pathname,"^%.") then
   pathname=false
  else
   pathname=gsub(pathname,"%*",".*")
   pathname="/"..pathname.."$"
  end
  local hashes=instance.hashes
  local files=instance.files
  for h=1,#hashes do
   local hash=hashes[h]
   local hashname=hash.name
   local content=hashname and files[hashname]
   if content then
    if trace_details then
     report_resolving("deep checking %a, base %a, pattern %a",hashname,basename,pathname)
    end
    local path,name=lookup(content,basename)
    if path then
     local metadata=content.metadata
     local realroot=metadata and metadata.path or hashname
     if type(path)=="string" then
      check(hash,realroot,pathname,path,basename,name)
     else
      for i=1,#path do
       check(hash,realroot,pathname,path[i],basename,name)
      end
     end
    end
   elseif trace_locating then
    report_resolving("no match in %a (%s)",hashname,basename)
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
  local collapsed=collapsepath(foundname,true)
  local t={
   filename=filename,
   format=format~="" and format   or nil,
   filetype=filetype~="" and filetype or nil,
   usedmethod=usedmethod,
   foundname=foundname,
   fullname=collapsed,
  }
  fit[foundname]=t
  foundintrees[#foundintrees+1]=t
 end
end
function resolvers.foundintrees()
 return instance.foundintrees or {}
end
function resolvers.foundintree(fullname)
 local f=fit[fullname]
 return f and f.usedmethod=="database"
end
local function can_be_dir(name) 
 local fakepaths=instance.fakepaths
 if not fakepaths[name] then
  if isdir(name) then
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
 local filetype=''
 local filesuffix=suffixonly(filename)
 local wantedfiles={}
 wantedfiles[#wantedfiles+1]=filename
 if askedformat=="" then
  if filesuffix=="" or not suffixmap[filesuffix] then
   local defaultsuffixes=resolvers.defaultsuffixes
   for i=1,#defaultsuffixes do
    local forcedname=filename..'.'..defaultsuffixes[i]
    wantedfiles[#wantedfiles+1]=forcedname
    filetype=formatofsuffix(forcedname)
    if trace_locating then
     report_resolving("forcing filetype %a",filetype)
    end
   end
  else
   filetype=formatofsuffix(filename)
   if trace_locating then
    report_resolving("using suffix based filetype %a",filetype)
   end
  end
 else
  if filesuffix=="" or not suffixmap[filesuffix] then
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
  if trace_details then
   report_resolving("file %a found directly",filename)
  end
  return "direct",{ filename }
 end
end
local function find_wildcard(filename,allresults)
 if find(filename,'*',1,true) then
  if trace_locating then
   report_resolving("checking wildcard %a",filename)
  end
  local result=resolvers.findwildcardfiles(filename)
  if result then
   return "wildcard",result
  end
 end
end
local function find_qualified(filename,allresults,askedformat,alsostripped) 
 if not is_qualified_path(filename) then
  return
 end
 if trace_locating then
  report_resolving("checking qualified name %a",filename)
 end
 if isreadable(filename) then
  if trace_details then
   report_resolving("qualified file %a found",filename)
  end
  return "qualified",{ filename }
 end
 if trace_details then
  report_resolving("locating qualified file %a",filename)
 end
 local forcedname,suffix="",suffixonly(filename)
 if suffix=="" then 
  local format_suffixes=askedformat=="" and resolvers.defaultsuffixes or suffixes[askedformat]
  if format_suffixes then
   for i=1,#format_suffixes do
    local suffix=format_suffixes[i]
    forcedname=filename.."."..suffix
    if isreadable(forcedname) then
     if trace_locating then
      report_resolving("no suffix, forcing format filetype %a",suffix)
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
   askedformat=formatofsuffix(suffix)
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
  if trace_details then
   report_resolving("found %a by deep scanning",fname)
  end
  return fname
 end
end
local function makepathlist(list,filetype)
 local typespec=resolvers.variableofformat(filetype)
 local pathlist=expandedpathlist(typespec,filetype and usertypes[filetype]) 
 local entry={}
 if pathlist and #pathlist>0 then
  for k=1,#pathlist do
   local path=pathlist[k]
   local prescanned=find(path,'^!!')
   local resursive=find(path,'//$')
   local pathname=lpegmatch(inhibitstripper,path)
   local expression=makepathexpression(pathname)
   local barename=gsub(pathname,"/+$","")
   barename=resolveprefix(barename)
   local scheme=urlhasscheme(barename)
   local schemename=gsub(barename,"%.%*$",'')
   entry[k]={
    path=path,
    pathname=pathname,
    prescanned=prescanned,
    recursive=recursive,
    expression=expression,
    barename=barename,
    scheme=scheme,
    schemename=schemename,
   }
  end
  entry.typespec=typespec
  list[filetype]=entry
 else
  list[filetype]=false
 end
 return entry
end
local function find_intree(filename,filetype,wantedfiles,allresults)
 local pathlists=instance.pathlists
 if not pathlists then
  pathlists=setmetatableindex({},makepathlist)
  instance.pathlists=pathlists
 end
 local pathlist=pathlists[filetype]
 if pathlist then
  local method="intree"
  local filelist=collect_files(wantedfiles) 
  local dirlist={}
  local result={}
  if filelist then
   for i=1,#filelist do
    dirlist[i]=filedirname(filelist[i][3]).."/" 
   end
  end
  if trace_details then
   report_resolving("checking filename %a in tree",filename)
  end
  for k=1,#pathlist do
   local entry=pathlist[k]
   local path=entry.path
   local pathname=entry.pathname
   local done=false
   if filelist then
    local expression=entry.expression
    if trace_details then
     report_resolving("using pattern %a for path %a",expression,pathname)
    end
    for k=1,#filelist do
     local fl=filelist[k]
     local f=fl[2]
     local d=dirlist[k]
     if find(d,expression) or find(resolveprefix(d),expression) then
      result[#result+1]=resolveprefix(fl[3]) 
      done=true
      if allresults then
       if trace_details then
        report_resolving("match to %a in hash for file %a and path %a, continue scanning",expression,f,d)
       end
      else
       if trace_details then
        report_resolving("match to %a in hash for file %a and path %a, quit scanning",expression,f,d)
       end
       break
      end
     elseif trace_details then
      report_resolving("no match to %a in hash for file %a and path %a",expression,f,d)
     end
    end
   end
   if done then
    method="database"
   else
    method="filesystem" 
    local scheme=entry.scheme
    if not scheme or scheme=="file" then
     local pname=entry.schemename
     if not find(pname,"*",1,true) then
      if can_be_dir(pname) then
       if not done and not entry.prescanned then
        if trace_details then
         report_resolving("quick root scan for %a",pname)
        end
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
        if not done and entry.recursive then
         if trace_details then
          report_resolving("scanning filesystem for %a",pname)
         end
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
      end
     else
     end
    else
     for k=1,#wantedfiles do
      local pname=entry.barename
      local fname=methodhandler('finders',pname.."/"..wantedfiles[k])
      if fname then
       result[#result+1]=fname
       done=true
       if not allresults then
        break
       end
      end
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
 if trace_details then
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
  return "otherwise",{ resolveprefix(fl[3]) } 
 end
end
collect_instance_files=function(filename,askedformat,allresults) 
 if not filename or filename=="" then
  return {}
 end
 askedformat=askedformat or ""
 filename=collapsepath(filename,".")
 filename=gsub(filename,"^%./",getcurrentdir().."/") 
 if allresults then
  local filetype,wantedfiles=find_analyze(filename,askedformat)
  local results={
   { find_direct   (filename,true) },
   { find_wildcard (filename,true) },
   { find_qualified(filename,true,askedformat) },
   { find_intree   (filename,filetype,wantedfiles,true) },
   { find_onpath   (filename,filetype,wantedfiles,true) },
   { find_otherwise(filename,filetype,wantedfiles,true) },
  }
  local result={}
  local status={}
  local done={}
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
  if trace_details then
   report_resolving("lookup status: %s",table.serialize(status,filename))
  end
  return result,status
 else
  local method,result,stamp,filetype,wantedfiles
  if instance.remember then
   if askedformat=="" then
    stamp=formatters["%s::%s"](suffixonly(filename),filename)
   else
    stamp=formatters["%s::%s"](askedformat,filename)
   end
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
      if resolve_otherwise and not result then
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
    report_resolving("remembering file %a using hash %a",filename,stamp)
   end
   instance.found[stamp]=result
  end
  return result
 end
end
local function findfiles(filename,filetype,allresults)
 if not filename or filename=="" then
  return {}
 end
 if allresults==nil then
  allresults=true
 end
 local result,status=collect_instance_files(filename,filetype or "",allresults)
 if not result or #result==0 then
  local lowered=lower(filename)
  if filename~=lowered then
   result,status=collect_instance_files(lowered,filetype or "",allresults)
  end
 end
 return result or {},status
end
local function findfile(filename,filetype)
 if not filename or filename=="" then
  return ""
 else
  return findfiles(filename,filetype,false)[1] or ""
 end
end
resolvers.findfiles=findfiles
resolvers.findfile=findfile
resolvers.find_file=findfile  
resolvers.find_files=findfiles 
function resolvers.findpath(filename,filetype)
 return filedirname(findfiles(filename,filetype,false)[1] or "")
end
local function findgivenfiles(filename,allresults)
 local hashes=instance.hashes
 local files=instance.files
 local base=filebasename(filename)
 local result={}
 local function okay(hash,path,name)
  local found=methodhandler('concatinators',hash.type,hash.name,path,name)
  if found and found~="" then
   result[#result+1]=resolveprefix(found)
   return not allresults
  end
 end
 for k=1,#hashes do
  local hash=hashes[k]
  local content=files[hash.name]
  if content then
   local path,name=lookup(content,base)
   if not path then
   elseif type(path)=="string" then
    if okay(hash,path,name) then
     return result
    end
   else
    for i=1,#path do
     if okay(hash,path[i],name) then
      return result
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
local makewildcard=Cs(
 (P("^")^0*P("/")*P(-1)+P(-1))/".*"+(P("^")^0*P("/")/"")^0*(P("*")/".*"+P("-")/"%%-"+P(".")/"%%."+P("?")/"."+P("\\")/"/"+P(1))^0
)
function resolvers.wildcardpattern(pattern)
 return lpegmatch(makewildcard,pattern) or pattern
end
local function findwildcardfiles(filename,allresults,result)
 local files=instance.files
 local hashes=instance.hashes
 local result=result or {}
 local base=filebasename(filename)
 local dirn=filedirname(filename)
 local path=lower(lpegmatch(makewildcard,dirn) or dirn)
 local name=lower(lpegmatch(makewildcard,base) or base)
 if find(name,"*",1,true) then
  local function okay(found,path,base,hashname,hashtype)
   if find(found,path) then
    local full=methodhandler('concatinators',hashtype,hashname,found,base)
    if full and full~="" then
     result[#result+1]=resolveprefix(full)
     return not allresults
    end
   end
  end
  for k=1,#hashes do
   local hash=hashes[k]
   local hashname=hash.name
   local hashtype=hash.type
   if hashname and hashtype then
    for found,base in filtered(files[hashname],name) do
     if type(found)=='string' then
      if okay(found,path,base,hashname,hashtype) then
       break
      end
     else
      for i=1,#found do
       if okay(found[i],path,base,hashname,hashtype) then
        break
       end
      end
     end
    end
   end
  end
 else
  local function okayokay(found,path,base,hashname,hashtype)
   if find(found,path) then
    local full=methodhandler('concatinators',hashtype,hashname,found,base)
    if full and full~="" then
     result[#result+1]=resolveprefix(full)
     return not allresults
    end
   end
  end
  for k=1,#hashes do
   local hash=hashes[k]
   local hashname=hash.name
   local hashtype=hash.type
   if hashname and hashtype then
    local found,base=lookup(content,base)
    if not found then
    elseif type(found)=='string' then
     if okay(found,path,base,hashname,hashtype) then
      break
     end
    else
     for i=1,#found do
      if okay(found[i],path,base,hashname,hashtype) then
       break
      end
     end
    end
   end
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
do
 local starttiming=statistics.starttiming
 local stoptiming=statistics.stoptiming
 local elapsedtime=statistics.elapsedtime
 function resolvers.starttiming()
  starttiming(instance)
 end
 function resolvers.stoptiming()
  stoptiming(instance)
 end
 function resolvers.loadtime()
  return elapsedtime(instance)
 end
end
function resolvers.automount()
end
function resolvers.load(option)
 resolvers.starttiming()
 identify_configuration_files()
 load_configuration_files()
 if option~="nofiles" then
  load_databases()
  resolvers.automount()
 end
 resolvers.stoptiming()
 local files=instance.files
 return files and next(files) and true
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
 return joinpath(expandedpathlist(resolvers.formatofvariable(str)))
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
 local pathlist=expandedpathlist(name)
 for i=1,#pathlist do
  func("^"..cleanpath(pathlist[i]))
 end
end
function resolvers.dowithvariable(name,func)
 func(expandedvariable(name))
end
function resolvers.locateformat(name)
 local engine=environment.ownmain or "luatex"
 local barename=removesuffix(file.basename(name))
 local fullname=addsuffix(barename,"fmt")
 local fmtname=caches.getfirstreadablefile(fullname,"formats",engine) or ""
 if fmtname=="" then
  fmtname=findfile(fullname)
  fmtname=cleanpath(fmtname)
 end
 if fmtname~="" then
  local barename=removesuffix(fmtname)
  local luaname=addsuffix(barename,luasuffixes.lua)
  local lucname=addsuffix(barename,luasuffixes.luc)
  local luiname=addsuffix(barename,luasuffixes.lui)
  if isfile(luiname) then
   return fmtname,luiname
  elseif isfile(lucname) then
   return fmtname,lucname
  elseif isfile(luaname) then
   return fmtname,luaname
  end
 end
 return nil,nil
end
function resolvers.booleanvariable(str,default)
 local b=expansion(str)
 if b=="" then
  return default
 else
  b=toboolean(b)
  return (b==nil and default) or b
 end
end
function resolvers.dowithfilesintree(pattern,handle,before,after) 
 local hashes=instance.hashes
 local files=instance.files
 for i=1,#hashes do
  local hash=hashes[i]
  local blobtype=hash.type
  local blobpath=hash.name
  if blobtype and blobpath then
   local total=0
   local checked=0
   local done=0
   if before then
    before(blobtype,blobpath,pattern)
   end
   for path,name in filtered(files[blobpath],pattern) do
    if type(path)=="string" then
     checked=checked+1
     if handle(blobtype,blobpath,path,name) then
      done=done+1
     end
    else
     checked=checked+#path
     for i=1,#path do
      if handle(blobtype,blobpath,path[i],name) then
       done=done+1
      end
     end
    end
   end
   if after then
    after(blobtype,blobpath,pattern,checked,done)
   end
  end
 end
end
function resolvers.knownvariables(pattern)
 if instance then
  local environment=instance.environment
  local variables=instance.variables
  local expansions=instance.expansions
  local order=instance.order
  local pattern=upper(pattern or "")
  local result={}
  for i=1,#order do
   for key in next,order[i] do
    if result[key]==nil and key~="" and (pattern=="" or find(upper(key),pattern)) then
     result[key]={
      environment=rawget(environment,key),
      variable=key,
      expansion=expansions[key],
      resolved=resolveprefix(expansions[key]),
     }
    end
   end
  end
  return result
 else
  return {}
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-pre"] = package.loaded["data-pre"] or true

-- original size: 5088, stripped down to: 3144

if not modules then modules={} end modules ['data-pre']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local insert,remove=table.insert,table.remove
local resolvers=resolvers
local prefixes=resolvers.prefixes
local cleanpath=resolvers.cleanpath
local findgivenfile=resolvers.findgivenfile
local expansion=resolvers.expansion
local getenv=resolvers.getenv 
local basename=file.basename
local dirname=file.dirname
local joinpath=file.join
local isfile=lfs.isfile
prefixes.environment=function(str)
 return cleanpath(expansion(str))
end
local function relative(str,n)
 if not isfile(str) then
  local pstr="./"..str
  if isfile(pstr) then
   str=pstr
  else
   local p="../"
   for i=1,n or 2 do
    local pstr=p..str
    if isfile(pstr) then
     str=pstr
     break
    else
     p=p.."../"
    end
   end
  end
 end
 return cleanpath(str)
end
local function locate(str)
 local fullname=findgivenfile(str) or ""
 return cleanpath(fullname~="" and fullname or str)
end
prefixes.relative=relative
prefixes.locate=locate
prefixes.auto=function(str)
 local fullname=relative(str)
 if not isfile(fullname) then
  fullname=locate(str)
 end
 return fullname
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
 local pth=getenv('SELFAUTOLOC')
 return cleanpath(str and joinpath(pth,str) or pth)
end
prefixes.selfautoparent=function(str)
 local pth=getenv('SELFAUTOPARENT')
 return cleanpath(str and joinpath(pth,str) or pth)
end
prefixes.selfautodir=function(str)
 local pth=getenv('SELFAUTODIR')
 return cleanpath(str and joinpath(pth,str) or pth)
end
prefixes.home=function(str)
 local pth=getenv('HOME')
 return cleanpath(str and joinpath(pth,str) or pth)
end
prefixes.env=prefixes.environment
prefixes.rel=prefixes.relative
prefixes.loc=prefixes.locate
prefixes.kpse=prefixes.locate
prefixes.full=prefixes.locate
prefixes.file=prefixes.filename
prefixes.path=prefixes.pathname
local inputstack={}
local stackpath=resolvers.stackpath
local function toppath()
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
local function jobpath()
 local path=stackpath()
 if not path or path=="" then
  return "."
 else
  return path
 end
end
local function pushinputname(name)
 insert(inputstack,name)
end
local function popinputname(name)
 return remove(inputstack)
end
resolvers.toppath=toppath
resolvers.jobpath=jobpath
resolvers.pushinputname=pushinputname
resolvers.popinputname=popinputname
prefixes.toppath=function(str) return cleanpath(joinpath(toppath(),str)) end 
prefixes.jobpath=function(str) return cleanpath(joinpath(jobpath(),str)) end 
resolvers.setdynamic("toppath")
resolvers.setdynamic("jobpath")


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-inp"] = package.loaded["data-inp"] or true

-- original size: 910, stripped down to: 818

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

-- original size: 551, stripped down to: 470

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

-- original size: 4365, stripped down to: 3588

if not modules then modules={} end modules ['data-fil']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local ioopen=io.open
local isdir=lfs.isdir
local trace_locating=false  trackers.register("resolvers.locating",function(v) trace_locating=v end)
local report_files=logs.reporter("resolvers","files")
local resolvers=resolvers
local resolveprefix=resolvers.resolve
local findfile=resolvers.findfile
local scanfiles=resolvers.scanfiles
local registerfilehash=resolvers.registerfilehash
local appendhash=resolvers.appendhash
local loadcachecontent=caches.loadcontent
local checkgarbage=utilities.garbagecollector and utilities.garbagecollector.check
function resolvers.locators.file(specification)
 local filename=specification.filename
 local realname=resolveprefix(filename) 
 if realname and realname~='' and isdir(realname) then
  if trace_locating then
   report_files("file locator %a found as %a",filename,realname)
  end
  appendhash('file',filename,true) 
 elseif trace_locating then
  report_files("file locator %a not found",filename)
 end
end
function resolvers.hashers.file(specification)
 local pathname=specification.filename
 local content=loadcachecontent(pathname,'files')
 registerfilehash(pathname,content,content==nil)
end
function resolvers.generators.file(specification)
 local pathname=specification.filename
 local content=scanfiles(pathname,false,true) 
 registerfilehash(pathname,content,true)
end
resolvers.concatinators.file=file.join
local finders=resolvers.finders
local notfound=finders.notfound
function finders.file(specification,filetype)
 local filename=specification.filename
 local foundname=findfile(filename,filetype)
 if foundname and foundname~="" then
  if trace_locating then
   report_files("file finder: %a found",filename)
  end
  return foundname
 else
  if trace_locating then
   report_files("file finder: %a not found",filename)
  end
  return notfound()
 end
end
local openers=resolvers.openers
local notfound=openers.notfound
local overloaded=false
local function textopener(tag,filename,f)
 return {
  reader=function() return f:read () end,
  close=function() return f:close() end,
 }
end
function openers.helpers.textopener(...)
 return textopener(...)
end
function openers.helpers.settextopener(opener)
 if overloaded then
  report_files("file opener: %s overloaded","already")
 else
  if trace_locating then
   report_files("file opener: %s overloaded","once")
  end
  overloaded=true
  textopener=opener
 end
end
function openers.file(specification,filetype)
 local filename=specification.filename
 if filename and filename~="" then
  local f=ioopen(filename,"r")
  if f then
   if trace_locating then
    report_files("file opener: %a opened",filename)
   end
   return textopener("file",filename,f)
  end
 end
 if trace_locating then
  report_files("file opener: %a not found",filename)
 end
 return notfound()
end
local loaders=resolvers.loaders
local notfound=loaders.notfound
function loaders.file(specification,filetype)
 local filename=specification.filename
 if filename and filename~="" then
  local f=ioopen(filename,"rb")
  if f then
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
 return notfound()
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-con"] = package.loaded["data-con"] or true

-- original size: 5388, stripped down to: 3685

if not modules then modules={} end modules ['data-con']={
 version=1.100,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local setmetatable=setmetatable
local format,lower,gsub=string.format,string.lower,string.gsub
local trace_cache=false  trackers.register("resolvers.cache",function(v) trace_cache=v end)
local trace_containers=false  trackers.register("resolvers.containers",function(v) trace_containers=v end)
local trace_storage=false  trackers.register("resolvers.storage",function(v) trace_storage=v end)
containers=containers or {}
local containers=containers
containers.usecache=true
local getwritablepath=caches.getwritablepath
local getreadablepaths=caches.getreadablepaths
local cacheiswritable=caches.is_writable
local loaddatafromcache=caches.loaddata
local savedataincache=caches.savedata
local report_containers=logs.reporter("resolvers","containers")
local allocated={}
local mt={
 __index=function(t,k)
  if k=="writable" then
   local writable=getwritablepath(t.category,t.subcategory) or { "." }
   t.writable=writable
   return writable
  elseif k=="readables" then
   local readables=getreadablepaths(t.category,t.subcategory) or { "." }
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
 return container.enabled and caches and cacheiswritable(container.writable,name)
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
  stored=loaddatafromcache(container.readables,name,container.writable)
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
function containers.write(container,name,data,fast)
 if data then
  data.cache_version=container.version
  if container.enabled and caches then
   local unique=data.unique
   local shared=data.shared
   data.unique=nil
   data.shared=nil
   savedataincache(container.writable,name,data,fast)
   if trace_cache or trace_containers then
    report_containers("action %a, category %a, name %a","save",container.subcategory,name)
   end
   data.unique=unique
   data.shared=shared
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
 return (gsub(lower(name),"[^%w\128-\255]+","-")) 
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-use"] = package.loaded["data-use"] or true

-- original size: 5790, stripped down to: 2910

if not modules then modules={} end modules ['data-use']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local format=string.format
local trace_locating=false  trackers.register("resolvers.locating",function(v) trace_locating=v end)
local report_mounts=logs.reporter("resolvers","mounts")
local resolvers=resolvers
local findfile=resolvers.findfile
statistics.register("used config file",function() return caches.configfiles() end)
statistics.register("used cache path",function() return caches.usedpaths() end)
function statistics.savefmtstatus(texname,formatbanner,sourcefile,kind,banner) 
 local enginebanner=status.banner
 if formatbanner and enginebanner and sourcefile then
  local luvname=file.replacesuffix(texname,"luv") 
  local luvdata={
   enginebanner=enginebanner,
   formatbanner=formatbanner,
   sourcehash=md5.hex(io.loaddata(findfile(sourcefile)) or "unknown"),
   sourcefile=sourcefile,
   luaversion=LUAVERSION,
   formatid=LUATEXFORMATID,
   functionality=LUATEXFUNCTIONALITY,
  }
  io.savedata(luvname,table.serialize(luvdata,true))
  lua.registerfinalizer(function()
   if jit then
    logs.report("format banner","%s  lua: %s jit",banner,LUAVERSION)
   else
    logs.report("format banner","%s  lua: %s",banner,LUAVERSION)
   end
   logs.newline()
  end)
 end
end
function statistics.checkfmtstatus(texname)
 local enginebanner=status.banner
 if enginebanner and texname then
  local luvname=file.replacesuffix(texname,"luv") 
  if lfs.isfile(luvname) then
   local luv=dofile(luvname)
   if luv and luv.sourcefile then
    local sourcehash=md5.hex(io.loaddata(findfile(luv.sourcefile)) or "unknown")
    local luvbanner=luv.enginebanner or "?"
    if luvbanner~=enginebanner then
     return format("engine mismatch (luv: %s <> bin: %s)",luvbanner,enginebanner)
    end
    local luvhash=luv.sourcehash or "?"
    if luvhash~=sourcehash then
     return format("source mismatch (luv: %s <> bin: %s)",luvhash,sourcehash)
    end
    local luvluaversion=luv.luaversion or 0
    local engluaversion=LUAVERSION or 0
    if luvluaversion~=engluaversion then
     return format("lua mismatch (luv: %s <> bin: %s)",luvluaversion,engluaversion)
    end
    local luvfunctionality=luv.functionality or 0
    local engfunctionality=status.development_id or 0
    if luvfunctionality~=engfunctionality then
     return format("functionality mismatch (luv: %s <> bin: %s)",luvfunctionality,engfunctionality)
    end
    local luvformatid=luv.formatid or 0
    local engformatid=status.format_id or 0
    if luvformatid~=engformatid then
     return format("formatid mismatch (luv: %s <> bin: %s)",luvformatid,engformatid)
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

-- original size: 10725, stripped down to: 7949

if not modules then modules={} end modules ['data-zip']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local format,find,match=string.format,string.find,string.match
local trace_locating=false  trackers.register("resolvers.locating",function(v) trace_locating=v end)
local report_zip=logs.reporter("resolvers","zip")
local resolvers=resolvers
local findfile=resolvers.findfile
local registerfile=resolvers.registerfile
local splitmethod=resolvers.splitmethod
local prependhash=resolvers.prependhash
local starttiming=resolvers.starttiming
local extendtexmf=resolvers.extendtexmfvariable
local stoptiming=resolvers.stoptiming
local urlquery=url.query
zip=zip or {}
local zip=zip
local archives=zip.archives or {}
zip.archives=archives
local registeredfiles=zip.registeredfiles or {}
zip.registeredfiles=registeredfiles
local zipfiles=utilities.zipfiles
local openzip,closezip,validfile,wholefile,filehandle,traversezip
if zipfiles then
 local ipairs=ipairs
 openzip=zipfiles.open
 closezip=zipfiles.close
 validfile=zipfiles.found
 wholefile=zipfiles.unzip
 local listzip=zipfiles.list
 traversezip=function(zfile)
  return ipairs(listzip(zfile))
 end
 local streams=utilities.streams
 local openstream=streams.open
 local readstring=streams.readstring
 local streamsize=streams.size
 local metatable={
  close=streams.close,
  read=function(stream,n)
   readstring(stream,n=="*a" and streamsize(stream) or n)
  end
 }
 filehandle=function(zfile,queryname)
  local data=wholefile(zfile,queryname)
  if data then
   local stream=openstream(data)
   if stream then
    return setmetatableindex(stream,metatable)
   end
  end
 end
else
 openzip=zip.open
 closezip=zip.close
 validfile=function(zfile,queryname)
  local dfile=zfile:open(queryname)
  if dfile then
   dfile:close()
   return true
  end
  return false
 end
 traversezip=function(zfile)
  return z:files()
 end
 wholefile=function(zfile,queryname)
  local dfile=zfile:open(queryname)
  if dfile then
   local s=dfile:read("*all")
   dfile:close()
   return s
  end
 end
 filehandle=function(zfile,queryname)
  local dfile=zfile:open(queryname)
  if dfile then
   return dfile
  end
 end
end
local function validzip(str) 
 if not find(str,"^zip://") then
  return "zip:///"..str
 else
  return str
 end
end
local function openarchive(name)
 if not name or name=="" then
  return nil
 else
  local arch=archives[name]
  if not arch then
     local full=findfile(name) or ""
     arch=full~="" and openzip(full) or false
     archives[name]=arch
  end
    return arch
 end
end
local function closearchive(name)
 if not name or (name=="" and archives[name]) then
  closezip(archives[name])
  archives[name]=nil
 end
end
zip.openarchive=openarchive
zip.closearchive=closearchive
function resolvers.locators.zip(specification)
 local archive=specification.filename
 local zipfile=archive and archive~="" and openarchive(archive) 
 if trace_locating then
  if zipfile then
   report_zip("locator: archive %a found",archive)
  else
   report_zip("locator: archive %a not found",archive)
  end
 end
end
function resolvers.concatinators.zip(zipfile,path,name) 
 if not path or path=="" then
  return format('%s?name=%s',zipfile,name)
 else
  return format('%s?name=%s/%s',zipfile,path,name)
 end
end
local finders=resolvers.finders
local notfound=finders.notfound
function finders.zip(specification)
 local original=specification.original
 local archive=specification.filename
 if archive then
  local query=urlquery(specification.query)
  local queryname=query.name
  if queryname then
   local zfile=openarchive(archive)
   if zfile then
    if trace_locating then
     report_zip("finder: archive %a found",archive)
    end
    if validfile(zfile,queryname) then
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
 return notfound()
end
local openers=resolvers.openers
local notfound=openers.notfound
local textopener=openers.helpers.textopener
function openers.zip(specification)
 local original=specification.original
 local archive=specification.filename
 if archive then
  local query=urlquery(specification.query)
  local queryname=query.name
  if queryname then
   local zfile=openarchive(archive)
   if zfile then
    if trace_locating then
     report_zip("opener; archive %a opened",archive)
    end
    local handle=filehandle(zfile,queryname)
    if handle then
     if trace_locating then
      report_zip("opener: file %a found",queryname)
     end
     return textopener('zip',original,handle)
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
 return notfound()
end
local loaders=resolvers.loaders
local notfound=loaders.notfound
function loaders.zip(specification)
 local original=specification.original
 local archive=specification.filename
 if archive then
  local query=urlquery(specification.query)
  local queryname=query.name
  if queryname then
   local zfile=openarchive(archive)
   if zfile then
    if trace_locating then
     report_zip("loader: archive %a opened",archive)
    end
    local data=wholefile(zfile,queryname)
    if data then
     if trace_locating then
      report_zip("loader; file %a loaded",original)
     end
     return true,data,#data
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
 return notfound()
end
local function registerzipfile(z,tree)
 local names={}
 local files={} 
 local remap={} 
 local n=0
 local filter=tree=="" and "^(.+)/(.-)$" or format("^%s/(.+)/(.-)$",tree)
 if trace_locating then
  report_zip("registering: using filter %a",filter)
 end
 starttiming()
 for i in traversezip(z) do
  local filename=i.filename
  local path,name=match(filename,filter)
  if not path then
   n=n+1
   registerfile(names,filename,"")
   local usedname=lower(filename)
   files[usedname]=""
   if usedname~=filename then
    remap[usedname]=filename
   end
  elseif name and name~="" then
   n=n+1
   register(names,name,path)
   local usedname=lower(name)
   files[usedname]=path
   if usedname~=name then
    remap[usedname]=name
   end
  else
  end
 end
 stoptiming()
 report_zip("registering: %s files registered",n)
 return {
  files=files,
  remap=remap,
 }
end
local function usezipfile(archive)
 local specification=splitmethod(archive) 
 local archive=specification.filename
 if archive and not registeredfiles[archive] then
  local z=openarchive(archive)
  if z then
   local tree=urlquery(specification.query).tree or ""
   if trace_locating then
    report_zip("registering: archive %a",archive)
   end
   prependhash('zip',archive)
   extendtexmf(archive) 
   registeredfiles[archive]=z
   registerfilehash(archive,registerzipfile(z,tree))
  elseif trace_locating then
   report_zip("registering: unknown archive %a",archive)
  end
 elseif trace_locating then
  report_zip("registering: archive %a not found",archive)
 end
end
resolvers.usezipfile=usezipfile
resolvers.registerzipfile=registerzipfile
function resolvers.hashers.zip(specification)
 local archive=specification.filename
 if trace_locating then
  report_zip("loading file %a",archive)
 end
 usezipfile(specification.original)
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-tre"] = package.loaded["data-tre"] or true

-- original size: 10802, stripped down to: 6619

if not modules then modules={} end modules ['data-tre']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local type=type
local find,gsub,lower=string.find,string.gsub,string.lower
local basename,dirname,joinname=file.basename,file.dirname,file.join
local globdir,isdir,isfile=dir.glob,lfs.isdir,lfs.isfile
local P,lpegmatch=lpeg.P,lpeg.match
local trace_locating=false  trackers.register("resolvers.locating",function(v) trace_locating=v end)
local report_trees=logs.reporter("resolvers","trees")
local resolvers=resolvers
local finders=resolvers.finders
local openers=resolvers.openers
local loaders=resolvers.loaders
local locators=resolvers.locators
local hashers=resolvers.hashers
local generators=resolvers.generators
do
 local collectors={}
 local found={}
 local notfound=finders.notfound
 function finders.tree(specification) 
  local spec=specification.filename
  local okay=found[spec]
  if okay==nil then
   if spec~="" then
    local path=dirname(spec)
    local name=basename(spec)
    if path=="" then
     path="."
    end
    local names=collectors[path]
    if not names then
     local pattern=find(path,"/%*+$") and path or (path.."/*")
     names=globdir(pattern)
     collectors[path]=names
    end
    local pattern="/"..gsub(name,"([%.%-%+])","%%%1").."$"
    for i=1,#names do
     local fullname=names[i]
     if find(fullname,pattern) then
      found[spec]=fullname
      return fullname
     end
    end
    local pattern=lower(pattern)
    for i=1,#names do
     local fullname=lower(names[i])
     if find(fullname,pattern) then
      if isfile(fullname) then
       found[spec]=fullname
       return fullname
      else
       break
      end
     end
    end
   end
   okay=notfound() 
   found[spec]=okay
  end
  return okay
 end
end
do
 local resolveprefix=resolvers.resolve
 local appendhash=resolvers.appendhash
 local function dolocate(specification)
  local name=specification.filename
  local realname=resolveprefix(name) 
  if realname and realname~='' and isdir(realname) then
   if trace_locating then
    report_trees("locator %a found",realname)
   end
   appendhash('tree',name,false) 
  elseif trace_locating then
   report_trees("locator %a not found",name)
  end
 end
 locators.tree=dolocate
 locators.dirlist=dolocate
 locators.dirfile=dolocate
end
do
 local filegenerator=generators.file
 generators.dirlist=filegenerator
 generators.dirfile=filegenerator
end
do
 local filegenerator=generators.file
 local methodhandler=resolvers.methodhandler
 local function dohash(specification)
  local name=specification.filename
  if trace_locating then
   report_trees("analyzing %a",name)
  end
  methodhandler("hashers",name)
  filegenerator(specification)
 end
 hashers.tree=dohash
 hashers.dirlist=dohash
 hashers.dirfile=dohash
end
local resolve  do
 local collectors={}
 local splitter=lpeg.splitat("/**/")
 local stripper=lpeg.replacer { [P("/")*P("*")^1*P(-1)]="" }
 local loadcontent=caches.loadcontent
 local savecontent=caches.savecontent
 local notfound=finders.notfound
 local scanfiles=resolvers.scanfiles
 local lookup=resolvers.get_from_content
 table.setmetatableindex(collectors,function(t,k)
  local rootname=lpegmatch(stripper,k)
  local dataname=joinname(rootname,"dirlist")
  local content=loadcontent(dataname,"files",dataname)
  if not content then
   content=scanfiles(rootname,nil,nil,false,true) 
   savecontent(dataname,"files",content,dataname)
  end
  t[k]=content
  return content
 end)
 local function checked(root,p,n)
  if p then
   if type(p)=="table" then
    for i=1,#p do
     local fullname=joinname(root,p[i],n)
     if isfile(fullname) then 
      return fullname
     end
    end
   else
    local fullname=joinname(root,p,n)
    if isfile(fullname) then 
     return fullname
    end
   end
  end
  return notfound()
 end
 resolve=function(specification) 
  local filename=specification.filename
  if filename~="" then
   local root,rest=lpegmatch(splitter,filename)
   if root and rest then
    local path,name=dirname(rest),basename(rest)
    if name~=rest then
     local content=collectors[root]
     local p,n=lookup(content,name)
     if not p then
      return notfound()
     end
     local pattern=".*/"..path.."$"
     local istable=type(p)=="table"
     if istable then
      for i=1,#p do
       local pi=p[i]
       if pi==path or find(pi,pattern) then
        local fullname=joinname(root,pi,n)
        if isfile(fullname) then 
         return fullname
        end
       end
      end
     elseif p==path or find(p,pattern) then
      local fullname=joinname(root,p,n)
      if isfile(fullname) then 
       return fullname
      end
     end
     local queries=specification.queries
     if queries and queries.option=="fileonly" then
      return checked(root,p,n)
     else
      return notfound()
     end
    end
   end
   local path=dirname(filename)
   local name=basename(filename)
   local root=lpegmatch(stripper,path)
   local content=collectors[path]
   local p,n=lookup(content,name)
   if p then
    return checked(root,p,n)
   end
  end
  return notfound()
 end
 finders.dirlist=resolve
 function finders.dirfile(specification)
  local queries=specification.queries
  if queries then
   queries.option="fileonly"
  else
   specification.queries={ option="fileonly" }
  end
  return resolve(specification)
 end
end
do
 local fileopener=openers.file
 local fileloader=loaders.file
 openers.dirlist=fileopener
 loaders.dirlist=fileloader
 openers.dirfile=fileopener
 loaders.dirfile=fileloader
end
do
 local hashfile="dirhash.lua"
 local kind="HASH256"
 local version=1.0
 local loadtable=table.load
 local savetable=table.save
 local loaddata=io.loaddata
 function resolvers.dirstatus(patterns)
  local t=type(patterns)
  if t=="string" then
   patterns={ patterns }
  elseif t~="table" then
   return false
  end
  local status=loadtable(hashfile)
  if not status or status.version~=version or status.kind~=kind then
   status={
    version=1.0,
    kind=kind,
    hashes={},
   }
  end
  local hashes=status.hashes
  local changed={}
  local action=sha2[kind]
  local update={}
  for i=1,#patterns do
   local pattern=patterns[i]
   local files=globdir(pattern)
   for i=1,#files do
    local name=files[i]
    local hash=action(loaddata(name))
    if hashes[name]~=hash then
     changed[#changed+1]=name
    end
    update[name]=hash
   end
  end
  status.hashes=update
  savetable(hashfile,status)
  return #changed>0 and changed or false
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-sch"] = package.loaded["data-sch"] or true

-- original size: 6945, stripped down to: 5408

if not modules then modules={} end modules ['data-sch']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local load,tonumber=load,tonumber
local gsub,format=string.gsub,string.format
local sortedhash,concat=table.sortedhash,table.concat
local finders,openers,loaders=resolvers.finders,resolvers.openers,resolvers.loaders
local addsuffix,suffix,splitbase=file.addsuffix,file.suffix,file.splitbase
local md5hex=md5.hex
local trace_schemes=false  trackers.register("resolvers.schemes",function(v) trace_schemes=v end)
local report_schemes=logs.reporter("resolvers","schemes")
local http=require("socket.http")
local ltn12=require("ltn12")
if mbox then mbox=nil end 
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
 local path,name=splitbase(specification.original)
 if path=="" then
  return (gsub(name,"[^%a%d%.]+","-"))
 else
  return (gsub((gsub(path,"%.","-").."-"..name),"[^%a%d%.]+","-"))
 end
end
function cleaners.md5(specification)
 return addsuffix(md5hex(specification.original),suffix(specification.path))
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
local cached={}
local loaded={}
local reused={}
local thresholds={}
local handlers={}
local runner=sandbox.registerrunner {
 name="curl resolver",
 method="execute",
 program="curl",
 template='--silent --insecure --create-dirs --output "%cachename%" "%original%"',
 checkers={
  cachename="cache",
  original="url",
 }
}
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
    runner {
     original=original,
     cachename=cachename,
    }
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
 handlers  [scheme]=handler
 loaded [scheme]=0
 reused [scheme]=0
 finders   [scheme]=finder
 openers   [scheme]=opener
 loaders   [scheme]=loader
 thresholds[scheme]=newthreshold or threshold
end
schemes.install=install
local function http_handler(specification,cachename)
 local tempname=cachename..".tmp"
 local handle=io.open(tempname,"wb")
 local status,message=http.request {
  url=specification.original,
  sink=ltn12.sink.file(handle)
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
 for k,v in sortedhash(loaded) do
  if v>0 then
   nl=nl+1
   l[nl]=k..":"..v
  end
 end
 for k,v in sortedhash(reused) do
  if v>0 then
   nr=nr+1
   r[nr]=k..":"..v
  end
 end
 local n=nl+nr
 if n>0 then
  if nl==0 then l={ "none" } end
  if nr==0 then r={ "none" } end
  return format("%s seconds, %s processed, threshold %s seconds, loaded: %s, reused: %s",
   statistics.elapsedtime(schemes),n,threshold,concat(l," "),concat(l," "))
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

-- original size: 4227, stripped down to: 3049

if not modules then modules={} end modules ['data-lua']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local package,lpeg=package,lpeg
local loadfile=loadfile
local addsuffix=file.addsuffix
local P,S,Cs,lpegmatch=lpeg.P,lpeg.S,lpeg.Cs,lpeg.match
local luasuffixes={ 'tex','lua' }
local libsuffixes={ 'lib' }
local luaformats={ 'TEXINPUTS','LUAINPUTS' }
local libformats={ 'CLUAINPUTS' }
local helpers=package.helpers or {}
local methods=helpers.methods or {}
local resolvers=resolvers
local resolveprefix=resolvers.resolve
local expandedpaths=resolvers.expandedpathlistfromvariable
local findfile=resolvers.findfile
helpers.report=logs.reporter("resolvers","libraries")
trackers.register("resolvers.libraries",function(v) helpers.trace=v end)
trackers.register("resolvers.locating",function(v) helpers.trace=v end)
helpers.sequence={
 "already loaded",
 "preload table",
 "lua variable format",
 "lib variable format",
 "lua extra list",
 "lib extra list",
 "path specification",
 "cpath specification",
 "all in one fallback",
 "not loaded",
}
local pattern=Cs(P("!")^0/""*(P("/")*P(-1)/"/"+P("/")^1/"/"+1)^0)
function helpers.cleanpath(path) 
 return resolveprefix(lpegmatch(pattern,path))
end
local loadedaslib=helpers.loadedaslib
local registerpath=helpers.registerpath
local lualibfile=helpers.lualibfile
local luaformatpaths
local libformatpaths
local function getluaformatpaths()
 if not luaformatpaths then
  luaformatpaths={}
  for i=1,#luaformats do
   registerpath("lua format","lua",luaformatpaths,expandedpaths(luaformats[i]))
  end
 end
 return luaformatpaths
end
local function getlibformatpaths()
 if not libformatpaths then
  libformatpaths={}
  for i=1,#libformats do
   registerpath("lib format","lib",libformatpaths,expandedpaths(libformats[i]))
  end
 end
 return libformatpaths
end
local function loadedbyformat(name,rawname,suffixes,islib,what)
 local trace=helpers.trace
 local report=helpers.report
 for i=1,#suffixes do 
  local format=suffixes[i]
  local resolved=findfile(name,format) or ""
  if trace then
   report("%s format, identifying %a using format %a",what,name,format)
  end
  if resolved~="" then
   if trace then
    report("%s format, %a found on %a",what,name,resolved)
   end
   if islib then
    return loadedaslib(resolved,rawname)
   else
    return loadfile(resolved)
   end
  end
 end
end
helpers.loadedbyformat=loadedbyformat
methods["lua variable format"]=function(name)
 if helpers.trace then
  helpers.report("%s format, checking %s paths","lua",#getluaformatpaths()) 
 end
 return loadedbyformat(addsuffix(lualibfile(name),"lua"),name,luasuffixes,false,"lua")
end
methods["lib variable format"]=function(name)
 if helpers.trace then
  helpers.report("%s format, checking %s paths","lib",#getlibformatpaths()) 
 end
 return loadedbyformat(addsuffix(lualibfile(name),os.libsuffix),name,libsuffixes,true,"lib")
end
resolvers.loadlualib=require 


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["data-aux"] = package.loaded["data-aux"] or true

-- original size: 2610, stripped down to: 2019

if not modules then modules={} end modules ['data-aux']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local find=string.find
local type,next=type,next
local addsuffix,removesuffix=file.addsuffix,file.removesuffix
local loaddata,savedata=io.loaddata,io.savedata
local trace_locating=false  trackers.register("resolvers.locating",function(v) trace_locating=v end)
local resolvers=resolvers
local cleanpath=resolvers.cleanpath
local findfiles=resolvers.findfiles
local report_scripts=logs.reporter("resolvers","scripts")
function resolvers.updatescript(oldname,newname)
 local scriptpath="context/lua"
 local oldscript=cleanpath(oldname)
 local newname=addsuffix(newname,"lua")
 local newscripts=findfiles(newname) or {}
 if trace_locating then
  report_scripts("to be replaced old script %a",oldscript)
 end
 if #newscripts==0 then
  if trace_locating then
   report_scripts("unable to locate new script")
  end
 else
  for i=1,#newscripts do
   local newscript=cleanpath(newscripts[i])
   if trace_locating then
    report_scripts("checking new script %a",newscript)
   end
   if oldscript==newscript then
    if trace_locating then
     report_scripts("old and new script are the same")
    end
   elseif not find(newscript,scriptpath,1,true) then
    if trace_locating then
     report_scripts("new script should come from %a",scriptpath)
    end
   elseif not (find(oldscript,removesuffix(newname).."$") or find(oldscript,newname.."$")) then
    if trace_locating then
     report_scripts("invalid new script name")
    end
   else
    local newdata=loaddata(newscript)
    if newdata then
     if trace_locating then
      report_scripts("old script content replaced by new content: %s",oldscript)
     end
     savedata(oldscript,newdata)
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

-- original size: 2601, stripped down to: 1549

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

-- original size: 2038, stripped down to: 1696

if not modules then modules={} end modules ['data-lst']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local type=type
local sortedhash=table.sortedhash
local isdir=lfs.isdir
local resolvers=resolvers
local listers=resolvers.listers or {}
resolvers.listers=listers
local resolveprefix=resolvers.resolve
local configurationfiles=resolvers.configurationfiles
local expandedpathfromlist=resolvers.expandedpathfromlist
local splitpath=resolvers.splitpath
local knownvariables=resolvers.knownvariables
local report_lists=logs.reporter("resolvers","lists")
local report_resolved=logs.reporter("system","resolved")
local function tabstr(str)
 if not str then
  return "unset"
 elseif type(str)=='table' then
  return concat(str," | ")
 else
  return str
 end
end
function listers.variables(pattern)
 local result=resolvers.knownvariables(pattern)
 for key,value in sortedhash(result) do
  report_lists(key)
  report_lists("  env: %s",tabstr(value.environment))
  report_lists("  var: %s",tabstr(value.variable))
  report_lists("  exp: %s",tabstr(value.expansion))
  report_lists("  res: %s",tabstr(value.resolved))
 end
end
function listers.configurations()
 local configurations=configurationfiles()
 for i=1,#configurations do
  report_resolved("file : %s",resolveprefix(configurations[i]))
 end
 report_resolved("")
 local list=expandedpathfromlist(splitpath(resolvers.luacnfspec))
 for i=1,#list do
  local li=resolveprefix(list[i])
  if isdir(li) then
   report_resolved("path - %s",li)
  else
   report_resolved("path + %s",li)
  end
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["libs-ini"] = package.loaded["libs-ini"] or true

-- original size: 5822, stripped down to: 3629

if not modules then modules={} end modules ['libs-ini']={
 version=1.001,
 comment="companion to luat-lib.mkiv",
 author="Hans Hagen, PRAGMA-ADE, Hasselt NL",
 copyright="PRAGMA ADE / ConTeXt Development Team",
 license="see context related readme files"
}
local type,unpack=type,unpack
local type=type
local nameonly=file.nameonly
local joinfile=file.join
local addsuffix=file.addsuffix
local qualifiedpath=file.is_qualified_path
local isfile=lfs.isfile
local findfile=resolvers.findfile
local expandpaths=resolvers.expandedpathlistfromvariable
local report=logs.reporter("resolvers","libraries")
local trace=false
trackers.register("resolvers.lib",function(v) trace=v end)
local function findlib(required) 
 local suffix=os.libsuffix or "so"
 if not qualifiedpath(required) then
  local list=directives.value("system.librarynames" )
  local only=nameonly(required)
  if type(list)=="table" then
   list=list[only]
   if type(list)~="table" then
    list={ only }
   end
  else
   list={ only }
  end
  if trace then
   report("using lookup list for library %a: % | t",only,list)
  end
  for i=1,#list do
   local name=list[i]
   local found=findfile(name,"lib")
   if not found then
    found=findfile(addsuffix(name,suffix),"lib")
   end
   if found then
    if trace then
     report("library %a resolved via %a path to %a",name,"tds lib",found)
    end
    return found
   end
  end
  if expandpaths then
   local list=expandpaths("PATH")
   local base=addsuffix(only,suffix)
   for i=1,#list do
    local full=joinfile(list[i],base)
    local found=isfile(full) and full
    if found then
     if trace then
      report("library %a resolved via %a path to %a",name,"system",found)
     end
     return found
    end
   end
  end
 elseif isfile(addsuffix(required,suffix)) then
  if trace then
   report("library with qualified name %a %sfound",required,"")
  end
  return required
 else
  if trace then
   report("library with qualified name %a %sfound",required,"not ")
  end
 end
 return false
end
local foundlibraries=table.setmetatableindex(function(t,k)
 local v=findlib(k)
 t[k]=v
 return v
end)
function resolvers.findlib(required)
 return foundlibraries[required]
end
local libraries={}
resolvers.libraries=libraries
local report=logs.reporter("optional")
if optional then optional.loaded={} end
function libraries.validoptional(name)
 local thelib=optional and optional[name]
 if not thelib then
 elseif thelib.initialize then
  return thelib
 else
  report("invalid optional library %a",libname)
 end
end
function libraries.optionalloaded(name,libnames)
 local thelib=optional and optional[name]
 if not thelib then
  report("no optional %a library found",name)
 else
  local thelib_initialize=thelib.initialize
  if not thelib_initialize then
   report("invalid optional library %a",name)
  else
   if type(libnames)=="string" then
    libnames={ libnames }
   end
   if type(libnames)=="table" then
    for i=1,#libnames do
     local libname=libnames[i]
     local filename=foundlibraries[libname]
     if filename then
      libnames[i]=filename
     else
      report("unable to locate library %a",libname)
      return
     end
    end
    local initialized=thelib_initialize(unpack(libnames))
    if initialized then
     report("using library '% + t'",libnames)
    else
     report("unable to initialize library '% + t'",libnames)
    end
    return initialized
   end
  end
 end
end
if FFISUPPORTED and ffi and ffi.load then
 local ffiload=ffi.load
 function ffi.load(name)
  local full=name and foundlibraries[name]
  if full then
   return ffiload(full)
  else
   return ffiload(name)
  end
 end
end


end -- of closure

do -- create closure to overcome 200 locals limit

package.loaded["luat-sta"] = package.loaded["luat-sta"] or true

-- original size: 5703, stripped down to: 2321

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

-- original size: 13843, stripped down to: 9991

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
local function primaryflags(arguments)
 local flags={}
 if arguments.silent then
  flags[#flags+1]="--interaction=batchmode"
 end
 return concat(flags," ")
end
local function secondaryflags(arguments)
 local trackers=arguments.trackers
 local directives=arguments.directives
 local flags={}
 if trackers and trackers~="" then
  flags[#flags+1]="--c:trackers="..quoted(trackers)
 end
 if directives and directives~="" then
  flags[#flags+1]="--c:directives="..quoted(directives)
 end
 if arguments.silent then
  flags[#flags+1]="--c:silent"
 end
 if arguments.errors then
  flags[#flags+1]="--c:errors"
 end
 if arguments.ansi then
  flags[#flags+1]="--c:ansi"
 end
 if arguments.ansilog then
  flags[#flags+1]="--c:ansilog"
 end
 if arguments.strip then
  flags[#flags+1]="--c:strip"
 end
 if arguments.lmtx then
  flags[#flags+1]="--c:lmtx"
 end
 return concat(flags," ")
end
local template=[[--ini %primaryflags% --lua=%luafile% %texfile% %secondaryflags% %dump% %redirect%]]
local checkers={
 primaryflags="verbose",
 secondaryflags="verbose",
 luafile="readable",
 texfile="readable",
 redirect="string",
 dump="string",
 binarypath="string",
}
local runners={
 luatex=sandbox.registerrunner {
  name="make luatex format",
  program="luatex",
  template=template,
  checkers=checkers,
  reporter=report_format,
 },
 luametatex=sandbox.registerrunner {
  name="make luametatex format",
  program="luametatex",
  template=template,
  checkers=checkers,
  reporter=report_format,
 },
 luajittex=sandbox.registerrunner {
  name="make luajittex format",
  program="luajittex",
  template=template,
  checkers=checkers,
  reporter=report_format,
 },
}
local function validbinarypath()
 if not environment.arguments.nobinarypath then
  local path=environment.ownpath or file.dirname(environment.ownname)
  if path and path~="" then
   path=dir.expandname(path)
   if path~="" and lfs.isdir(path) then
    return path
   end
  end
 end
end
function environment.make_format(formatname)
 local arguments=environment.arguments
 local engine=environment.ownmain or "luatex"
 local silent=arguments.silent
 local errors=arguments.errors
 local texsourcename=""
 local texsourcepath=""
 local fulltexsourcename=""
 if engine=="luametatex" then
  texsourcename=file.addsuffix(formatname,"mkxl")
  fulltexsourcename=resolvers.findfile(texsourcename,"tex") or ""
 end
 if fulltexsourcename=="" then
  texsourcename=file.addsuffix(formatname,"mkiv")
  fulltexsourcename=resolvers.findfile(texsourcename,"tex") or ""
 end
 if fulltexsourcename=="" then
  texsourcename=file.addsuffix(formatname,"tex")
  fulltexsourcename=resolvers.findfile(texsourcename,"tex") or ""
 end
 if fulltexsourcename=="" then
  report_format("no tex source file with name %a (mkiv or tex)",formatname)
  return
 end
 report_format("using tex source file %a",fulltexsourcename)
 fulltexsourcename=dir.expandname(fulltexsourcename)
 texsourcepath=file.dirname(fulltexsourcename)
 if not lfs.isfile(fulltexsourcename) then
  report_format("no accessible tex source file with name %a",fulltexsourcename)
  return
 end
 local specificationname="context.lus"
 local specificationpath=""
 local fullspecificationname=resolvers.findfile(specificationname) or ""
 if fullspecificationname=="" then
  report_format("unable to locate specification file %a",specificationname)
  return
 end
 report_format("using specification file %a",fullspecificationname)
 fullspecificationname=dir.expandname(fullspecificationname)
 specificationpath=file.dirname(fullspecificationname)
 if texsourcepath~=specificationpath then
  report_format("tex source file and specification file are on different paths")
  return
 end
 if not lfs.isfile(fulltexsourcename) then
  report_format("no accessible tex source file with name %a",fulltexsourcename)
  return
 end
 if not lfs.isfile(fullspecificationname) then
  report_format("no accessible specification file with name %a",fulltexsourcename)
  return
 end
 report_format("using tex source path %a",texsourcepath)
 local validformatpath=caches.getwritablepath("formats",engine) or ""
 local startupdir=dir.current()
 if validformatpath=="" then
  report_format("invalid format path, insufficient write access")
  return
 end
 local binarypath=validbinarypath()
 report_format("changing to format path %a",validformatpath)
 lfs.chdir(validformatpath)
 if dir.current()~=validformatpath then
  report_format("unable to change to format path %a",validformatpath)
  return
 end
 local usedluastub=nil
 local usedlualibs=dofile(fullspecificationname)
 if type(usedlualibs)=="string" then
  usedluastub=file.join(specificationpath,usedlualibs)
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
  lfs.chdir(startupdir)
  return
 end
 local runner=runners[engine]
 if not runner then
  report_format("the format %a cannot be generated, no runner available for engine %a",name,engine)
  lfs.chdir(startupdir)
  return
 end
 local primaryflags=primaryflags(arguments)
 local secondaryflags=secondaryflags(arguments)
 local specification={
  binarypath=binarypath,
  primaryflags=primaryflags,
  secondaryflags=secondaryflags,
  luafile=quoted(usedluastub),
  texfile=quoted(fulltexsourcename),
  dump=os.platform=="unix" and "\\\\dump" or "\\dump",
 }
 if silent then
  specification.redirect="> temp.log"
 end
 statistics.starttiming("format")
 local result=runner(specification)
 statistics.stoptiming("format")
 if silent then
  os.remove("temp.log")
 end
 report_format()
  if binarypath and binarypath~="" then
 report_format("binary path      : %s",binarypath or "?")
  end
 report_format("format path      : %s",validformatpath)
 report_format("luatex engine    : %s",engine)
 report_format("lua startup file : %s",usedluastub)
  if primaryflags~="" then
 report_format("primary flags    : %s",primaryflags)
  end
  if secondaryflags~="" then
 report_format("secondary flags  : %s",secondaryflags)
  end
 report_format("context file     : %s",fulltexsourcename)
 report_format("run time         : %.3f seconds",statistics.elapsed("format"))
 report_format("return value     : %s",result==0 and "okay" or "error")
 report_format()
 lfs.chdir(startupdir)
end
local template=[[%primaryflags% --fmt=%fmtfile% --lua=%luafile% %texfile% %secondaryflags%]]
local checkers={
 primaryflags="verbose",
 secondaryflags="verbose",
 fmtfile="readable",
 luafile="readable",
 texfile="readable",
}
local runners={
 luatex=sandbox.registerrunner {
  name="run luatex format",
  program="luatex",
  template=template,
  checkers=checkers,
  reporter=report_format,
 },
 luametatex=sandbox.registerrunner {
  name="run luametatex format",
  program="luametatex",
  template=template,
  checkers=checkers,
  reporter=report_format,
 },
 luajittex=sandbox.registerrunner {
  name="run luajittex format",
  program="luajittex",
  template=template,
  checkers=checkers,
  reporter=report_format,
 },
}
function environment.run_format(formatname,scriptname,filename,primaryflags,secondaryflags,verbose)
 local engine=environment.ownmain or "luatex"
 if not formatname or formatname=="" then
  report_format("missing format name")
  return
 end
 if not scriptname or scriptname=="" then
  report_format("missing script name")
  return
 end
 if not lfs.isfile(formatname) or not lfs.isfile(scriptname) then
  formatname,scriptname=resolvers.locateformat(formatname)
 end
 if not formatname or formatname=="" then
  report_format("invalid format name")
  return
 end
 if not scriptname or scriptname=="" then
  report_format("invalid script name")
  return
 end
 local runner=runners[engine]
 if not runner then
  report_format("format %a cannot be run, no runner available for engine %a",file.nameonly(name),engine)
  return
 end
 if not filename then
  filename ""
 end
 local binarypath=validbinarypath()
 local specification={
  binarypath=binarypath,
  primaryflags=primaryflags or "",
  secondaryflags=secondaryflags or "",
  fmtfile=quoted(formatname),
  luafile=quoted(scriptname),
  texfile=filename~="" and quoted(filename) or "",
 }
 statistics.starttiming()
 local result=runner(specification)
 local runtime=statistics.stoptiming()
 if verbose then
  report_format()
   if binarypath and binarypath~="" then
  report_format("binary path      : %s",binarypath)
   end
  report_format("luatex engine    : %s",engine)
  report_format("lua startup file : %s",scriptname)
  report_format("tex format file  : %s",formatname)
   if filename~="" then
  report_format("tex input file   : %s",filename)
   end
   if primaryflags~="" then
  report_format("primary flags    : %s",primaryflags)
   end
   if secondaryflags~="" then
  report_format("secondary flags  : %s",secondaryflags)
   end
  report_format("run time         : %.3f seconds",runtime)
  report_format("return value     : %s",result==0 and "okay" or "error")
  report_format()
 end
 return result
end


end -- of closure

-- used libraries    : l-bit32.lua l-lua.lua l-macro.lua l-sandbox.lua l-package.lua l-lpeg.lua l-function.lua l-string.lua l-table.lua l-io.lua l-number.lua l-set.lua l-os.lua l-file.lua l-gzip.lua l-md5.lua l-sha.lua l-url.lua l-dir.lua l-boolean.lua l-unicode.lua l-math.lua util-str.lua util-tab.lua util-fil.lua util-sac.lua util-sto.lua util-prs.lua util-fmt.lua util-soc-imp-reset.lua util-soc-imp-socket.lua util-soc-imp-copas.lua util-soc-imp-ltn12.lua util-soc-imp-mime.lua util-soc-imp-url.lua util-soc-imp-headers.lua util-soc-imp-tp.lua util-soc-imp-http.lua util-soc-imp-ftp.lua util-soc-imp-smtp.lua trac-set.lua trac-log.lua trac-inf.lua trac-pro.lua util-lua.lua util-deb.lua util-tpl.lua util-sbx.lua util-mrg.lua util-env.lua luat-env.lua util-zip.lua lxml-tab.lua lxml-lpt.lua lxml-mis.lua lxml-aux.lua lxml-xml.lua trac-xml.lua data-ini.lua data-exp.lua data-env.lua data-tmp.lua data-met.lua data-res.lua data-pre.lua data-inp.lua data-out.lua data-fil.lua data-con.lua data-use.lua data-zip.lua data-tre.lua data-sch.lua data-lua.lua data-aux.lua data-tmf.lua data-lst.lua libs-ini.lua luat-sta.lua luat-fmt.lua
-- skipped libraries : -
-- original bytes    : 1038373
-- stripped bytes    : 409980

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

    'l-bit32.lua',
    'l-lua.lua',
    'l-macro.lua',
    'l-sandbox.lua',
    'l-package.lua',
    'l-lpeg.lua',
    'l-function.lua',
    'l-string.lua',
    'l-table.lua',
    'l-io.lua',
    'l-number.lua',
    'l-set.lua',
    'l-os.lua',
    'l-file.lua',
    'l-gzip.lua',
    'l-md5.lua',
    'l-sha.lua',
    'l-url.lua',
    'l-dir.lua',
    'l-boolean.lua',
    'l-unicode.lua',
    'l-math.lua',

    'util-str.lua', -- code might move to l-string
    'util-tab.lua',
    'util-fil.lua',
    'util-sac.lua',
    'util-sto.lua',
    'util-prs.lua',
    'util-fmt.lua',

    'util-soc-imp-reset.lua',
    'util-soc-imp-socket.lua',
    'util-soc-imp-copas.lua',
    'util-soc-imp-ltn12.lua',
 -- 'util-soc-imp-mbox.lua',
    'util-soc-imp-mime.lua',
    'util-soc-imp-url.lua',
    'util-soc-imp-headers.lua',
    'util-soc-imp-tp.lua',
    'util-soc-imp-http.lua',
    'util-soc-imp-ftp.lua',
    'util-soc-imp-smtp.lua',

    'trac-set.lua',
    'trac-log.lua',
    'trac-inf.lua', -- was before trac-set
    'trac-pro.lua', -- not really needed
    'util-lua.lua', -- indeed here?
    'util-deb.lua',

    'util-tpl.lua',
    'util-sbx.lua',
    'util-mrg.lua',

    'util-env.lua',
    'luat-env.lua', -- can come before inf (as in mkiv)

    'util-zip.lua',

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

    'libs-ini.lua',

    'luat-sta.lua',
    'luat-fmt.lua',

}

-- c:/data/develop/tex-context/tex/texmf-win64/bin/../../texmf-context/tex/context/base/mkiv/data-tmf.lua
-- c:/data/develop/context/sources/data-tmf.lua

local ownlist = {
 -- '.',
 -- ownpath ,
    owntree .. "/../../../../context/sources", -- HH's development path
    --
    owntree .. "/../../texmf-local/tex/context/base/mkiv",
    owntree .. "/../../texmf-context/tex/context/base/mkiv",
    owntree .. "/../../texmf/tex/context/base/mkiv",
    owntree .. "/../../../texmf-local/tex/context/base/mkiv",
    owntree .. "/../../../texmf-context/tex/context/base/mkiv",
    owntree .. "/../../../texmf/tex/context/base/mkiv",
    --
    owntree .. "/../../texmf-local/tex/context/base",
    owntree .. "/../../texmf-context/tex/context/base",
    owntree .. "/../../texmf/tex/context/base",
    owntree .. "/../../../texmf-local/tex/context/base",
    owntree .. "/../../../texmf-context/tex/context/base",
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

----- e_verbose = environment.arguments["verbose"]

local e_verbose = false

-- some common flags (also passed through environment)

local e_silent       = environment.argument("silent")
local e_errors       = environment.argument("errors")
local e_noconsole    = environment.argument("noconsole")

local e_trackers     = environment.argument("trackers")
local e_directives   = environment.argument("directives")
local e_experiments  = environment.argument("experiments")

local t = { }

if type(e_directives) == "string" then
    t[#t+1] = e_directives
end

if type(e_silent) == "string" then
    t[#t+1] = format("logs.blocked={%s}",e_silent)
elseif e_silent == true then
    t[#t+1] = "logs.blocked"
end

if type(e_errors) == "string" then
    t[#t+1] = format("logs.errors={%s}",e_errors)
elseif e_errors == true then
    t[#t+1] = "logs.errors"
end

if e_noconsole then
    t[#t+1] = format("logs.target=file")
end

if #t > 0 then
    e_directives = concat(t,",")
end

if e_trackers    then trackers   .enable(e_trackers)    end
if e_directives  then directives .enable(e_directives)  end
if e_experiments then experiments.enable(e_experiments) end

if not environment.trackers    then environment.trackers    = e_trackers    end
if not environment.directives  then environment.directives  = e_directives  end
if not environment.experiments then environment.experiments = e_experiments end

--

resolvers.reset()

local helpinfo = [[
<?xml version="1.0" ?>
<application>
 <metadata>
  <entry name="name">mtxrun</entry>
  <entry name="detail">ConTeXt TDS Runner Tool</entry>
  <entry name="version">1.33</entry>
 </metadata>
 <flags>
  <category name="basic">
   <subcategory>
    <flag name="script"><short>run an mtx script (lua prefered method) (<ref name="noquotes"/>), no script gives list</short></flag>
    <flag name="evaluate"><short>run code passed on the commandline (between quotes) (=loop) (exit|quit aborts)</short></flag>
    <flag name="execute"><short>run a script or program (texmfstart method) (<ref name="noquotes"/>)</short></flag>
    <flag name="resolve"><short>resolve prefixed arguments</short></flag>
    <flag name="ctxlua"><short>run internally (using preloaded libs)</short></flag>
    <flag name="internal"><short>run script using built in libraries (same as <ref name="ctxlua"/>)</short></flag>
    <flag name="locate"><short>locate given filename in database (default) or system (<ref name="first"/> <ref name="all"/> <ref name="detail"/>)</short></flag>
   </subcategory>
   <subcategory>
    <flag name="tree" value="pathtotree"><short>use given texmf tree (default file: setuptex.tmf)</short></flag>
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
    <flag name="addbinarypath"><short>prepend the (found) binarypath to runners</short></flag>
   </subcategory>
   <subcategory>
    <flag name="verbose"><short>give a bit more info</short></flag>
    <flag name="trackers" value="list"><short>enable given trackers</short></flag>
    <flag name="progname" value="str"><short>format or backend</short></flag>
    <flag name="systeminfo" value="str"><short>show current operating system, processor, etc</short></flag>
   </subcategory>
   <subcategory>
    <flag name="edit"><short>launch editor with found file</short></flag>
    <flag name="launch"><short>launch files like manuals, assumes os support (<ref name="all"/>,<ref name="list"/>)</short></flag>
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
    <flag name="resolve-path"><short>expand variable (completely resolve paths)</short></flag>
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
    banner   = "ConTeXt TDS Runner Tool 1.32",
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
local texmfcnf =  resolvers.getenv("TEXMFCNF")
if not texmfcnf or texmfcnf == "" then
    texmfcnf = resolvers.expandedpathfromlist(resolvers.splitpath(resolvers.resolve(resolvers.luacnfspec)))
    resolvers.setenv("TEXMFCNF",table.concat(texmfcnf,";")) -- for running texexec etc (after tl change to texmf-dist)
end
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
                    local code = os.execute(command)
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
            local code = os.execute(command)
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
    local allresults = environment.arguments["all"]
    local pattern    = environment.arguments["pattern"]
    local listonly   = environment.arguments["list"]
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
            for i=1,#t do
                local name = t[i]
                if listonly then
                    report("% 3i:  %-30s %s",i,file.basename(name),file.dirname(name))
                else
                    report("launching: %s",name)
                    resolvers.launch(name)
                    if not allresults then
                        break
                    end
                end
            end
            if listonly then
                io.write("\n")
                io.write("\n[select number]\n\n>> ")
                local answer = tonumber(io.read())
                if answer then
                    io.write("\n")
                    local name = t[answer]
                    if name then
                        report("launching: %s",name)
                        resolvers.launch(name)
                    else
                        report("invalid number")
                    end
                end
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
        resolvers.renewcache()
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
                report("using script: %s (if --path is used don't run on path where mtxrun lives)\n",fullname)
            end
            environment.ownscript = fullname
            dofile(fullname)
            local savename = environment.arguments['save']
            if savename then
                if type(savename) ~= "string" then savename = file.basename(fullname) end
                savename = file.replacesuffix(savename,"cfg")
                runners.save_script_session(savename,save_list)
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
                        local application = match(data,"local application.-=.-(%{.-%})")
                        if application then
                            application = loadstring("return " .. application)
                            if application then
                                application = application()
                                local banner = application.banner
                                if banner then
                                    local description, version = match(banner,"^(.-) ([%d.]+)$")
                                    if description then
                                        valid[#valid+1] = { scriptbase, version, description }
                                    else
                                        valid[#valid+1] = { scriptbase, "", banner }
                                    end
                                end
                            end
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
    statistics.timed(action,true)
end

function runners.associate(filename)
    os.launch(filename)
end

function runners.evaluate(code,filename) -- for Luigi
    local environment = table.setmetatableindex(_G)
    if code == "loop" then
        while true do
            io.write("lua > ")
            local code = io.read()
            if code == "quit" or code == "exit"  then
                break
            elseif code ~= "" then
                local temp = string.match(code,"^= (.*)$")
                if temp then
                    code = "inspect("..temp..")"
                end
                local compiled, message = load(code,"console","t",environment)
                if type(compiled) ~= "function" then
                    compiled = load("inspect("..code..")","console","t",environment)
                end
                if type(compiled) ~= "function" then
                    io.write("! " .. (message or code).."\n")
                else
                    io.write(compiled())
                end
            end
        end
    else
        if type(code) ~= "string" or code == "" then
            code = filename
        end
        if code ~= "" then
            local compiled, message = load(code,"console","t",environment)
            if type(compiled) ~= "function" then
                compiled = load("inspect("..code..")","console","t",environment)
            end
            if type(compiled) ~= "function" then
                io.write("invalid lua code: " .. (message or code))
                return
            end
            io.write(compiled())
        end
    end
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

function runners.systeminfo()
    report("architecture      : %s",os.platform  or "<unset>")
    report("operating system  : %s",os.name      or "<unset>")
    report("file architecture : %s",os.type      or "<unset>")
    report("binary path       : %s",os.selfdir   or "<unset>")
    report("binary suffix     : %s",os.binsuffix or "<unset>")
    report("library suffix    : %s",os.libsuffix or "<unset>")
end

-- this is a bit dirty ... first we store the first filename and next we
-- split the arguments so that we only see the ones meant for this script
-- ... later we will use the second half

local filename = environment.files[1] or ""
local ok      = true

local before, after = environment.splitarguments(filename)
environment.arguments_before, environment.arguments_after = before, after
environment.initializearguments(before)

e_verbose = environment.arguments["verbose"] -- delayed till here (we need the ones before script)

if e_verbose then
    trackers.enable("resolvers.locating")
end

-- maybe the unset has to go to this level

local is_mkii_stub = runners.registered[file.removesuffix(file.basename(filename))]

local e_argument = environment.argument

if e_argument("timedlog") then
    logs.settimedlog()
end

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
            resolvers.renewcache()
            trackers.enable("resolvers.locating")
            if not resolvers.load(...) then
                report("the resolver databases are not present or outdated")
            end
        end
    end

    resolvers.load_tree(e_argument('tree'),e_argument("resolve"))

end

-- joke .. reminds me of messing with gigi terminals

do

    local a_locale = e_argument("locale")

    if a_locale then

        -- I really hate this crap but am too tired of discussing it over and over
        -- again so for the sake of usiage outside context we will provide ways to
        -- use locales in an otherwise supposed to be locale agnostic system. And
        -- forget about support in case of interferences.

        report()
        report(what == "force" and "forcing locale:" or "original locale:")
        report()
        report("  collate  : %s",status.lc_collate  or "<unset>")
        report("  ctype    : %s",status.lc_ctype    or "<unset>")
        report("  monetary : %s",status.lc_monetary or "<unset>")
        report("  numeric  : %s",status.lc_numeric  or "<unset>")
        report("  time     : %s",status.lc_time     or "<unset>")
        report()

    end

    if a_locale == "force" then
        os.setlocale(status.lc_collate ,"collate")
        os.setlocale(status.lc_ctype   ,"ctype")
        os.setlocale(status.lc_monetary,"monetary")
        os.setlocale(status.lc_numeric ,"numeric")
        os.setlocale(status.lc_time    ,"time")
    else
        function os.setlocale()
        end
    end

end

-- if e_argument("ansi") or e_argument("ansilog") then

--     logs.setformatters(e_argument("ansi") and "ansi" or "ansilog")

--  -- local script = e_argument("script") or e_argument("scripts")
--  --
--  -- if type(script) == "string" then
--  --     logs.writer("]0;"..script.."") -- for Alan to test
--  -- end

-- end

if e_argument("script") or e_argument("scripts") then

    -- run a script by loading it (using libs), pass args

    if e_argument("nofiledatabase") then
        -- handy for mtx-update
    else
        runners.loadbase()
    end
    if is_mkii_stub then
        ok = runners.execute_script(filename,false,true)
    else
        ok = runners.execute_ctx_script(filename)
    end

elseif e_argument("evaluate") then

    runners.evaluate(e_argument("evaluate"),filename)

elseif e_argument("selfmerge") then

    -- embed used libraries

    runners.loadbase()
    local found = locate_libs()

    if found then
        local mtxrun = resolvers.findfile("mtxrun.lua") -- includes local name
        if lfs.isfile(mtxrun) then
            utilities.merger.selfmerge(mtxrun,own.libs,{ found })
            application.report("runner updated on resolved path: %s",mtxrun)
        else
            utilities.merger.selfmerge(own.name,own.libs,{ found })
            application.report("runner updated on relative path: %s",own.name)
        end
    end

elseif e_argument("selfclean") then

    -- remove embedded libraries

    runners.loadbase()

    local mtxrun = resolvers.findfile("mtxrun.lua") -- includes local name
    if lfs.isfile(mtxrun) then
        utilities.merger.selfclean(mtxrun)
        application.report("runner cleaned on resolved path: %s",mtxrun)
    else
        utilities.merger.selfclean(own.name)
        application.report("runner cleaned on relative path: %s",own.name)
    end

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
    local path = resolvers.findpath(filename)
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

elseif e_argument("resolve-path") then

    resolvers.load("nofiles")
    runners.register_arguments(filename)
    environment.initializearguments(environment.arguments_after)
    resolvers.dowithfilesandreport(resolvers.cleanedpathlist, environment.files)

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

-- elseif e_argument("pattern") then
--
--     -- luatools
--
--     runners.execute_ctx_script("mtx-base","--pattern='" .. e_argument("pattern") .. "'",filename)

elseif e_argument("generate") then

    -- luatools

    if filename and filename ~= "" then
        resolvers.load("nofiles")
        trackers.enable("resolvers.locating")
        resolvers.renew(filename)
    else
        resolvers.renewcache()
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

    application.report("source path",environment.ownbin)

elseif e_argument("directives") then

    directives.show()

elseif e_argument("trackers") then

    trackers.show()

elseif e_argument("experiments") then

    experiments.show()

elseif e_argument("exporthelp") then

    runners.loadbase()
    application.export(e_argument("exporthelp"),filename)

elseif e_argument("systeminfo") then

    runners.systeminfo()

elseif e_argument("locale") then

    -- already done

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
    report("elapsed lua time: %0.3f seconds",os.runtime())
end

if os.type ~= "windows" then
    texio.write("\n") -- is this still valid?
end

if ok == false then ok = 1 elseif ok == true or ok == nil then ok = 0 end

-- os.exit(ok,true) -- true forces a cleanup in 5.2+

os.exit(ok)         -- true forces a cleanup in 5.2+ but reports a wrong number then
