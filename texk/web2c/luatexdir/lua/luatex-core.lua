-- if not modules then modules = { } end modules ['luatex-core'] = {
--     version   = 1.001,
--     comment   = 'companion to luatex',
--     author    = 'Hans Hagen & Luigi Scarso',
--     copyright = 'LuaTeX Development Team',
-- }

LUATEXCOREVERSION = 1.002

-- This file overloads some Lua functions. The readline variants provide the same
-- functionality as LuaTeX <= 1.04 and doing it this way permits us to keep the
-- original io libraries clean. Performance is probably even a bit better now.

local type, next, getmetatable, require = type, next, getmetatable, require
local find, gsub = string.find, string.gsub

local io_open             = io.open
local io_popen            = io.popen
local io_line             = io.lines

local fio_readline        = fio.readline
local fio_checkpermission = fio.checkpermission
local fio_recordfilename  = fio.recordfilename

local mt                  = getmetatable(io.stderr)
local mt_lines            = mt.lines
local saferoption         = status.safer_option
local shellescape         = status.shell_escape     -- 0 (disabled) 1 (restricted) 2 (everything)
local kpseused            = status.kpse_used        -- 0 1

io.saved_open             = io_open  -- can be protected
io.saved_popen            = io_popen -- can be protected
io.saved_lines            = io_lines -- always readonly
mt.saved_lines            = mt_lines -- always readonly

local function luatex_io_open(name,how)
    if not how then
        how = 'r'
    end
    local f = io_open(name,how)
    if f then
        if type(how) == 'string' and find(how,'w') then
            fio_recordfilename(name,'w')
        else
            fio_recordfilename(name,'r')
        end
    end
    return f
end

local function luatex_io_open_readonly(name,how)
    if how then
        how = 'r'
    else
        how = gsub(how,'[^rb]','')
        if how == '' then
            how = 'r'
        end
    end
    local f = io_open(name,how)
    if f then
        fio_recordfilename(name,'r')
    end
    return f
end

local function luatex_io_popen(name,...)
    local okay, found = fio_checkpermission(name)
    if okay and found then
        return io_popen(found,...)
    end
end

local function luatex_io_lines(name)
    local f = io_open(name,'r')
    if f then
        return function()
            return fio_readline(f)
        end
    end
end

local function luatex_io_readline(f)
    return function()
        return fio_readline(f)
    end
end

io.lines = luatex_io_lines
mt.lines = luatex_io_readline

-- We assume management to be provided by the replacement of kpse. This is the
-- case in ConTeXt.

if kpseused == 1 then

    io.open  = luatex_io_open
    io.popen = luatex_io_popen

end

if saferoption == 1 then

    os.execute = nil
    os.spawn   = nil
    os.exec    = nil
    os.setenv  = nil
    os.tempdir = nil

    io.popen   = nil
    io.open    = nil

    os.rename  = nil
    os.remove  = nil

    io.tmpfile = nil
    io.output  = nil

    lfs.chdir  = nil
    lfs.lock   = nil
    lfs.touch  = nil
    lfs.rmdir  = nil
    lfs.mkdir  = nil

    io.saved_popen = nil
    io.saved_open  = luatex_io_open_readonly

end

if saferoption == 1 or shellescape ~= 2 then

    ffi = require('ffi')
    for k, v in next, ffi do
        if k ~= 'gc' then
            ffi[k] = nil
        end
    end
    ffi = nil
end

-- os.[execute|os.spawn|os.exec] already are shellescape aware)


if md5 then

    local sum    = md5.sum
    local gsub   = string.gsub
    local format = string.format
    local byte   = string.byte

    function md5.sumhexa(k)
        return (gsub(sum(k), ".", function(c)
            return format("%02x",byte(c))
        end))
    end

    function md5.sumHEXA(k)
        return (gsub(sum(k), ".", function(c)
            return format("%02X",byte(c))
        end))
    end

end

if utilities and utilities.merger and utilities.merger.compact then

    local byte, format, gmatch = string.byte, string.format, string.gmatch
    local concat = table.concat

    local data = gsub(io.loaddata('luatex-core.lua'),'if%s+utilities.*','')
    local t = { }
    local r = { }
    local n = 0
    local d = gsub(data,'\r\n','\n')      -- be nice for unix
    local s = utilities.merger.compact(d) -- no comments and less spaces

    t[#t+1] = '/* generated from and by luatex-core.lua */'
    t[#t+1] = ''
 -- t[#t+1] = format('/*\n\n%s\n\n*/',d)
 -- t[#t+1] = ''
    t[#t+1] = '#include "lua.h"'
    t[#t+1] = '#include "lauxlib.h"'
    t[#t+1] = ''
    t[#t+1] = 'int load_luatex_core_lua (lua_State * L);'
    t[#t+1] = ''
    t[#t+1] = 'int load_luatex_core_lua (lua_State * L)'
    t[#t+1] = '{'
    t[#t+1] = '  static unsigned char luatex_core_lua[] = {'
    for c in gmatch(d,'.') do
        if n == 16 then
            n = 1
            t[#t+1] = '    ' .. concat(r,', ') .. ','
        else
            n = n + 1
        end
        r[n] = format('0x%02x',byte(c))
    end
    n = n + 1
    r[n] = '0x00'
    t[#t+1] = '    ' .. concat(r,', ',1,n)
    t[#t+1] = '  };'
 -- t[#t+1] = format('unsigned int luatex_core_lua_len = 0x%x;',#d+1)
    t[#t+1] = '  return luaL_dostring(L, (const char*) luatex_core_lua);'
    t[#t+1] = '}'

    io.savedata('luatex-core.c',concat(t,'\n'))
    io.savedata('luatex-core-stripped.lua',s)

end
