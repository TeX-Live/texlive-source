local format = string.format
local table_insert = table.insert
local table_remove = table.remove
local table_concat = table.concat
local table_sort = table.sort
local print_int    = mflua.MF.print_int
local abs = math.abs
local math_floor = math.floor
local math_tointeger = math.tointeger or math.floor

local print_scaled = mflua.MF.print_scaled
local unity = mflua.MF.unity

local GF = mflua.GF
local GF_chars = GF.chars

local ttx	= {}
local xmlstream = {}
local scrt	= {} -- general table for local use




local function DUMP(t)
   if type(t) == 'table' then 
      print("table",#table)
      for k,v in pairs(t) do print(k,v) end
   end
end

local function print_err(msg)
   print(format("! ttx backend: %s",tostring(msg)))
end

local function tointeger(n)
   return math_tointeger(n)
end

local function _normalize(w,n)
   n = tonumber(n) or 5
   n = math_floor(abs(n))
   n = (n<25 and n) or 24
   local fmt = format("%%0.%sf", n)
   local W = tonumber(format(fmt,w))
   nx = tointeger(W)
   if nx==W then
      return nx
   else
      return w
   end
end

local function normalize(w,n)
   return print_scaled(w)
end

local function _normalize_and_simplify(w,n)
   n = tonumber(n) or 2
   n = math_floor(abs(n))
   n = (n<25 and n) or 24
   local fmt = format("%%0.%sf", n)
   local W = tonumber(format(fmt,w))
   nx = tointeger(W)
   if nx==W then
      return nx
   else
      return W
   end
end

local function normalize_and_simplify(w,n)
   return print_scaled(w)
end


local function toscaled(n)
   return tonumber(format("%0.0f",n*unity))
end

local function table_sorted(t)
   if type(t)=='table' then 
     table_sort(t) 
     return t
   else 
    return {t}
   end
end


local xml_comment                      = 'c'
local xml_processing_instruction       = 'p'
local xml_element                      = 'e'
local xml_header                       = 'h'


--------------------------------------------------------------------------------
--
-- Errors and warning
--------------------------------------------------------------------------------
local function warning(t,msg)
   print("Warning:"..msg..".")
   print("Element:")
   if type(t)=='table' then
      for k,v in pairs(t) do print(k,v) end
      print(#t.." values")
   else
      print(tostring(s))
   end
end


--------------------------------------------------------------------------------
--
-- Element management
--------------------------------------------------------------------------------


-- Todo: valid XML  totext & toattr & tocomment 
local function totext(s)
   if not(type(s)=='string') then
      warning(s,"text:the data in not a string")
   end
   return tostring(s)
end

local function toattr(s)
   if not(type(s)=='string') then
      warning(s,"attr.:the data in not a string")
   end
   return tostring(s)
end

local function tocomment(s)
   if not(type(s)=='string') then
      warning(s,"comment:the data in not a string")
   end
   return tostring(s)
end

local function new_comment(text)
   local t ={}
   t[0]=xml_comment 
   t[1]=false
   t[2]=tocomment(text)
   return t
end

local function new_element(name,attr)
   local t ={}
   t[0]=xml_element
   t[1]=tostring(name)
   if attr and type(attr)=='table' then
      for k,v in pairs(attr) do
	 if type(k)~='number' then
	    t[k]=v
	 end
      end
   end
   return t
end

local function get_elementname(t)
   if t and type(t)=='table' then
      return t[1]
   else
      return nil
   end
end

local function insert_child(f,c)      
   table_insert(f,c)
end

local function insert_child_at_beginning(f,c)      
   -- f[1] is always the name
   table_insert(f,2,c)
end

local function insert_children(f,cc)      
   for j=1,#cc do 
      table_insert(f,cc[j])
   end
end


--
-- get all the children (not descendants!)
-- of source that match target name and attr.
-- The existence of an attribute can be checked
-- with @attrname=true (not @attrname="true") in
-- target
local function get_children(source,target)
   local res={}
   local t_attr={}
   -- only key/val from target
   for k,v in pairs(target) do
      if type(k)=='number' then
	 t_attr[k]=v
      end
   end
   for i=2,#source do
      local e = source[i]
      if e[1]==target[1] then
	 local found=true
	 for k,v in pairs(t_attr) do
	    if not(e[k] and ((e[k]==v) or (v==true))) then
	       found=false
	       break
	    end
	 end
	 if found then
	    res[#res+1]=e
	 end
      end
   end
   return res
end

--
-- remove the first xml_element inserted
-- with name. Return the element removed
-- false otherwise
local function remove_child(t,name)
   for i=1,#t do
      if t[i][0]==xml_element and t[i][1]==tostring(name) then
	 return table.remove(t,i)
      end
   end
   return false
end

--
-- call remove_child on all elements of
-- names 

local function remove_children(t,names)
   local res = {}
   local name, r
   for _,v in pairs(names) do
      name = tostring(v)
      r = remove_child(t,name)
      if r then
	 table_insert(res,r)
      end
   end
   return res
end



--------------------------------------------------------------------------------
--
-- Serialization
---------------------------------------------------------------------------------

local function  serialize_comment(t) 
   if not(t[0]== xml_comment) then
      warning(t,'the node to serialize is not a comment')
      return
   end
   local name=t[1] -- false, discarded
   local elem={}
   elem[#elem+1]='<--'
   elem[#elem+1]=tocomment(t2)
   elem[#elem+1]='-->'
   elem[#elem+1]='\n'
   xmlstream[#xmlstream+1] = table_concat(elem)
end


local function  serialize_processing_instruction(t) 
   if not(t[0]== xml_processing_instruction) then
      warning(t,'the node to serialize is not a processing_instruction')
      return
   end
   local name=t[1]
   local elem={}
   elem[#elem+1]='<?'
   elem[#elem+1]=name
   local _t={}
   for k,v in pairs(t) do
      if not(type(k)=='number') then
	 _t[#_t+1]=k
     end
   end
   for j=1,#_t do 
      elem[#elem+1]=format([[ %s="%s"]],_t[j],toattr(t[_t[j]]))  
   end
   elem[#elem+1]='?>'
   elem[#elem+1]="\n"
   xmlstream[#xmlstream+1] = table_concat(elem)
end

local function  serialize_xml_header(t) 
   local _t={}
   local elem={}
   elem[#elem+1]='<?'
   elem[#elem+1]='xml '
   elem[#elem+1]=t
   elem[#elem+1]='?>'
   elem[#elem+1]='\n'
   xmlstream[#xmlstream+1] = table_concat(elem)
end

local function  serialize_empty_elem(t) 
  local name=t[1]
  local elem={}
  elem[#elem+1]='<'
  elem[#elem+1]=name
  local _t={}
  for k,v in pairs(t) do
     if not(type(k)=='number') then
	_t[#_t+1]=k
     end
  end
  table.sort(_t)
  for j=1,#_t do 
     elem[#elem+1]=format([[ %s="%s"]],_t[j],toattr(t[_t[j]]))
  end
  elem[#elem+1]='/>'
  elem[#elem+1]="\n"
  xmlstream[#xmlstream+1] = table_concat(elem)
end

local function  serialize_elem(t) 
   if #t==1 then 
    serialize_empty_elem(t) 
  else
   local name=t[1]
   local elem={}
   elem[#elem+1]='<'
   elem[#elem+1]=name
   -- attributes 
   local _t={}
   for k,v in pairs(t) do
      if not(type(k)=='number') then
	 _t[#_t+1]=k
      end
   end
   table.sort(_t)
   for j=1,#_t do 
      elem[#elem+1]=format([[ %s="%s"]],_t[j],toattr(t[_t[j]])) --
   end
   elem[#elem+1]='>'
   if type(t[2])~= 'string' then 
   elem[#elem+1]="\n"
   end
   xmlstream[#xmlstream+1] = table_concat(elem)
   elem = {}
   for j=2,#t do
      if type(t[j])=='table' then
	 serialize_elem(t[j])
      elseif type(t[j])=='string' then
	 xmlstream[#xmlstream+1] = totext(t[j])
      end
   end
   elem[#elem+1]='</'
   elem[#elem+1]=name
   elem[#elem+1]='>'
   elem[#elem+1]="\n"
   xmlstream[#xmlstream+1] = table_concat(elem)
  end
end


--------------------------------------------------------------------------------
--
-- First children of XML ttx, plus utilities
--------------------------------------------------------------------------------

local ttFont     
local GlyphOrder 
local GlyphID    
local head       
local hhea       
local maxp       
local OS_2,panose -- handy to have        
local name       
local cmap       
local post       
local CFF, CFFFont, Private   -- only a single CFFFont, for the moment
local FFTM       
local GDEF  
local hmtx       

-- For variable fonts
local extranames -- handy to have 
local CFF2
local HVAR,VarStore,VarRegionList,Regions,VarRegionAxis,VarData,AdvWidthMap
local fvar


-- Important, child of CFFFont 
local CharStrings

local function get_GlyphID(id,name)
   return new_element('GlyphID',{['id']=tostring(id),['name']=tostring(name)})
end

local function put_head(name,value)
   table_insert(head, new_element(name, {['value']=tostring(value)}))
end

local function put_hhea(name,value)
   table_insert(hhea, new_element(name, {['value']=tostring(value)}))
end

local function put_maxp(name,value)
    table_insert(maxp,new_element(name, {['value']=tostring(value)}))
end

local function put_OS_2(name,value)
    table_insert(OS_2,new_element(name, {['value']=tostring(value)}))
end
-- handy 
local function put_panose(name,value)
    table_insert(panose,new_element(name, {['value']=tostring(value)}))
end

local function get_map(code,name)
   return new_element('map', {['code']=tostring(code),['name']=tostring(name)})
end

local function put_post(name,value)
    table_insert(post,new_element(name, {['value']=tostring(value)}))
end

local function put_CFF(name,value)
    table_insert(CFF,new_element(name, {['value']=tostring(value)}))
end

local function put_CFF2(name,value)
    table_insert(CFF2,new_element(name, {['value']=tostring(value)}))
end


local function put_CFFFont(name,value)
    table_insert(CFFFont,new_element(name, {['value']=tostring(value)}))
end

local function put_Private(name,value)
    table_insert(Private,new_element(name, {['value']=tostring(value)}))
end

local function put_FFTM(name,value)
    table_insert(FFTM,new_element(name, {['value']=tostring(value)}))
end

local function get_ClassDef(glyph,class)
   return new_element('ClassDef', {['glyph']=tostring(glyph),['class']=tostring(class)})
end

local function get_mtx(name,width,lsb)
   return new_element('mtx', {['name']=tostring(name),['width']=tostring(width),['lsb']=tostring(lsb)})
end

--
-- convenient method for a cmap0
--
-- local function get_cmap0_maps(maps) 
--    local t = {}
--    if not(#maps==256) then
--       warning(maps,"maps has not 256 values")
--    end
--    local s=(#maps>=256 and 256) or #maps
--    for j=1,s do
--       t[#t+1]=get_map(format("0x%x",j-1),maps[j])
--    end
--    return t
-- end

local function get_raw_cmap0() 
   local t = {}
   for j=0,255 do 
      t[format("0x%02x",j)] = ".notdef"
   end
   return t
end




--------------------------------------------------------------------------------
--
-- Create & save xml font
--------------------------------------------------------------------------------

local function checkall(tfm)
   -- several checks & fixes here
   local gf_chars = {}
   if GF then
      for k,_ in pairs(GF.chars) do
	 if type(k)=='number' then
	    table_insert(gf_chars,k)
	 end
      end
      table_sort(gf_chars)
   end
   --
   -- fix head
   if ttx.userdata.update_head and
   type(ttx.userdata.update_head)=='function' then
      ttx.userdata.update_head(tfm)
   elseif GF then 
      remove_children(head,{'xMin', 'xMax','yMin','yMax','modified'})
      put_head('xMin',GF_chars.min_m);
      put_head('yMin',GF_chars.min_n);
      put_head('xMax',GF_chars.max_m-GF_chars.min_m+1);
      put_head('yMax',GF_chars.max_n-GF_chars.min_n+1);
      -- Date is UTC
      put_head('modified',os.date("!%c"));
   end
   --
   -- fix CFFFont, only one for the moment
   if ttx.userdata.update_CFFFont and
   type(ttx.userdata.update_CFFFont)=='function' then
      ttx.userdata.update_CFFFont(CFFont)
   elseif GF then 
      remove_child(CFFFont,'FontBBox')
      put_CFFFont('FontBBox',
		  format("%s %s %s %s",GF_chars.min_m,GF_chars.min_n,
			 GF_chars.max_m-GF_chars.min_m+1,
			 GF_chars.max_n-GF_chars.min_n+1))
   end
   --
   -- fix OS_2
   if ttx.userdata.update_OS_2 and
   type(ttx.userdata.update_OS_2)=='function' then
      ttx.userdata.update_CFFFont(OS_2)
   elseif mflua.chartable then
      local design_size=tonumber ( print_scaled(MFbuiltin.designsize()) ) --pt 
      local emsize = ttx.emsize or 1000
      local avgx = 0
      local avgx_cnt=0
      for k,v in pairs(mflua.chartable) do
	 if tonumber(v.char_wd) > 0 then 
	    avgx = tonumber(v.char_wd)*emsize/design_size+avgx
	    avgx_cnt=avgx_cnt+1
	 end
      end
      avgx = math_floor(avgx/avgx_cnt+0.5)
      remove_child(OS_2,'xAvgCharWidth')
      put_OS_2('xAvgCharWidth',avgx)
   end
   -- fix cmap0
   -- local t = {}
   -- for k,_ in pairs(scrt.cmap.rawcmap0) do t[#t+1]=k end
   -- table.sort(t)
   -- for k=1,256 do
   --    insert_child(scrt.cmap.cmap0, get_map(t[k],scrt.cmap.rawcmap0[t[k]]))
   -- end
   
end

local function savettxfont(t)
   serialize_xml_header([[version="1.0" encoding="UTF-8"]])
   serialize_elem(t) 
   return table_concat(xmlstream)
   
end

local function makefont(tfm)
   --
   checkall(tfm)
   serialize_xml_header([[version="1.0" encoding="UTF-8"]])
   serialize_elem(ttFont)
   return table_concat(xmlstream)
end




--------------------------------------------------------------------------------
--
-- mflua layer
--------------------------------------------------------------------------------

local function _eval_tonumber(q,offset)
   local qx,qy,xo,yo
   local w 
   local _offset = offset 
   if _offset == nil then _offset = '(0,0)' end
   w=string.gmatch(q,"[-0-9.]+"); qx,qy=w(),w()
   w=string.gmatch(_offset,"[-0-9.]+"); xo,yo=w(),w()
   return {tonumber(qx+xo),tonumber(qy+yo)}
end

local function setup(tfm,index,chartable)
   -- Default emsize is 1000 for CFF and  CFF2
   ttx.emsize = ttx.emsize or 1000

   scrt.font_space = tfm.printfloat(ttx.emsize*(tfm.font.space),3)
   
   ttFont     = new_element('ttFont', {['sfntVersion']="OTTO", ['ttLibVersion']="3.1"})
   GlyphOrder = new_element('GlyphOrder')
   GlyphID    = new_element('GlyphID', {['id']="0", ['name']=".notdef"})
   head       = new_element('head')
   hhea       = new_element('hhea')
   maxp       = new_element('maxp')
   OS_2       = new_element('OS_2')
   name       = new_element('name')
   cmap       = new_element('cmap')
   post       = new_element('post')
   if ttx.cffver==1 then 
      CFF        = new_element('CFF')
      CFF2       = nil
   elseif ttx.cffver==2 then
      CFF2       = new_element('CFF2')
      CFF        = nil
   end      
   -- FFTM       = new_element('FFTM') --FontForge TimeStamp table
   GDEF       = new_element('GDEF')
   if ttx.cffver==2 then
      HVAR       = new_element('HVAR')
   end
   hmtx       = new_element('hmtx')

   -- handy 
   panose     = new_element('panose')
   if ttx.cffver==1 then 
      CFFFont    = new_element('CFFFont',{['name']= (ttx.userdata.CFFFont['FullName'] or "mfluatestfont"..tostring(math.random(0,1000000)))})
   elseif ttx.cffver==2 then
      CFFFont    = new_element('CFFFont',{['name']="CFF2Font"})
   end
   Private    = new_element('Private')

   CharStrings= new_element('CharStrings')
   
   table_insert(ttFont,GlyphOrder)
   table_insert(ttFont,head)
   table_insert(ttFont,hhea)
   table_insert(ttFont,maxp)
   table_insert(ttFont,OS_2)
   table_insert(ttFont,name)
   table_insert(ttFont,cmap)
   table_insert(ttFont,post)
   if ttx.cffver==1 then 
      table_insert(ttFont,CFF)
   elseif ttx.cffver==2 then 
      table_insert(ttFont,CFF2)
   end
   --   table_insert(ttFont,FFTM)
   table_insert(ttFont,GDEF)

   if ttx.cffver==2 then
      table_insert(ttFont,HVAR)
      insert_child(HVAR,{'Version', ['value']="0x00010000"})
      VarStore = new_element('VarStore',{['Format']="1"}) 
      insert_child(VarStore,{'Format',['value']="1"})
      VarRegionList = new_element('VarRegionList') 
      -- For the moment, 1 axis and 1 region 
      Regions = {new_element('Region',{['index']="0"})}
      VarRegionAxis = new_element('VarRegionAxis',{['index']="0"})
      insert_child(VarRegionAxis,new_element('StartCoord',{['value']="-1.0"}))
      insert_child(VarRegionAxis,new_element('PeakCoord',{['value']="-1.0"}))
      insert_child(VarRegionAxis,new_element('EndCoord',{['value']="0.0"}))
      -- insert_child(VarRegionAxis,new_element('StartCoord',{['value']="0.0"}))
      -- insert_child(VarRegionAxis,new_element('PeakCoord',{['value']="1.0"}))
      -- insert_child(VarRegionAxis,new_element('EndCoord',{['value']="1.0"}))
      insert_child(Regions[1],VarRegionAxis)
      insert_children(VarRegionList,Regions)
      insert_child(VarStore,VarRegionList)
      VarData = new_element('VarData',{['index']="0"})
      insert_child(VarData,new_element('NumShorts',{['value']="1"}))
      insert_child(VarData,new_element('VarRegionIndex',{['value']="0"}))
      insert_child(VarData,new_element('Item',{['index']="0", ['value']="[-125]"}))
      insert_child(VarStore,VarData)
      AdvWidthMap = new_element('AdvWidthMap')
      insert_child(AdvWidthMap,new_element('Map', {['index']="0", ['outer']="0", ['inner']="0"}))
      insert_child(HVAR,VarStore)
      --insert_child(HVAR,AdvWidthMap)
   end
   
   table_insert(ttFont,hmtx)
   
   table_insert(GlyphOrder,GlyphID)

   if ttx.userdata.head then
      for k,v in pairs(ttx.userdata.head) do
	 put_head(k,v)
      end
   end

   if ttx.userdata.hhea then
      for k,v in pairs(ttx.userdata.hhea) do
	 put_hhea(k,v)
      end
   end

   if ttx.userdata.maxp then
      for k,v in pairs(ttx.userdata.maxp) do
	 put_maxp(k,v)
      end
   end

   if ttx.userdata.OS_2 then
      for k,v in pairs(ttx.userdata.OS_2) do
	 put_OS_2(k,v)
      end
   end

   if ttx.userdata.panose then
      for k,v in pairs(ttx.userdata.panose) do
	 put_panose(k,v)
      end
   end
   insert_child(OS_2,panose)
   
   if ttx.userdata.namerecord then
      local _e
      local t
      for i=1,#ttx.userdata.namerecord do
	 t = ttx.userdata.namerecord[i]
	 _e = new_element('namerecord',{['nameID']=t.nameID,['platformID']=t.platformID,
			     ['platEncID']=t.platEncID, ['langID']=t.langID, ['unicode']=t.unicode})
	 insert_child(_e,t.value); insert_child(name,_e)
      end
   end
      
   
   local cmap0,cmap4_0,cmap4_3
   cmap4_0 = new_element('cmap_format_4', {['platformID']="0",['platEncID']="3",['language']="0",})
   --cmap0   = new_element('cmap_format_0', {['platformID']="1",['platEncID']="0",['language']="0",})
   cmap4_3 = new_element('cmap_format_4', {['platformID']="3",['platEncID']="1",['language']="0",})

   
   insert_child(cmap4_0,get_map("0x0",".notdef"))
   insert_child(cmap4_3,get_map("0x0",".notdef"))
   
   scrt.cmap = scrt.cmap or {}
   scrt.cmap.cmap4_0  = cmap4_0 
   scrt.cmap.cmap4_3  = cmap4_3 
   --scrt.cmap.cmap0    = cmap0
   --scrt.cmap.rawcmap0 = get_raw_cmap0() 

   
   insert_child(cmap,{'tableVersion', ['version']="0"})
   insert_child(cmap,cmap4_0)
   --insert_child(cmap,cmap0)
   insert_child(cmap,cmap4_3)


   if ttx.userdata.post then
      for k,v in pairs(ttx.userdata.post) do
	 put_post(k,v)
      end
   end
   if ttx.cffver==2 then
      insert_child(post,{'psNames'})
      extranames = new_element('extraNames')
      insert_child(post,extranames)
   end
   
   if ttx.cffver==1 then 
      put_CFF('major',"1")
      put_CFF('minor',"0")
      insert_child(CFF,CFFFont)
   elseif ttx.cffver==2 then 
      put_CFF2('major',"2")
      put_CFF2('minor',"0")
      insert_child(CFF2,CFFFont)
   end

   if ttx.cffver==1 then 
      if ttx.userdata.CFFFont then
	 for k,v in pairs(mflua.ttx.userdata.CFFFont) do
	    put_CFFFont(k,v)
	 end
      end

      insert_child(CFFFont,{'Encoding', ['name']=mflua.ttx.userdata.CFFFont.Encoding})

      insert_child(CFFFont,Private)
      if ttx.userdata.CFFFont.Private then
	 for k,v in pairs(ttx.userdata.CFFFont.Private) do
	    put_Private(k,v)
	 end
      end 

      local subrs = new_element('Subrs')
      scrt.subrs  = subrs
      if ttx.userdata.CFFFont.Private.Subrs then
	 for k,v in pairs(ttx.userdata.CFFFont.Private.Subrs) do
	    insert_child(subrs,{'CharString', ['index']=k, v})
	 end
      end
      insert_child(Private,subrs)

   elseif ttx.cffver==2 then 
      insert_child(CFFFont,{'FontMatrix', ['value']=mflua.ttx.userdata.CFF2Font['FontMatrix']})
      local FDArray  = new_element('FDArray')
      local FontDict = new_element('FontDict', {['index']="0"})
      insert_child(FontDict,{'FontMatrix', ['value']=mflua.ttx.userdata.CFF2Font['FontMatrix']})
      insert_child(FontDict,{'Private'})
      insert_child(FDArray,FontDict)
      insert_child(CFFFont,FDArray)
   end
      
   -- .notdef char
   local notdef = new_element('CharString', {['name']=".notdef"})
   if ttx.cffver == 1 then 
      insert_child(notdef,"endchar\n")
   elseif ttx.cffver == 2 then
      insert_child(notdef,"0 0  0 0  0 0 rrcurveto \n")
   end

   insert_child_at_beginning(CharStrings, notdef)
   
   insert_child(CFFFont,CharStrings)

   if CFF2 and  mflua.ttx.userdata.varstore then
      insert_child(CFFFont,mflua.ttx.userdata.varstore)
   end
   
   
   --
   -- TODO: check GlobalSubrs!!
   --
   local globsubrs = new_element('GlobalSubrs')
   if ttx.cffver==1 then 
      insert_child(CFF,globsubrs)
   elseif ttx.cffver==2 then 
      insert_child(CFF2,globsubrs)
   end
   if ttx.userdata.CFF.GlobalSubrs then
      for k,v in pairs(ttx.userdata.CFF.GlobalSubrs) do
	 insert_child(globsubrs,{'CharString', ['index']=k, v})
      end
   end

   local glyphclassdef = new_element('GlyphClassDef',{['Format']="2"})
   insert_child(GDEF,{'Version',['value']="0x00010000"})
   insert_child(GDEF,glyphclassdef)
   scrt.gdef               = scrt.gdef or {}
   scrt.gdef.glyphclassdef = glyphclassdef

   local LigCaretList = new_element('LigCaretList')
   insert_child(LigCaretList,new_element('Coverage',{['Format']="2"}))
   insert_child(GDEF,LigCaretList)


   insert_child(hmtx, get_mtx('.notdef',
                               ttx.userdata.char_notdef['width'],
                               ttx.userdata.char_notdef['lsb']))

   if CFF2 and ttx.userdata.fvar then
      local fvar = new_element('fvar')
      local v, axis, _e
      for i=1,#ttx.userdata.fvar.Axis do
	 v = ttx.userdata.fvar.Axis[i]
	 axis = new_element('Axis')
	 _e = new_element('AxisTag');	   _e[2] = v['AxisTag'];      insert_child(axis,_e)
	 _e = new_element('MinValue');	   _e[2] = v['MinValue'];     insert_child(axis,_e)
	 _e = new_element('DefaultValue'); _e[2] = v['DefaultValue']; insert_child(axis,_e)
	 _e = new_element('MaxValue');	   _e[2] = v['MaxValue'];     insert_child(axis,_e)
	 _e = new_element('AxisNameID');   _e[2] = v['AxisNameID'];   insert_child(axis,_e)
      end
      insert_child(fvar,axis)
      insert_child(ttFont,fvar)
   end
   
end


local function reverse(cycle)
   local t={}
   local curve
   local p,c1,c2,q
   for i=#cycle,1,-1 do
      curve=cycle[i]
      p,c1,c2,q=curve[1],curve[2],curve[3],curve[4]
      curve[1],curve[2],curve[3],curve[4]=q,c2,c1,p
      t[#t+1]=curve
   end
   return t
end


local function sort(cycle)
   local t={}
   local curve
   local p,q, index
   local xmin,ymin = 4096,4096
   index = 1
   for i=1,#cycle do
      curve=cycle[i]
      p=curve[1]; 
      t = _eval_tonumber(p)
      -- Align to leftmost x and y 
      --if t[1]<=xmin and t[2]<=ymin then index = i;xmin,ymin=t[1],t[2]  end
      -- Align to the leftmost x
      if t[1]<=xmin  then index = i;xmin,ymin=t[1],t[2]  end
   end
   t={}
   for i=index,#cycle do t[#t+1]=cycle[i] end
   for i=1,index-1    do t[#t+1]=cycle[i] end
   for i=1,#t do
      curve=t[i]
      p,c1,c2,q=curve[1],curve[2],curve[3],curve[4]
    end
    return t
end


local function _reverse(cycle)
  local cv1, cv2
  local p,c1,c2,q
  local l=#cycle
  for i=1,l/2 do
     cv1=cycle[i]
     cv2=cycle[l+1-i]
     p,c1,c2,q = cv1[1],cv1[2],cv1[3],cv1[4]
     cv1[1],cv1[2],cv1[3],cv1[4] = q,c2,c1,p 
     p,c1,c2,q = cv2[1],cv2[2],cv2[3],cv2[4]
     cv2[1],cv2[2],cv2[3],cv2[4] = q,c2,c1,p 
     cycle[l+1-i]=cv1
     cycle[i]=cv2
   end
end


-- local function updateminmax(p,c1,c2,q)
--   ttx = ttx or {}
--   ttx.userdata = ttx.userdata or {}
--   ttx.userdata.head = ttx.userdata.head or {}
--   local t;
--   -- hm not ok, too raw
--   -- we need at least 3 bisections
--   t = {p[1],c1[1],c2[1],q[1]}
--   table_sort(t) 
--   ttx.userdata.head['xMin']= math_floor(t[1])
--   ttx.userdata.head['xMax']= math_floor(t[4])
--   t = {p[2],c1[2],c2[2],q[2]}
--   table_sort(t) 
--   ttx.userdata.head['yMin']= math_floor(t[1])
--   ttx.userdata.head['yMax']= math_floor(t[4])
-- end


local function update_char_lsb(glyph_name,char_wd_emunit,index)
   local err = true
   if ttx.userdata.update_char_lsb and
   type(ttx.userdata.update_char_lsb)=='function' then
      ttx.userdata.update_char_lsb(glyph_name,char_wd_emunit,index)
      return
   end
   if GF_chars[index] then
      insert_child(hmtx, get_mtx(glyph_name,
				 char_wd_emunit,GF_chars[index].min_m))
      return
   end
   if err then print_err("update_char_lsb failed") end
end





local function isvhline(py,c1y,c2y,qy)
  if (py==c1y) and (c1y==c2y) and (c2y==qy) then 
    return true
  end
  --local abs = math.abs
  local eps = ttx.eps or 0.01
  local d1,d2,d3,d4 = abs(py-c1y), abs(c1y-c2y), abs(c2y-qy),abs(py-qy)
  if d1<eps and d2<eps and d3<eps and d4<eps then 
    return true 
  end
  return false
end


local function isline(px,py,c1x,c1y,c2x,c2y,qx,qy)
  local px,py,c1x,c1y,c2x,c2y,qx,qy = 0,0,c1x-px,c1y-py,c2x-px,c2y-py,qx-px,qy-py
  local eps = ttx.deps or 0.01
  local atan2,abs = math.atan2, math.abs
  a  = atan2(qy,qx)
  a1 = atan2(c1y,c1x) 
  a2 = atan2(c2y,c2x) 
  if abs(a-a1)<eps and abs(a-a2)<eps and abs(a1-a2)<eps then 
   return true 
  end
 return false
end


local function insert_stem(index,stem_type)
 if mflua and mflua.userdata and mflua.userdata.char and mflua.userdata.char[index]  and mflua.userdata.char[index][stem_type] then
     -- stem[i]={base, delta} 
     local stem = mflua.userdata.char[index][stem_type]
     local content = {}
     for i=1,#stem do 
	--table_insert(content, format("%s %s", normalize(stem[i][1]),normalize(stem[i][2])))
	table_insert(content, format("%s %s",stem[i][1],stem[i][2]))
     end 
     table_insert(content,stem_type)
     return table_concat(content, " ")
 end
 return nil
end



local function make_glyph(valid_curves,char,cycles,tfm)
   -- Write the ttx

   if #cycles == 0 and #valid_curves==0 then
       print("no outlines ")
       --return
   end 
   local tfm = tfm
   local mflua_index = tointeger( char['index'] )  -- better a string or a number
   local index = mflua_index
   if mflua.ttx.userdata.charindex and mflua.ttx.userdata.charindex[index] then
      index = mflua.ttx.userdata.charindex[index]
   end
   local design_size=tonumber ( print_scaled(MFbuiltin.designsize()) ) --pt 
   local char_wd=tonumber( char['char_wd'] ) -- pt
   local char_ht=tonumber( char['char_ht'] ) -- pt
   local char_dp=tonumber( char['char_dp'] ) -- pt

   local cmap4_0  = scrt.cmap.cmap4_0
   local cmap4_3  = scrt.cmap.cmap4_3
   --local rawcmap0 = scrt.cmap.rawcmap0

   local glyphclassdef =  scrt.gdef.glyphclassdef


   local x_resolution = math_floor(0.5+tonumber( print_scaled(MFbuiltin.hppp()) )* 72.27)
   local y_resolution = math_floor(0.5+tonumber( print_scaled(MFbuiltin.vppp()) )* 72.27)
   assert(x_resolution==y_resolution, format('Error on _make_glyph x_res=%d and y_res=%d differ',x_resolution,y_resolution))

   local resolution = x_resolution 
   local emsize = ttx.emsize -- 1000, type 1, also known as em_unit: 1000 emsize = 1em
   local em_unit  = emsize  
   local em_unit_for_pixel = (72.27/design_size) * (emsize / resolution)
   local bp_for_pt = 72/72.27
   local char_wd_emunit = tonumber(format("%0.0f", (char_wd/design_size) *em_unit))
   local char_ht_emunit = tonumber(format("%0.0f", (char_ht/design_size) *em_unit))
   local char_dp_emunit = tonumber(format("%0.0f", (char_dp/design_size) *em_unit))
   local glyph_name  = tostring(char['charname'])

   if glyph_name and glyph_name ~= '' and not(index==0) then 
      table_insert(GlyphOrder,get_GlyphID(index,glyph_name))
   end

   local content = {}
   table_insert(content," ")
   if CFF2 and (abs(char_wd_emunit-scrt.font_space)>1e-4) then
      table_insert(content,format("%0.3f w",char_wd_emunit-scrt.font_space))  
   end
     

   table_insert(content,insert_stem(mflua_index,"hstem"))
   table_insert(content,insert_stem(mflua_index,"vstem"))
   --table_insert(content,"mask ?")  -- TODO

   local origin = {0,0}
   for i,_cycle in pairs(cycles) do 
      local cycle 
      local p,c1,c2,q,offset
      --cycle = reverse(_cycle)
      cycle = _cycle
      --  do we need a sort ?
      --cycle = sort(cycle)
      for i1,curve in ipairs(cycle) do 
	 p,c1,c2,q,offset=curve[1],curve[2],curve[3],curve[4],curve[5]
	 p =_eval_tonumber(p ,offset)
	 c1=_eval_tonumber(c1,offset)
	 c2=_eval_tonumber(c2,offset)
	 q =_eval_tonumber(q ,offset)
	 -- em_unit_for_pixel
	 p[1],p[2]   = p[1] *em_unit_for_pixel,  p[2]*em_unit_for_pixel
	 q[1],q[2]   = q[1] *em_unit_for_pixel,  q[2]*em_unit_for_pixel
	 c1[1],c1[2] = c1[1]*em_unit_for_pixel, c1[2]*em_unit_for_pixel
	 c2[1],c2[2] = c2[1]*em_unit_for_pixel, c2[2]*em_unit_for_pixel


	 p[1],p[2]   = toscaled(p[1]), toscaled(p[2])
	 q[1],q[2]   = toscaled(q[1]), toscaled(q[2])
	 c1[1],c1[2] = toscaled(c1[1]), toscaled(c1[2])
	 c2[1],c2[2] = toscaled(c2[1]), toscaled(c2[2])
 
	 
	 if CFF then
	    -- print(format("p1 %s path=(%0.14f,%0.14f) (%0.14f,%0.14f) (%0.14f,%0.14f) (%0.14f,%0.14f)",  index,
	    -- 					    p[1], p[2],
	    -- 					    c1[1], c1[2],
	    -- 					    c2[1], c2[2],
	    -- 					    q[1],q[2]))
	    -- print(format("p2 %s path=(%0.4f,%0.4f) (%0.4f,%0.4f) (%0.4f,%0.4f) (%0.4f,%0.4f)",  index,
	    -- 					    p[1], p[2],
	    -- 					    c1[1], c1[2],
	    -- 					    c2[1], c2[2],
	    -- 					    q[1],q[2]))
	    -- print(format("p3 %s path=(%s,%s) (%s,%s) (%s,%s) (%s,%s)",  index,
	    -- 		 normalize(p[1]), normalize(p[2]), 
	    -- 		 normalize(c1[1]), normalize(c1[2]), 
	    -- 		 normalize(c2[1]), normalize(c2[2]), 
	    -- 		 normalize(q[1]), normalize(q[2])))

	    if i1==1 then
	       --print(format("m %s %s %s rmoveto",index, normalize(p[1]-origin[1]),normalize(p[2]-origin[2])))
	       table_insert(content,format("%s %s rmoveto",normalize(p[1]-origin[1]),normalize(p[2]-origin[2])))
	       origin = {p[1],p[2]}
	    end
            if isvhline(p[2],c1[2],c2[2], q[2]) then     -- check  horizontal line
               --print(format("h %s %s hlineto", index, normalize(q[1]-p[1])))
	       table_insert(content,format("%s hlineto", normalize(q[1]-p[1])))
            elseif isvhline(p[1],c1[1],c2[1], q[1]) then -- check  vertical line
	       --print(format("v %s %s vlineto", index,normalize(q[2]-p[2])))
	       table_insert(content,format("%s vlineto", normalize(q[2]-p[2])))
            elseif isline(p[1],p[2],c1[1],c1[2],c2[1],c2[2],q[1],q[2]) then -- check a line 
               --print(format("r %s %s %s rlineto", index, normalize(q[1]-p[1]), normalize(q[2]-p[2])))
	       table_insert(content,format("%s %s rlineto", normalize(q[1]-p[1]), normalize(q[2]-p[2])))
            else 
	       local prec = 3;
               -- p[1] =  normalize_and_simplify(p[1],prec);p[2] =  normalize_and_simplify(p[2],prec);
               -- c1[1] =  normalize_and_simplify(c1[1],prec);c1[2] =  normalize_and_simplify(c1[2],prec);
               -- c2[1] =  normalize_and_simplify(c2[1],prec);c2[2] =  normalize_and_simplify(c2[2],prec);
               -- q[1] =  normalize_and_simplify(q[1],prec);q[2] =  normalize_and_simplify(q[2],prec);


	       table_insert(content,(format("%s %s %s %s %s %s rrcurveto", 
			    normalize_and_simplify(c1[1] -p[1],prec), normalize_and_simplify(c1[2]-p[2],prec),
			    normalize_and_simplify(c2[1]-c1[1],prec), normalize_and_simplify(c2[2]-c1[2],prec),
			    normalize_and_simplify(q[1] -c2[1],prec),  normalize_and_simplify(q[2]-c2[2],prec))))
	       
            end  

	 elseif CFF2 then -- 
	    -- reverse the key, because we reverse the cycle
	    local kp = table.concat({curve[4],curve[3],curve[2],curve[1]}) 
	    local deltas_width = {{0,0},{0,0}, {0,0},{0,0}, {0,0},{0,0}, {0,0},{0,0}}
	    local origin_deltas_width = {0,0}
	    local a, b, c, d, da, Da, db, Db, dc, Dc, dd, Dd
	    if mflua.userdata.deltas_width[glyph_name] then 
	       deltas_width  = mflua.userdata.deltas_width[glyph_name][kp] 
	       -- Original from MF, we have to reverse it; d..c..b..a is the key,
	       -- da and Da are the delta min and delta max,
	       -- where delta is (variated_instance - reference_instance)
	       -- delta are not affected by offset, but scaling are cannot be discarded
	       a, b, c, d, da, Da, db, Db, dc, Dc, dd, Dd = table.unpack(deltas_width)
	       deltas_width = {dd, Dd,dc, Dc,db, Db,da, Da}
	       for _,v in ipairs(deltas_width) do
	       	 v[1],v[2]=v[1]*em_unit_for_pixel, v[2]*em_unit_for_pixel
  	       end
	    end	
	    da, Da, db, Db, dc, Dc, dd, Dd = table.unpack(deltas_width)
	    -- One axis only, width 
	    -- Condensed Regular (still not Extended)
	    local blend
	    if i1==1 then
	       blend = format("%0.2f %0.2f  2 blend",da[1]-origin_deltas_width[1],da[2]-origin_deltas_width[2]) ; 
	       table_insert(content,format("%0.0f %0.0f %s rmoveto",p[1]-origin[1],p[2]-origin[2],blend))
	       origin = {p[1],p[2]}
	       origin_deltas_width ={da[1],da[2]}
	    end
	    blend = format("%0.2f %0.2f %0.2f %0.2f %0.2f %0.2f 6 blend",
	                                                                db[1]-da[1],db[2]-da[2],
	       	      	      		    	  	  	       dc[1]-db[1],dc[2]-db[2],
	       	      	      		    	  	  	       dd[1]-dc[1],dd[2]-dc[2]) 	

	    table_insert(content,format("%0.0f %0.0f %0.0f %0.0f %0.0f %0.0f %s rrcurveto",
	    				c1[1] -p[1], c1[2]-p[2],
	    				c2[1]-c1[1], c2[2]-c1[2],
	    				q[1] -c2[1],  q[2]-c2[2],blend))



	    -- if i1==1 then
	    --    blend = '0 0 2 blend' ; 
	    --    table_insert(content,format("%0.0f %0.0f %s rmoveto",p[1]-origin[1],p[2]-origin[2],blend))
	    --    origin = {p[1],p[2]}
	    -- end
	    -- --table_insert(content,format("%0.2f %0.2f %0.2f %0.2f %0.2f %0.2f rrcurveto",
	    -- if i1==1 then  blend = '0 0  0 0  0 0  6 blend' ; else  blend = '0 0  0 0  -10 0  6 blend'  end
	    -- table_insert(content,format("%0.0f %0.0f %0.0f %0.0f %0.0f %0.0f %s rrcurveto",
	    -- 				c1[1] -p[1], c1[2]-p[2],
	    -- 				c2[1]-c1[1], c2[2]-c1[2],
	    -- 				q[1] -c2[1],  q[2]-c2[2],blend))
	 end
      end
   end
   if ttx.cffver == 1 then  
      table_insert(content,"endchar\n")
   elseif ttx.cffver == 2 then 
      table_insert(content,"")
   end
   local CharString = new_element('CharString', {['name']=glyph_name})
   insert_child(CharString,table_concat(content,"\n"))

   insert_child(CharStrings,CharString)
   insert_child(cmap4_0,get_map(format("0x%x",index),glyph_name))
   --rawcmap0[format("0x%x",1+index%256)] = glyph_name
   insert_child(cmap4_3,get_map(format("0x%x",index),glyph_name))
   
   insert_child(glyphclassdef, get_ClassDef(glyph_name,"1"))
   update_char_lsb(glyph_name,char_wd_emunit,mflua_index)
   

   if ttx.cffver==2 then
      local psName = new_element('psName', {['name']=glyph_name})
      local Item = new_element('Item', {['index']=format("%d",index), ['value']="[-125]"})
      local Map  = new_element('Map', {['index']=format("%d",index), ['outer']="0", ['inner']="0"})
      local VarStore = get_children(HVAR,{'VarStore'}) 
      local VarData  = get_children(VarStore[1],{'VarData'}) 
      --local AdvWidthMap = get_children(HVAR,{'AdvWidthMap'}) 
      insert_child(VarData[1],Item)
      --insert_child(AdvWidthMap[1],Map)
      insert_child(extranames,psName)
        
   end
   
end -- make_glyph





ttx.make_glyph = make_glyph
ttx.makefont   = makefont
ttx.setup      = setup
ttx.userdata   = {}
ttx.cffver     = 1
ttx.eps        = 0.01
ttx.deps       = 0.01
return ttx
