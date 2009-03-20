-- md5.lua
-- (you should put this file in any directory of your LUA_PATH)


-- change the next line to point to the md5 dynamic library

-- local path = "/usr/local/lua/lib/libmd5.so"
--local path = "./libmd5.so"

--assert(loadlib(path, "luaopen_md5"))()


-- Returns the md5 hash value as a string of hexadecimal digits

function md5.sumhexa (k)
  k = md5.sum(k)
  return (string.gsub(k, ".", function (c)
           return string.format("%02x", string.byte(c))
         end))
end

return md5
