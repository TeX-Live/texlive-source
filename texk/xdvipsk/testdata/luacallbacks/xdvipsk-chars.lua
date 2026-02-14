--
--  This is file `xdvipsk-chars.lua'
--
--  Output chars info suitable to compare
--
--  v1.0.1, 2025-03-05 (ST)
--

local xdvipsk_params = nil
local fonts = {}
local not_unicode = {}

after_prescan_callback = function(...)
   --[[ available keys:
      dvifile totalpages hpapersize vpapersize hoff voff
      actualdpi vactualdpi mag num den
   --]]
   xdvipsk_params = ...
   get_x = function(x)
      return ((x * 72)/xdvipsk_params['actualdpi'] + 72.0)
   end
   get_y = function(y)
      return ((y * 72)/xdvipsk_params['vactualdpi'] + 72.0)
   end
   get_pdf_y = function(y)
      return (xdvipsk_params['vpapersize']/65781.76 - get_y(y))
   end
   math_ceil = function(val)
      return math.ceil(val * 100 + 0.5)/100
   end
   --[[
   print("PAGESIZE:", xdvipsk_params['hpapersize']/65781.76, xdvipsk_params['vpapersize']/65781.76,
         'vdpi:', xdvipsk_params['vactualdpi'], 'dpi:', xdvipsk_params['actualdpi'],
         'total:', xdvipsk_params['totalpages'])
   --]]
   xdvipsk_params.fd = io.open(xdvipsk_params.dvifile .. ".chars", 'w')
end

after_drawchar_callback = function(...)
   --[[ available keys:
         lastfont tfmname psname fntfile
         charcode cid glyphname
         dir pixelwidth
         pagenum rvv rhh bpos epos
   --]]
   local char_info = ...
   local tounicode = char_info.tounicode
   local lastfont = char_info.lastfont
   if not fonts[lastfont] then
      fonts[lastfont] = {lastfont, char_info.tfmname, char_info.psname, char_info.fntfile}
   end
   local hex = ''
   if type(tounicode) ~= 'table' then
      local idx = ''..char_info.charcode..'/'..char_info.lastfont
      if not not_unicode[idx] then 
         not_unicode[idx] = {'gl:'..char_info.glyphname, 'c:'..char_info.charcode, 'tfm:'..char_info.tfmname,
                             'psname:'..char_info.psname, 'fnt:'..char_info.fntfile, 'pg:'..char_info.pagenum}
      end
   else
      for k, v in ipairs(tounicode) do
         hex = hex .. string.format("%x", v)
      end
   end
   local item = table.concat({char_info.pagenum, char_info.charcode, (char_info.glyphname or '???'), hex, char_info.tfmname, get_x(char_info.rhh), get_y(char_info.rvv)}, ", ")
   xdvipsk_params.fd:write(item .. "\n")
end

dvips_exit_callback = function(...)
   xdvipsk_params.fd:close()
--[[
   print("FONTS:")
   for k, v in ipairs(fonts) do
      print(table.unpack(v))
   end
   print("Not Unicode CHARS:")
   for k, v in pairs(not_unicode) do
      print(table.unpack(v))
   end
--]]
end

after_drawrule_callback = function(...)
   --[[ available keys:
         vv hh rw rh dir
         pagenum bpos epos
   --]]
   --[[
   print("DRAWRULE:")
   for k, v in pairs(...) do
      print(k, v)
   end
   --]]
end

--
-- End of file `xdvipsk-chars.lua'.
