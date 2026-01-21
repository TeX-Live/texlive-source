--
--  This is file `xdvipsk.lua'
--
--  Developed by: Sigitas Tolusis
--
--  Demo functions for lua callbacks in xdvipsk (TeXLive 2024)
--

prescan_specials_callback = function(...)
   local special, tbl = ...
   print("PRESCAN SPECIAL:", special)
   print("INFO:", tbl['hh'], tbl['vv'], tbl['pagenum'])
   return true
end

after_prescan_callback = function(...)
   xdvipsk_params = ...
   for k, v in pairs(xdvipsk_params) do
      print(k,v)
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
end

scan_specials_callback = function(...)
   local special, tbl = ...
   print("SPECIAL:", special)
   print("INFO:", tbl['hh'], tbl['vv'], tbl['pagenum'])
end

after_drawchar_callback = function(...)
   char_info = ...
   if char_info.cid > 0 then
      for k, v in pairs(char_info) do
         if k == "tounicode" then
            for kk, vv in pairs(v) do
               print(k, kk, vv)
            end
         else
            print(k,v)
         end
      end
   end
end

process_stack_callback = function(...)
   for k, v in pairs(...) do
      print(k, v)
   end
end

after_drawrule_callback = function(...)
   for k, v in pairs(...) do
      print(k, v)
   end
end

--
-- End of file `xdvipsk.lua'.