--[[
   xxd -i -n xdvipsk_tounicode xdvipsk_tounicode.lua >xdvipsk_tounicode.h
--]]

---------------
glyphtounicode_add = {}

function uni32_to_utf16(val)
   --[[
   converts an unicode value to an UTF-16 array table (one or two elements)
   --]]
   local utf16_arr = nil
   if val <= 0x10FFFF then
      if val >= 0x10000 then
         val = val - 0x10000
         utf16_arr = {0xD800 | ((val >> 10) & 0x3FF), 0xDC00 | (val & 0x3FF)}
      else
         utf16_arr = {val}
      end
   end
   return utf16_arr
end

function utf16_to_uni32(utf16_arr)
   --[[
   converts an UTF-16 array table to a 32 bit unicode values array table
   --]]
   local uni32_arr = {}
   local uni32_val = 0
   for _, val in ipairs(utf16_arr) do
      if (val & 0xFC00) == 0xD800 then
         uni32_val = (val & 0x3FF) << 10
      elseif (val & 0xFC00) == 0xDC00 then
         uni32_arr[#uni32_arr + 1] = (uni32_val | (val & 0x3FF)) + 0x10000
      else
         uni32_arr[#uni32_arr + 1] = val
      end
   end
   return uni32_arr
end

function load_touni(gltab, overwrite, map_name)
   for key, val in pairs(gltab) do
      local key_to_add = key
      if map_name and (map_name ~= '') and (not string.find(key_to_add, ':', 1, true)) and (not string.find(key_to_add, '/', 1, true)) then
          key_to_add = 'pfb:' .. map_name .. '/' .. key_to_add
      end
      if (not glyphtounicode_add[key_to_add]) or overwrite then
         local res_arr = {}
         for _, ucode in ipairs(val) do
            if type(ucode) == 'string' then
               res_arr[#res_arr + 1] = ucode
            else
               local utf16_arr = uni32_to_utf16(ucode)
               for _, uval in ipairs(utf16_arr) do
                  res_arr[#res_arr + 1] = uval
               end
            end
         end
         glyphtounicode_add[key_to_add] = res_arr
      end
   end
end

function is_private(uni_rec)
   local ret_val = true
   if uni_rec then
      local uni_type = type(uni_rec)
      if uni_type == 'number' then
         ret_val = -- Private Use Areas
             (((uni_rec >= 0xE000) and (uni_rec < 0xF900)) or
             ((uni_rec >= 0xF0000) and (uni_rec < 0xFFFFE)) or
             ((uni_rec >= 0x100000) and (uni_rec < 0x10FFFE)))
      elseif uni_type == 'table' then
         local uni_copy = {}
         for _, val in ipairs(uni_rec) do
            if type(val) == 'number' then
               uni_copy[#uni_copy + 1] = val
            end
         end
         local uni32_arr = utf16_to_uni32(uni_copy)
         for _, val in ipairs(uni32_arr) do
            ret_val = is_private(val)
            if ret_val then
               break
            end
         end
      end
   end
   return ret_val
end

function get_glname_touni_rec_single(glname, font_pref, glyphtounicode_table)
   --[[
   returns:
      nil if not found
      second return value -- false in case of 'uniXXXX' like glyph name -- they will be parsed by GS or Acrobat Distiller, no need to add them to the /GlyphNames2Unicode
   --]]
   local uni_rec = glyphtounicode_table[glname]
   local needs_g2u = true
   if (not uni_rec) and (glyphtounicode_table == glyphtounicode_add) then
      uni_rec = glyphtounicode_main[glname]
   end
   if (not uni_rec) or is_private(uni_rec) then
      local gl_pos = string.find(glname, '/', 1, true)
      if (gl_pos) then -- 'pfb:msbm10/A'
         font_pref = string.sub(glname, 1, gl_pos)
         glname = string.sub(glname, gl_pos + 1)
         uni_rec, needs_g2u = get_glname_touni_rec_single(glname, font_pref, glyphtounicode_table)
         if (not uni_rec) and (glyphtounicode_table == glyphtounicode_add) then
            uni_rec, needs_g2u = get_glname_touni_rec_single(glname, font_pref, glyphtounicode_main)
         end
      end
   end
   if (not uni_rec) or is_private(uni_rec) then
      local gl_pos = string.find(glname, '.', 1, true)
      if (gl_pos) then -- 'one.superior'
         glname = string.sub(glname, 1, gl_pos - 1)
         local glname_alias = font_pref .. glname
         uni_rec, needs_g2u = get_glname_touni_rec_single(glname_alias, font_pref, glyphtounicode_table)
         if (not uni_rec) and (glyphtounicode_table == glyphtounicode_add) then
            uni_rec, needs_g2u = get_glname_touni_rec_single(glname_alias, font_pref, glyphtounicode_main)
         end
      end
   end
   if (not uni_rec) or is_private(uni_rec) then -- 'f_f_i'
      local gl_pos = string.find(glname, '_', 1, true)
      if gl_pos then
         local uni_rec_comb = {}
         while true do
            local uni_rec_alias
            if gl_pos then
               local glname_part = string.sub(glname, 1, gl_pos - 1)
               glname = string.sub(glname, gl_pos + 1)
               local glname_alias = font_pref .. glname_part
               uni_rec_alias, needs_g2u = get_glname_touni_rec_single(glname_alias, font_pref, glyphtounicode_table)
               if (not uni_rec_alias) and (glyphtounicode_table == glyphtounicode_add) then
                  uni_rec_alias, needs_g2u = get_glname_touni_rec_single(glname_alias, font_pref, glyphtounicode_main)
               end
            else
               local glname_alias = font_pref .. glname
               uni_rec_alias, needs_g2u = get_glname_touni_rec_single(glname_alias, font_pref, glyphtounicode_table)
               if (not uni_rec_alias) and (glyphtounicode_table == glyphtounicode_add) then
                  uni_rec_alias, needs_g2u = get_glname_touni_rec_single(glname_alias, font_pref, glyphtounicode_main)
               end
            end
            if uni_rec_alias then
               local uni_type = type(uni_rec_alias)
               if uni_type == 'number' then
                  uni_rec_comb[#uni_rec_comb + 1] = uni_rec_alias
               elseif uni_type == 'table' then
                  for _, val in ipairs(uni_rec_alias) do
                     uni_rec_comb[#uni_rec_comb + 1] = val
                  end
               end
            else
               uni_rec_comb = {}
               break
            end
            if (not gl_pos) then
               break
            end
            gl_pos = string.find(glname, '_', 1, true)
         end
         if #uni_rec_comb > 0 then
            uni_rec = uni_rec_comb
         end
      end
   end
   if (not uni_rec) or is_private(uni_rec) then
      if ((glname == 'fi') or (glname == 'fl') or (glname == 'ff') or (glname == 'ffi') or (glname == 'ffl')) then
         local uni_rec_comb = {}
         for ii = 1, #glname do
            local glname_alias = font_pref .. string.sub(glname, ii, ii)
            local uni_rec_alias, needs_g2u = get_glname_touni_rec_single(glname_alias, font_pref, glyphtounicode_table)
            if (not uni_rec_alias) and (glyphtounicode_table == glyphtounicode_add) then
               uni_rec_alias, needs_g2u = get_glname_touni_rec_single(glname_alias, font_pref, glyphtounicode_main)
            end
            if uni_rec_alias then
               local uni_type = type(uni_rec_alias)
               if uni_type == 'number' then
                  uni_rec_comb[#uni_rec_comb + 1] = uni_rec_alias
               elseif uni_type == 'table' then
                  for _, val in ipairs(uni_rec_alias) do
                     uni_rec_comb[#uni_rec_comb + 1] = val
                  end
               end
            else
               uni_rec_comb = {}
               break
            end
         end
         if #uni_rec_comb > 0 then
            uni_rec = uni_rec_comb
         end
      end
   end
   if not uni_rec then
      if (string.find(glname, 'uni', 1, true) == 1) then
         needs_g2u = false
         uni_rec = {}
         local hex_str = string.sub(glname, 4)
         while hex_str ~= '' do
            local val = tonumber(string.sub(hex_str, 1, 4), 16)
            if val ~= nil then
               uni_rec[#uni_rec + 1] = val
            else
               needs_g2u = true
               uni_rec = nil
               break
            end
            hex_str = string.sub(hex_str, 5)
         end
      end
   end
   if not uni_rec then
      if (string.sub(glname, 1, 1) == 'u') then
         needs_g2u = false
         local hex_str = string.sub(glname, 2)
         local val = tonumber(hex_str, 16)
         if val ~= nil then
            uni_rec = uni32_to_utf16(val)
         else
            needs_g2u = true
         end
      end
   end
   return uni_rec, needs_g2u
end

function get_glname_tounicode(glname, exact)
   --[[
   returns:
      numeric part of the unicode record
      empty table if not found
      second return value -- false in case of 'uniXXXX' like glyph name
   --]]
   local uni_rec
   local ret_arr = {}
   local needs_g2u = true
   if exact then
      uni_rec = glyphtounicode_add[glname]
      if (not uni_rec) then
         uni_rec = glyphtounicode_main[glname]
      end
   else
      uni_rec, needs_g2u = get_glname_touni_rec_single(glname, '', glyphtounicode_add)
   end
   if uni_rec then
      local uni_type = type(uni_rec)
      if uni_type == 'number' then
         ret_arr = {uni_rec}
      elseif uni_type == 'table' then
         for _, val in ipairs(uni_rec) do
            if type(val) == 'number' then
               ret_arr[#ret_arr + 1] = val
            end
         end
      end
   end
   return ret_arr, needs_g2u
end

function get_glname_subst_single(glname, exact, glyphtounicode_table)
   --[[
   returns:
      alias name of the glyph and numeric part of the unicode record
      empty string and empty table if not found
   --]]
   local uni_rec
   local ret_str = ''
   local ret_arr = {}
   uni_rec = glyphtounicode_table[glname]
   if uni_rec then
      local uni_type = type(uni_rec)
      if uni_type == 'string' then
         ret_str = uni_rec
      elseif type(uni_rec) == 'table' then
         for _, val in ipairs(uni_rec) do
            if type(val) == 'number' then
               ret_arr[#ret_arr + 1] = val
            elseif type(val) == 'string' then
               ret_str = val
            end
         end
      end
   end
   if ((#ret_arr == 0) and (not exact)) then
      local gl_pos = string.find(glname, '/', 1, true)
      if (gl_pos) then -- 'pfb:msbm10/A'
         glname = string.sub(glname, gl_pos + 1)
         ret_str, ret_arr = get_glname_subst_single(glname, true, glyphtounicode_table)
      end
   end
   return ret_str, ret_arr
end

function get_glname_subst(glname, exact)
   --[[
   returns:
      alias name of the glyph
      empty string if not found
      generates uXXXXX name for overriden glyphs with no name alias and different unicode values
      second return value -- false in case of 'uniXXXX' like glyph name
   --]]
   local ret_str_main = ''
   local ret_arr_main = {}
   local ret_str_add = ''
   local ret_arr_add = {}
   local needs_g2u = true
   ret_str_add, ret_arr_add = get_glname_subst_single(glname, exact, glyphtounicode_add)
   ret_str_main, ret_arr_main = get_glname_subst_single(glname, false, glyphtounicode_main)
   if ret_str_add == '' then
      ret_str_add = ret_str_main
   end
   if (ret_str_add == '') and (#ret_arr_main > 0) and (#ret_arr_add > 0) then
      local equal = true
      if #ret_arr_add ~= #ret_arr_main then
         equal = false
      else
         for ii = 1, #ret_arr_add do
            if ret_arr_add[ii] ~= ret_arr_main[ii] then
               equal = false
               break
            end
         end
      end
      if not equal then
         ret_arr_add_32 = utf16_to_uni32(ret_arr_add)
         if #ret_arr_add_32 == 1 then
            ret_str_add = string.format('u%04X', ret_arr_add_32[1])
         else
            ret_str_add = 'uni'
            for ii = 1, #ret_arr_add do
               ret_str_add = ret_str_add .. string.format('%04X', ret_arr_add[ii])
            end
         end
         needs_g2u = false
      end
   end
   return ret_str_add, needs_g2u
end

---------------
tfm_2_pfb = {}

function add_tfm_2_pfb(tfm_name, pfb_name)
   tfm_2_pfb[tfm_name] = pfb_name
end

function encode_tfm_2_pfb_single(glyphtounicode_table)
   local new_entries = {}
   for key, val in pairs(glyphtounicode_table) do
      local gl_pos = string.find(key, '/', 1, true)
      if gl_pos and (string.find(key, 'tfm:', 1, true) == 1) then
         local glname = string.sub(key, gl_pos + 1)
         local pref = string.sub(key, 5, gl_pos - 1)
         local pfb_name = tfm_2_pfb[pref]
         if pfb_name then
            new_entries['pfb:' .. pfb_name .. '/' .. glname] = val
         end
      end
   end
   for key, val in pairs(new_entries) do
      glyphtounicode_table[key] = val
   end
end

function encode_tfm_2_pfb()
   encode_tfm_2_pfb_single(glyphtounicode_main)
   encode_tfm_2_pfb_single(glyphtounicode_add)
end

---------------
g2u_loaded = {}

function is_g2u_loaded(pfb_name)
   local ret_val = (g2u_loaded[pfb_name] ~= nil)
   g2u_loaded[pfb_name] = true
   return ret_val
end
--file end null char 
