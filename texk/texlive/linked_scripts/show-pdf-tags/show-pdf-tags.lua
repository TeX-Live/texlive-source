#!/usr/bin/env texlua

local show_pdf_tags_version = "1.5"

kpse.set_program_name'lualatex'

local mypath = string.match(debug.getinfo(1, 'S').source, '@(.*)[/\\][^/\\]+')
if mypath then
  package.path = mypath .. '/?.lua;' .. package.path
end

local out_format = "tree"
local follow_rolemap = false
local hide_w3c = false
local show_xmp = false

local pdfe = pdfe or require'pdfe'
local process_stream = require'show-pdf-tags_process_stream'
local text_string_to_utf8 = require'show-pdf-tags_decode'.text_string_to_utf8

local function ordered_pairs(t)
  local keys = {}
  local n = 0
  for k in pairs(t) do
    n = n + 1
    keys[n] = k
  end
  table.sort(keys)
  local i = 0
  return function()
    i = i + 1
    local key = keys[i]
    if key == nil then
      return
    end
    return key, t[key]
  end
end

local function almost_resolve(t, v, i)
  local id
  while t == 10 do
    id = i
    t, v, i = pdfe.getfromreference(v)
  end
  return id, t, v, i
end

local function get_page(elem)
  local id, t, v, i = almost_resolve(pdfe.getfromdictionary(elem, 'Pg'))
  if not t or t < 2 then return end
  assert(id and t == 8, 'page should be a dictionary')
  return id
end

local convert_kids

local function with_warnings(result)
  return function(ctx, warning_generator)
    local warnings = {}
    warning_generator(function(condition, warning)
      if condition then
        warnings[#warnings + 1] = warning
      end
    end, result)
    ctx.warnings[result] = warnings[1] and warnings
    return result
  end
end

local warnings_key = {} -- Just a marker, always empty
local function convert_mc(ctx, mcid, page, stream_id, stream, owner)
  local stream_or_page_id = stream_id or page
  local pageno = ctx.pagenos[page]
  local stream_data = ctx.streams[stream_or_page_id]
  if stream_or_page_id and not ctx.streams[stream_or_page_id] then
    local warnings
    stream_data, warnings = process_stream(
      stream or ctx.document.Pages[ctx.pagenos[page]].Contents,
      stream and stream.Resources or ctx.document.Pages[ctx.pagenos[page]].Resources
    )
    stream_data[warnings_key] = warnings
    ctx.streams[stream_or_page_id] = stream_data
  end
  return with_warnings {
    type = 'MCR',
    page = pageno,
    stream = stream,
    owner = owner,
    content = stream_data[mcid],
  } (ctx, function(warn)
    warn(not page, 'Missing page reference in marked content reference')
    warn(page and not pageno, 'Page referenced in marked content referene does not exist')
    warn(not stream_data, 'No stream referenced in marked content reference')
    warn(stream_data and not stream_data[mcid], 'Referenced marked content sequence not found')
    local warnings = stream_data and stream_data[warnings_key][mcid]
    if warnings then
      for _, warning in ipairs(warnings) do
        warn(true, warning)
      end
    end
  end)
end

local function convert_objr(ctx, obj, page)
  local id, _, obj = assert(almost_resolve(pdfe.getfromdictionary(obj, 'Obj')))
  return {
    type = 'OBJR',
    page = ctx.pagenos[page],--assert(ctx.pagenos[page]), -- TODO: assert(...) once tagpdf is adapted
    ObjId = id,
    Obj = obj,
  }
end

local default_namespace = 'http://iso.org/pdf/ssn'
-- this prefix to be confirmed, another possibility would be data:,
local owner_prefix = 'http://iso.org/pdf/ssn/'

local function get_string(container, index, warnings)
  local text = pdfe.getstring(container, index, true)
  if text then
    local u = text_string_to_utf8:match(text)
    if u == nil then
      warnings[#warnings + 1] = warning
      return "??"
    end
    return u
  else
    return
  end
end

local function pdf2lua(container, index, t, v, x, warnings)
  local saved = {}
  local function recurse(container, index, t, v, x)
    if t == 10 then
      local id
      id, t, v, x = almost_resolve(t, v, x)
      local result = saved[id]
      if result == nil then
        result = recurse(container, index, t, v, x)
        saved[id] = result
      end
      return result
    end
    if not t or t < 2 then
      return
    elseif t < 6 then
      return v
    elseif t == 6 then
      return get_string(container, index, warnings)
    elseif t == 7 then
      local arr = {}
      for i=1, #v do
        arr[i] = recurse(v, i, pdfe.getfromarray(v, i))
      end
      return arr
    elseif t == 8 then
      local dict = {}
      for i=1, #v do
        local k, inner_t, inner_v, detail = pdfe.getfromdictionary(v, i)
        dict[k] = recurse(v, k, inner_t, inner_v, detail)
      end
      return dict
    else
      assert(false, 'Streams are not handled at the moment')
    end
  end
  return recurse(container, index, t, v, x), warnings[0] and warnings
end

local function convert_attributes(ctx, attrs, classes, warnings)
  if not classes and not attrs then return end
  local attributes = {}
  local function apply_attr(attr)
    local owner = assert(attr.O)
    if owner == 'NSO' then
      -- avoid error if  no-namespace attributes to be modelled by missing NS field
      owner = (attr.NS and get_string(attr.NS, 'NS', warnings)) or "" 
    else
      owner = owner_prefix .. owner
    end
    local owner_dict = attributes[owner]
    if not owner_dict then
      owner_dict = {}
      attributes[owner] = owner_dict
    end
    for i = 1, #attr do
      local key, t, v, extra = pdfe.getfromdictionary(attr, i)
      if key ~= 'O' and key ~= 'NS' then
        owner_dict[key] = pdf2lua(attr, key, t, v, extra, warnings)
      end
    end
  end
  local function apply_attrs(attrs)
    if attrs == nil then return end
    local t = pdfe.type(attrs)
    if t == 'pdfe.dictionary' then
      apply_attr(attrs)
    else
      assert(t == 'pdfe.array')
      for i=1, #attrs do
        local attr = attrs[i]
        if type(attr) ~= 'number' then
          apply_attr(attr)
        end
      end
    end
  end
  if classes then
    if type(classes) == 'string' then
      apply_attrs(ctx.ClassMap[classes])
    else
      for i=1, #classes do
        local class = classes[i]
        if type(class) ~= 'number' then
          apply_attrs(ctx.ClassMap[classes[i]])
        end
      end
    end
  end
  if attrs then
    apply_attrs(attrs)
  end
  return attributes
end

local function convert(ctx, elem, id, page)
  if type(elem) == 'number' then
    return convert_mc(ctx, elem, page)
  elseif elem.Type == 'MCR' then
    local stm_id, _, stm = almost_resolve(pdfe.getfromdictionary(elem, 'Stm'))
    return convert_mc(ctx, elem.MCID, get_page(elem) or page, stm_id, stm, elem.StmOwn)
  elseif elem.Type == 'OBJR' then
    return convert_objr(ctx, elem, get_page(elem) or page)
  end
  local warnings = {}
  local ns = elem.NS
  local role_mapped_s, role_mapped_ns
  ns = ns and get_string(ns, 'NS', warnings) or default_namespace
  local obj = {
    subtype = ctx.type_maps[elem.NS and tostring(elem.NS) or false][elem.S],
    attributes = convert_attributes(ctx, elem.A, elem.C, warnings),
    title = get_string(elem, 'T', warnings),
    lang = get_string(elem, 'Lang', warnings),
    alt = get_string(elem, 'Alt', warnings),
    expanded = get_string(elem, 'E', warnings),
    actual_text = get_string(elem, 'ActualText', warnings),
    associated_files = elem.AF,
    id = get_string(elem, 'ID', warnings),
    phoneme = get_string(elem, 'Phoneme', warnings),
    phonetic_alphabet = get_string(elem, 'PhoneticAlphabet', warnings),
    kids = convert_kids(ctx, elem),
  }
  if warnings[1] then
    ctx.warnings[obj] = warnings
  end
  if id then
    ctx.id_map[id] = obj
  end
  local elem_ref = elem.Ref
  if elem_ref and #elem_ref > 0 then
    local ref = {}
    for i = 1, #elem_ref do
      ref[i] = assert(almost_resolve(pdfe.getfromarray(elem_ref, i)))
    end
    obj.ref = ref
    ctx.ref_entries[#ctx.ref_entries + 1] = obj
  end
  return obj
end

function convert_kids(ctx, elem)
  local id, t, k = almost_resolve(pdfe.getfromdictionary(elem, 'K'))
  if not k then return nil end
  local page = get_page(elem)
  if t == 7 then
    local result = {}
    for i = 1, #k do
      local id, t, kid = almost_resolve(pdfe.getfromarray(k, i))
      result[i] = convert(ctx, k[i], id, page)
    end
    return result
  else
    return {convert(ctx, k, id, page)}
  end
end

local function role_map_to_lua(role_map)
  if not role_map then return {} end
  local lua_role_map = pdfe.dictionarytotable(role_map)
  for k, mapping in pairs(lua_role_map) do
    if mapping[1] == 5 then -- name
      mapping[1], mapping[2] = mapping[2], false
    elseif mapping[1] == 7 then -- array
      if mapping[3] == 2 then
        mapping[1], mapping[2], mapping[3] = mapping[2][1], mapping[2][2]
      else
        io.stderr:write"Ignoring entry with invalid length in rolemap\n"
      end
    else
      io.stderr:write"Ignoring invalid rolemap entry\n"
    end
  end
  return lua_role_map
end

local function open(filename)
  local document = pdfe.open(filename)
  if 0 < (pdfe.getstatus(document) or 2) then
    return nil, 'Failed to open document'
  end
  local ctx = {
    document = document,
    streams = {},
  }

  local catalog = pdfe.getcatalog(document)
  local markinfo = catalog and catalog.MarkInfo
  local tagged = markinfo and markinfo.Marked
  local xmp = catalog and catalog.Metadata

  if not tagged then
   io.stderr:write("Document catalog has no markinfo.Marked entry. It might not be tagged.\n")
  end

  local pagenos = {}
  for i, page in ipairs(pdfe.pagestotable(document)) do
    pagenos[page[3]] = i
  end
  ctx.pagenos = pagenos

  local id_map = {}
  ctx.id_map = id_map
  ctx.ref_entries = {}

  ctx.xmp = xmp
  
  local structroot = catalog.StructTreeRoot
  if not structroot then
    return {}, ctx
  end
  local type_maps = {}
  ctx.warnings = setmetatable({}, {__mode = 'k'})
  local global_warnings = {}
  do
    local namespaces = structroot.Namespaces
    for i=0, namespaces and #namespaces or 0 do
      local ns, ns_key, role_map
      if i == 0 then
        ns, ns_key = false, false
        role_map = structroot.RoleMap
      else
        local namespace = namespaces[i]
        ns = get_string(namespace, 'NS', global_warnings)
        ns_key = tostring(namespace)
        role_map = namespace.RoleMapNS
      end
      role_map = role_map_to_lua(role_map)
      type_maps[ns_key] = setmetatable({}, {__index = function(t, elem)
        local element = {subtype = elem, namespace = ns}
        t[elem] = element

        local mapped = role_map[elem]
        if mapped then
          element.mapped = type_maps[mapped[2] and tostring(mapped[2])][mapped[1]]
        end
        return element
      end})
    end
  end
  if global_warnings[1] then warnings[false] = global_warnings end
  ctx.type_maps = type_maps
  ctx.ClassMap = structroot.ClassMap
  local elements = convert_kids(ctx, structroot)
  ctx.ClassMap = nil

  for _, obj in ipairs(ctx.ref_entries) do
    local refs = obj.ref
    for i, ref in ipairs(refs) do
      refs[i] = assert(id_map[ref])
    end
  end
  ctx.ref_entries = nil

  return elements, ctx
end

local function mark_references(tree)
  local count = 0
  local referenced = {}
  local function recurse(objs)
    for _, obj in ipairs(objs) do
      if obj.ref then
        for _, ref in ipairs(obj.ref) do
          if not referenced[ref] then
            count = count + 1
            referenced[ref] = count
          end
        end
      end
      if obj.kids then
        recurse(obj.kids)
      end
    end
  end
  recurse(tree)
  return referenced, count
end

local function format_subtype(subtype)
  if subtype.namespace then
    return string.format('%s (%s)', subtype.subtype, subtype.namespace)
  else
    return subtype.subtype
  end
end

local function format_subtype_xml(subtype)
  if subtype.namespace then
    return string.format('<%s xmlns="%s"', subtype.subtype,
                  (hide_w3c and subtype.namespace:gsub('http://www.w3.org', 'http://-www.w3.org')) or subtype.namespace)
  else
    return "<" .. subtype.subtype:gsub(":","_x3A_")
  end
end

local function print_tree(tree, ctx)
  local referenced = mark_references(tree)
  local function recurse(objs, first_prefix, last_first_prefix, prefix, last_prefix)
    for i, obj in ipairs(objs) do
      local warnings = ctx.warnings[obj]
      if warnings then
        print'### Warnings encountered:'
        for _, warning in ipairs(warnings) do
          print('#  ' .. warning)
        end
      end
      if i == #objs then first_prefix, prefix = last_first_prefix, last_prefix end
      if obj.type == 'MCR' then
        print(string.format('%sMarked content on page %i: %s', first_prefix, obj.page or -1, obj.content or ''))
      elseif obj.type == 'OBJR' then
        local t = obj.Obj.Type
        t = t and string.format(' of type %s', t) or ''
        local page = obj.page
        page = page and string.format(' on page %i', page) or '' -- TODO: Should eventually become always true
        print(string.format('%sReferenced object%s%s', first_prefix, t, page))
      else
        local mark = obj.kids and ':' or ''
        local subtype = obj.subtype
        local mapped = subtype.mapped
        mapped = mapped and ' / ' .. format_subtype(mapped) or ''
        print(string.format('%s%s%s%s', first_prefix, format_subtype(subtype), mapped, mark))
        local lines = {}
        if referenced[obj] then
          lines[#lines + 1] = 'Referenced as object ' .. referenced[obj]
        end
        if obj.title then
          lines[#lines + 1] = 'Title: ' .. obj.title
        end
        if obj.lang then
          lines[#lines + 1] = 'Language: ' .. obj.lang
        end
        if obj.expanded then
          lines[#lines + 1] = 'Expansion: ' .. obj.expanded
        end
        if obj.alt then
          lines[#lines + 1] = 'Alternate text: ' .. obj.alt
        end
        if obj.actual_text then
          lines[#lines + 1] = 'Actual text: ' .. obj.actual_text
        end
        if obj.phoneme then
          lines[#lines + 1] = 'Phoneme: ' .. obj.phoneme
        end
        if obj.phonetic_alphabet then
          lines[#lines + 1] = 'PhoneticAlphabet: ' .. obj.phonetic_alphabet
        end
        if obj.associated_files then
          local af_output = ''
          local total_count = #af_output
          for i, file in ipairs(obj.associated_files) do
            if file.EF.F then
              af_output = '\n└─Content: ' .. pdfe.readwholestream(file.EF.F, true):gsub('\n', '\n  ')
              -- recurse(obj.kids, prefix .. '├─', prefix .. '└─', prefix .. '│ ', prefix .. '  ')
            end
          end
          lines[#lines + 1] = 'Associated files are present:' .. af_output
        end
        if obj.attributes then
          local owners = {}
          for k in next, obj.attributes do
            owners[#owners + 1] = k
          end
          table.sort(owners)
          for i=1, #owners do
            local attrs = {}
            for k in next, obj.attributes[owners[i]] do
              attrs[#attrs + 1] = k
            end
            table.sort(attrs)
            for j=1, #attrs do
              attrs[j] = attrs[j] .. ': ' .. require'show-pdf-tags-inspect'(obj.attributes[owners[i]][attrs[j]])
            end
            table.insert(attrs, 1, (owners[i]:sub(1, #owner_prefix) == owner_prefix and '/' .. owners[i]:sub(#owner_prefix+1) or  owners[i]) .. ':')
            for j=1, #attrs-1 do
              attrs[j] = attrs[j]:gsub('\n', '\n│')
            end
            owners[i] = table.concat(attrs, '\n├', 1, #attrs-1) .. '\n└' .. attrs[#attrs]:gsub('\n', '\n ')
          end
          table.insert(owners, 1, 'Attributes: ')
          for j=1, #owners-1 do
            owners[j] = owners[j]:gsub('\n', '\n│')
          end
          lines[#lines + 1] = table.concat(owners, '\n├', 1, #owners-1) .. '\n└' .. owners[#owners]:gsub('\n', '\n ')
        end
        -- attributes = convert_attributes(elem.A),
        -- attribute_classes = convert_attribute_classes(elem.C),
        if obj.ref then
          local refs = {}
          for i, r in ipairs(obj.ref) do
            refs[i] = referenced[r]
          end
          lines[#lines + 1] = 'References object' .. (refs[2] and 's' or '') .. ' ' .. table.concat(refs, ', ')
        end
        if obj.kids then
          for _, l in ipairs(lines) do
            print(prefix .. '┝━━' .. l:gsub('\n', '\n' .. prefix .. '│  '))
          end
          recurse(obj.kids, prefix .. '├─', prefix .. '└─', prefix .. '│ ', prefix .. '  ')
        elseif #lines > 0 then
          for i=1, #lines-1 do
            print(prefix .. '┝━━' .. lines[i]:gsub('\n', '\n' .. prefix .. '│  '))
          end
          print(prefix .. '┕━━' .. lines[#lines]:gsub('\n', '\n' .. prefix .. '   '))
        end
      end
    end
  end
  return recurse(tree, '', '', '', '')
end




local function print_tree_xml(tree, ctx)
  local referenced = mark_references(tree)
  local function recurse(objs, indent)
    for i, obj in ipairs(objs) do
      local warnings = ctx.warnings[obj]
      if warnings then
        for _, warning in ipairs(warnings) do
          print(string.format('%s<!-- %s -->', indent, warning))
        end
      end
      if obj.type == 'MCR' then
        print(string.format('%s<?MarkedContent page="%i" ?>%s', indent, obj.page or -1, (obj.content and obj.content:gsub('&','&amp;'):gsub('<','&lt;'):gsub('\0','[NULL]'):gsub('[\1-\8\11\12\14-\31]','[CTRL]'):gsub('�.*','[TEXT]') or "[missing]")))
      elseif obj.type == 'OBJR' then
        local t = obj.Obj.Type
        t = t and string.format(' type="%s"', t) or ''
        local page = obj.page
        page = page and string.format(' page="%i"', page) or '' -- TODO: Should eventually become always true
        print(string.format('%s<?ReferencedObject%s%s ?>', indent, t, page))
      else
        local subtype = obj.subtype
	local mapped = subtype.mapped
--        mapped = mapped and mapped.subtype or ''
--        mapped = mapped and ' / ' .. format_subtype(mapped) or ''
        if follow_rolemap and mapped then
          print(string.format('%s%s', indent, format_subtype_xml(mapped)))
	else
          print(string.format('%s%s', indent, format_subtype_xml(subtype)))
	end
        local lines = {}
        if obj.id then
          lines[#lines + 1] = ' id="' .. obj.id:gsub('&','&amp;'):gsub('<','&lt;'):gsub('"','&quot;'):gsub('\0','[NULL]') .. '"'
        end
        if obj.title then
          lines[#lines + 1] = ' title="' .. obj.title:gsub('&','&amp;'):gsub('<','&lt;'):gsub('"','&quot;'):gsub('\0','[NULL]'):gsub('[\1-\8\11\12\14-\31]','[CTRL]') .. '"'
        end
        if obj.lang then
          lines[#lines + 1] = ' lang="' .. obj.lang .. '"'
        end
        if obj.expanded then
          lines[#lines + 1] = ' expansion="' .. obj.expanded:gsub('&','&amp;'):gsub('<','&lt;'):gsub('"','&quot;'):gsub('\0','[NULL]'):gsub('[\1-\8\11\12\14-\31]','[CTRL]')  .. '"'
        end
        if obj.alt then
          lines[#lines + 1] = ' alt="' .. obj.alt:gsub('&','&amp;'):gsub('<','&lt;'):gsub('"','&quot;'):gsub('\0','[NULL]'):gsub('[\1-\8\11\12\14-\31]','[CTRL]')  .. '"'
        end
        if obj.actual_text then
          lines[#lines + 1] = ' actualtext="' .. obj.actual_text:gsub('&','&amp;'):gsub('<','&lt;'):gsub('"','&quot;'):gsub('\0','[NULL]'):gsub('[\1-\8\11\12\14-\31]','[CTRL]') .. '"'
        end
        if obj.phoneme then
          lines[#lines + 1] = ' phoneme="' .. obj.phoneme:gsub('&','&amp;'):gsub('<','&lt;'):gsub('"','&quot;'):gsub('\0','[NULL]'):gsub('[\1-\8\11\12\14-\31]','[CTRL]') .. '"'
        end
        if obj.phonetic_alphabet then
          lines[#lines + 1] = ' phonetic-alphabet="' .. obj.phonetic_alphabet:gsub('&','&amp;'):gsub('<','&lt;'):gsub('"','&quot;'):gsub('\0','[NULL]'):gsub('[\1-\8\11\12\14-\31]','[CTRL]') .. '"'
        end
        if obj.associated_files then
	  local f = {}
	  local warnings = {}
	  for i, file in ipairs(obj.associated_files) do
            if file.EF.F then
	      f[#f+1] = get_string(file, "UF", warnings) 
            end
	  end
	  for _, warning in ipairs(warnings) do
            io.stderr:write('Warning while processing associated files: ' .. warning .. '\n')
	  end
          lines[#lines + 1] = ' af="' .. table.concat(f, ' ') .. '"'
        end
        if obj.attributes then
	  for k,v in ordered_pairs(obj.attributes) do
            local attrns=""
	    if k~=subtype.namespace then
	      attrns = k:gsub('.*/','')
	    end
            if type(v) == "table" then
	      if attrns ~= "" then
                lines[#lines +1] = ' xmlns:' .. attrns .. '="' .. k .. '"'
		attrns = attrns ..':'
              end
              for kk,vv in ordered_pairs(v) do
	        if type(vv) == "table" then
	          vv = require'show-pdf-tags-inspect'(vv):gsub('\n[ ]*',' ')
	        end
                lines[#lines+1] = ' ' ..attrns .. kk .. '="' .. tostring(vv):gsub('&','&amp;'):gsub('<','&lt;'):gsub('"','&quot;'):gsub('\0','[NULL]') .. '"'
              end
            else
              io.stderr:write("Unexpected attributes object\n")
            end
          end
        end
	if mapped and mapped.subtype then
          if follow_rolemap then
	    if subtype.namespace then
              lines[#lines+1] = ' xmlns:orig-ns="' .. subtype.namespace .. '"'
              lines[#lines+1] = ' rolemapped-from="orig-ns:' .. subtype.subtype .. '"'
	    else
              lines[#lines+1] = ' rolemapped-from="' .. subtype.subtype .. '"'
	    end
	  else
            lines[#lines+1] = ' rolemaps-to="' .. mapped.subtype .. '"'
	  end
	end
        -- attributes = convert_attributes(elem.A),
        -- attribute_classes = convert_attribute_classes(elem.C),
        if referenced[obj] then
          lines[#lines + 1] = ' referenced-as="' .. referenced[obj] .. '"'
        end
	lines[#lines+1] = ">"
--
        if obj.associated_files then
	  local f = {}
          local af_output = ''
	  local warnings = {}
	  for i, file in ipairs(obj.associated_files) do
            if file.EF.F then
	      af_output = pdfe.readwholestream(file.EF.F, true)
              lines[#lines + 1] = '<AssociatedFile name="' .. get_string(file, "UF", warnings) .. '" xmlns="">'
	      if file.EF.F.Subtype == 'application/mathml+xml' then
                lines[#lines + 1] = af_output
	      else
                lines[#lines + 1] = af_output:gsub('&','&amp;'):gsub('<','&lt;'):gsub('"','&quot;'):gsub('\0','[NULL]'):gsub('[\1-\8\11\12\14-\31]','[CTRL]')
              end	  
              lines[#lines + 1] = '</AssociatedFile>'
            end
	  end
        end
--
        if obj.ref then
          local refs = {}
          for i, r in ipairs(obj.ref) do
            refs[i] = referenced[r]
          end
          lines[#lines + 1] = '<?References objects="' .. table.concat(refs, ' ') ..'" ?>'
        end
        if obj.kids then
          for _, l in ipairs(lines) do
            print(indent .. '  ' .. l:gsub('\n', '\n' .. indent .. '  '))
          end
          recurse(obj.kids, indent .. ' ')
        if follow_rolemap and mapped then
	  print(indent .. "</" .. mapped.subtype ..">")
	else
	  print(indent .. "</" .. subtype.subtype:gsub(":","_x3A_") ..">")
	end
        elseif #lines > 0 then
          for i=1, #lines-1 do
            print(indent .. '  ' .. lines[i]:gsub('\n', '\n' .. indent .. '  '))
          end
          print(indent .. '  ' .. lines[#lines]:gsub('\n', '\n' .. indent .. '   '))
          if follow_rolemap and mapped then
            print(indent .. "</" .. mapped.subtype ..">")
	  else
            print(indent .. "</" .. subtype.subtype:gsub(":","_x3A_") ..">")
	  end
        end
      end
    end
  end
  print ("<PDF>\n <StructTreeRoot>")
  recurse(tree, '  ', '', '', ' ')
  print (" </StructTreeRoot>")
  if show_xmp then
    print(" <XMP>\n" ..pdfe.readwholestream(ctx.xmp,true):gsub("\n[ \n]*\n","\n") .. "\n </XMP>")
  end
  print ("</PDF>")
  return
end


local helpstr =[[

Usage: %s options <filename>.pdf
Options
  --help|-h        show this help
  --version|-v     show the current version
  --tree (default) show as tree
  --xml            show as XML
  --table          show Lua table structure
  --map            Follow role mapping (xml printer)
  --xmp            include XMP XML (xml printer)
  --w3c-           Add - to w3c namespaces to force browser tree display

]]

local argi = 1
while argi <= #arg and arg[argi]:match("^%-") do
  if arg[argi] == "--tree" then
    out_format="tree"
  elseif arg[argi] == "--xml" then
    out_format="xml"
  elseif arg[argi] == "--table" then
    out_format="table"
  elseif arg[argi] == "--map" then
    follow_rolemap=true
  elseif arg[argi] == "--w3c-" then
    hide_w3c=true
  elseif arg[argi] == "--xmp" then
    show_xmp=true
  elseif arg[argi] == "--help" or arg[argi] == "-h" then
    io.stderr:write(string.format(helpstr, arg[0]))
    return
  elseif arg[argi] == "--version" or arg[argi] == "-v" then
    io.stderr:write(string.format("show-pdf-tags version: %s\n", show_pdf_tags_version))
    return
  else
    io.stderr:write(string.format('Unknown option: %s\n', arg[argi]))
    return
  end
  argi=argi+1
end

if argi < #arg then
  io.stderr:write(string.format('Extra argument. Usage: %s options <filename>.pdf\n', arg[0]))
  return
end

if argi > #arg then
  io.stderr:write(string.format('Missing argument. Usage: %s options <filename>.pdf\n', arg[0]))
  return
end

local struct, ctx = assert(open(arg[argi]))

if out_format=="tree" then
  print_tree(struct, ctx)
else
  if out_format=="xml" then
    print_tree_xml(struct, ctx)
  else
    print(require'show-pdf-tags-inspect'(struct))
  end
end
