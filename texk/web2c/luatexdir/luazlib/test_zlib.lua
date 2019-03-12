-- $Id: test_zlib.lua,v 1.3 2004/07/22 19:10:47 tngd Exp $
-- zlib = loadlib("./lzlib.so", "luaopen_zlib")()


local function line(header, c)
    header = header or ''
    c = c or '-'
    print(string.rep(string.sub(c, 1, 1), 78 - string.len(header))..header)
end

local function ipart(value)
    return value - math.mod(value, 1)
end

local function bitvalues(value, bstart, num)
    value = ipart(value / 2^bstart)
    return math.mod(value, 2^num)
end

line(' zlib '..zlib.version(), '=')

line(' adler32')
local adler = zlib.adler32()
print('adler32 init : '..adler)
adler = zlib.adler32(adler, 'some text')
print('updated adler: '..adler)
adler = zlib.adler32(adler, 'some text')
print('updated adler: '..adler)
adler = zlib.adler32(adler, 'some text')
print('updated adler: '..adler)
adler = zlib.adler32(adler, 'some text')
print('updated adler: '..adler)
adler = zlib.adler32(adler, 'some text')
print('updated adler: '..adler)
adler = zlib.adler32(adler, 'some textd')
print('updated adler: '..adler)

line(' crc32')
local crc = zlib.crc32()
print('crc32 init : '..crc)
crc = zlib.crc32(crc, 'some text')
print('updated crc: '..crc)
crc = zlib.crc32(crc, 'some text')
print('updated crc: '..crc)
crc = zlib.crc32(crc, 'some text')
print('updated crc: '..crc)
crc = zlib.crc32(crc, 'some text')
print('updated crc: '..crc)
crc = zlib.crc32(crc, 'some text')
print('updated crc: '..crc)
crc = zlib.crc32(crc, 'some textd')
print('updated crc: '..crc)


line(' deflate/inflate')
local us
f = io.open('test_zlib.lua') -- f = io.open('../all.tar')
us = f:read('*a')
f:close()

do -- local block

local f, cs, zd, zi, aux_res, res, ret, count

print('file length              : '..string.len(us))

cs = ''
zd = zlib.compressobj(1)
print('deflate stream           : '..tostring(zd))

cs = cs .. zd:compress(string.sub(us, 1, string.len(us)/2))
cs = cs .. zd:compress(string.sub(us, string.len(us)/2+1))
cs = cs .. zd:flush()

print('compressed length        : '..string.len(cs))
print('compressed adler         : '..tostring(zd:adler()))
zd:close()

zi = zlib.decompressobj()
print('inflate stream           : '..tostring(zi))
res = ''
res = res .. zi:decompress(string.sub(cs, 1, 10))
res = res .. zi:decompress(string.sub(cs, 11))
res = res .. zi:flush()
print('uncompressed length      : '..string.len(res))
print('uncompressed adler       : '..tostring(zi:adler()))
zi:close()
print('result == uncompressed   : '..tostring(res == us))
print('compression ratio        : '..tostring(string.len(us)/string.len(cs)))

end -- local block
collectgarbage()

line(' compress/uncompress')
do -- local block
local cs, res
print('file length              : '..string.len(us))
cs = zlib.compress(us,1)
print('compressed length        : '..string.len(cs))
res = zlib.decompress(cs)
print('compressed length        : '..string.len(res))
print('result == uncompressed   : '..tostring(res == us))
print('compression ratio        : '..tostring(string.len(us)/string.len(cs)))
end -- local block

line(' zlib '..zlib.version(), '=')
