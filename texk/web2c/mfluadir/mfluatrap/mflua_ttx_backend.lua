--
--
-- Experimental ttx back end
--
--


local format = string.format
local table_insert = table.insert
local table_concat = table.concat
local print_int    = mflua.MF.print_int
local abs = math.abs

local ttx	= {}
local xmlstream = {}

local scrt	= {} -- general table for local use

local function DUMP(t)
   if type(t) == 'table' then 
      print("table",#table)
      for k,v in pairs(t) do print(k,v) end
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
   elem[#elem+1]="\n"
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
local CFF, CFFFont, Private   -- handy to have        
local FFTM       
local GDEF  
local hmtx       

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




--for i=1,255 do table_insert(GlyphOrder,new_element('GlyphID', {['id']=i, ['name']="uni"..tostring(i)})) end
--local ttFont = dofile('ttx_sample.lua')
--savettfont(ttFont)


--------------------------------------------------------------------------------
--
-- Create & save xml font
--------------------------------------------------------------------------------

local function checkall(tfm)
   -- several checks here
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
   -- Default emsize is 1000 of CFF
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
   CFF        = new_element('CFF')
   -- FFTM       = new_element('FFTM') --FontForge TimeStamp table
   GDEF       = new_element('GDEF')
   hmtx       = new_element('hmtx')

   -- handy 
   panose     = new_element('panose')
   CFFFont    = new_element('CFFFont',{['name']="SourceCode"})
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
   table_insert(ttFont,CFF)
--   table_insert(ttFont,FFTM)
   table_insert(ttFont,GDEF)
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
   
   put_CFF('major',"1")
   put_CFF('minor',"0")
   insert_child(CFF,CFFFont)

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

   -- .notdef char
   local notdef = new_element('CharString', {['name']=".notdef"})
   insert_child(notdef,"107 endchar\n")
   insert_child_at_beginning(CharStrings, notdef)
   
   insert_child(CFFFont,CharStrings)

   --
   -- TODO: check GlobalSubrs!!
   --
   local globsubrs = new_element('GlobalSubrs')
   insert_child(CFF,globsubrs)
   if ttx.userdata.CFF.GlobalSubrs then
      for k,v in pairs(ttx.userdata.CFF.GlobalSubrs) do
	 insert_child(globsubrs,{'CharString', ['index']=k, v})
      end
   end
   
   --
   -- fontforge timestamp table.
   --
   -- put_FFTM('version', "1")
   -- put_FFTM('FFTimeStamp', "Fri Oct  7 21:32:10 2016")
   -- put_FFTM('sourceCreated', "Tue Apr  4 13:38:00 2017")
   -- put_FFTM('sourceModified', "Wed Apr  5 16:26:11 2017")


   local glyphclassdef = new_element('GlyphClassDef',{['Format']="2"})
   insert_child(GDEF,{'Version',['value']="0x00010000"})
   insert_child(GDEF,glyphclassdef)
   scrt.gdef               = scrt.gdef or {}
   scrt.gdef.glyphclassdef = glyphclassdef

   local LigCaretList = new_element('LigCaretList')
   insert_child(LigCaretList,new_element('Coverage',{['Format']="2"}))
   insert_child(GDEF,LigCaretList)

   insert_child(hmtx, get_mtx('.notdef',scrt.font_space,"0"))
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



local function make_glyph(valid_curves,char,cycles,tfm)
   -- Write the ttx
   --
   --  print('BEZ  MFbuiltin.hppp='..print_scaled(MFbuiltin.hppp()))
   --  print('BEZ  MFbuiltin.vppp='..print_scaled(MFbuiltin.vppp()))
   --  print('BEZ  MFbuiltin.designsize='..print_scaled(MFbuiltin.designsize()))
   local tfm = tfm
   local mflua_index = tonumber( char['index'] ) -- better a string or a number
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

   --local xheight =  0.458333 *  design_size -- must be read from tfm !!

   local x_resolution = math.floor(0.5+tonumber( print_scaled(MFbuiltin.hppp()) )* 72.27)
   local y_resolution = math.floor(0.5+tonumber( print_scaled(MFbuiltin.vppp()) )* 72.27)
   assert(x_resolution==y_resolution, format('Error on _make_glyph x_res=%d and y_res=%d differ',x_resolution,y_resolution))

   local resolution = x_resolution 
   local emsize = ttx.emsize -- 1000, type 1, also known as em_unit: 1000 emsize = 1em
   local em_unit  = emsize  

   local em_unit_for_pixel = (72.27/design_size) * (emsize / resolution)
   local bp_for_pt = 72/72.27
   local char_wd_emunit = (char_wd/design_size) *em_unit
   local char_ht_emunit = (char_ht/design_size) *em_unit
   local char_dp_emunit = (char_dp/design_size) *em_unit
    
   local glyph_name  = tostring(char['charname'])
   if glyph_name and glyph_name ~= '' and not(index==0) then 
      table_insert(GlyphOrder,get_GlyphID(index,glyph_name))
   end
   local content = {}
   if (abs(char_wd_emunit-scrt.font_space)>1e-4) then
      table_insert(content,format("%0.3f w",char_wd_emunit-scrt.font_space,print_int(1000*(tfm.font.space)))) 
   end
   --table_insert(content,"stem ?")  -- TODO
   --table_insert(content,"mask ?")  -- TODO
   local origin = {0,0}
   for i,cycle in pairs(cycles) do 
      local p,c1,c2,q,offset
      cycle = reverse(cycle)
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
	 if i1==1 then
	    table_insert(content,format("%0.2f %0.2f rmoveto",p[1]-origin[1],p[2]-origin[2]))
	    origin = {p[1],p[2]}
	 end
	 --table_insert(content,format("%0.2f %0.2f %0.2f %0.2f %0.2f %0.2f rrcurveto",
	 table_insert(content,format("%0.0f %0.0f %0.0f %0.0f %0.0f %0.0f rrcurveto",
				     c1[1] -p[1], c1[2]-p[2],
				     c2[1]-c1[1], c2[2]-c1[2],
				     q[1] -c2[1],  q[2]-c2[2],p[1],p[2],c1[1],c1[2],c2[1],c2[2],q[1],q[2]))
      end
   end
   table_insert(content,"endchar\n")
   local CharString = new_element('CharString', {['name']=glyph_name})
   insert_child(CharString,table_concat(content,"\n"))

   insert_child(CharStrings,CharString)
   insert_child(cmap4_0,get_map(format("0x%x",index),glyph_name))
   --rawcmap0[format("0x%x",1+index%256)] = glyph_name
   insert_child(cmap4_3,get_map(format("0x%x",index),glyph_name))
   
   insert_child(glyphclassdef, get_ClassDef(glyph_name,"1"))
   insert_child(hmtx, get_mtx(glyph_name,char_wd_emunit,"80"))
   
   
end -- make_glyph





ttx.make_glyph = make_glyph
ttx.makefont   = makefont
ttx.setup      = setup
ttx.userdata   = {}

return ttx
