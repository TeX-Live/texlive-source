--
--  This is file `xdvipsk.lua'
--
--  Developed by: Sigitas Tolusis
--
--  Demo functions for lua callbacks in xdvipsk (TeXLive 2026)
--

local xdvipsk_params = {}

prescan_specials_callback = function(...)
   local special, tbl = ...
   print("PRESCAN SPECIAL: " .. special)
   print("INFO:", tbl['hh'], tbl['vv'], tbl['pagenum'])
   return true
end

after_prescan_callback = function(...)
   xdvipsk_params = ...
   for k, v in pairs(xdvipsk_params) do
      print(k, v)
   end
   print("PAGESIZE:", xdvipsk_params['hpapersize']/65781.76, xdvipsk_params['vpapersize']/65781.76)
   get_x = function(x)
      return ((x * 72)/xdvipsk_params['actualdpi'] + 72.0)
   end
   get_y = function(y)
      return ((y * 72)/xdvipsk_params['vactualdpi'] + 72.0)
   end
   get_pdf_y = function(y)
      return (xdvipsk_params['vpapersize']/65781.76 - y)
   end
   xdvipsk_params.fd = io.open(xdvipsk_params.dvifile .. ".info", 'w')
end

scan_specials_callback = function(...)
   local special, tbl = ...
   xdvipsk_params.fd:write("SPECIAL: " .. special .. "\n")
   local item = table.concat({"INFO:", tbl['hh'], tbl['vv'], tbl['pagenum'], "\n"}, ", ")
   xdvipsk_params.fd:write(item)
end

after_drawchar_callback = function(...)
   char_info = ...
   if char_info.cid > 0 then
      for k, v in pairs(char_info) do
         if k == "tounicode" then
            for kk, vv in pairs(v) do
               xdvipsk_params.fd:write(k, " ", kk, " ", vv, "\n")
            end
         else
            xdvipsk_params.fd:write(k, " ", v, "\n")
         end
      end
   end
end

process_stack_callback = function(...)
   for k, v in pairs(...) do
       xdvipsk_params.fd:write(k, " ", v, "\n")
   end
end

after_drawrule_callback = function(...)
   for k, v in pairs(...) do
      xdvipsk_params.fd:write(k, " ", v, "\n")
   end
end

dvips_exit_callback = function(...)
   xdvipsk_params.fd:close()
end
--
-- End of file `xdvipsk.lua'.
