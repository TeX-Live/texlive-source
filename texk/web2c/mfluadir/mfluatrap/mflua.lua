print("MFLua version: " .. mflua.MFbuiltin.mflua_version())
print("MFLua banner:  ".. mflua.MFbuiltin.mflua_banner())

local function PRINTDBG(s) print(tostring(s)) end
local function PRINTDBG(s) end
local function chMF(t,k) if t[k] then print(k.." already inserted") os.exit(1) end end 

PRINTDBG("mflua.lua")

--------------------------------------------------------------------------------
--
-- kpse 
--
--------------------------------------------------------------------------------
--    "new"
--    "set_program_name"
--    "init_prog"
--    "readable_file"
--    "find_file"
--    "expand_path"
--    "expand_var"
--    "expand_braces"
--    "var_value"
--    "show_path"
--    "lookup"
--    "version"
--    "default_texmfcnf"


--------------------------------------------------------------------------------
--
-- These are hardcoded into  mflua.MFbuiltin table:
--
--------------------------------------------------------------------------------
MFbuiltin = mflua.MFbuiltin or {}
-- function:	link
-- function:	info
-- function:	x_coord
-- function:	y_coord
-- function:	left_type
-- function:	right_type
-- function:	left_x
-- function:	left_y
-- function:	right_x
-- function:	right_y
-- function:	n_sin_cos
-- 
-- function:	LUAGLOBALGET_boundary_char
-- function:	LUAGLOBALGET_char_code
-- function:	LUAGLOBALGET_char_dp
-- function:	LUAGLOBALGET_char_dx
-- function:	LUAGLOBALGET_char_dy
-- function:	LUAGLOBALGET_char_ext
-- function:	LUAGLOBALGET_char_ht
-- function:	LUAGLOBALGET_char_ic
-- function:	LUAGLOBALGET_char_wd
-- function:	LUAGLOBALGET_cur_edges
-- function:	LUAGLOBALGET_cur_exp
-- function:	LUAGLOBALGET_cur_pen
-- function:	LUAGLOBALGET_designsize
-- function:	LUAGLOBALGET_fillin
-- function:	LUAGLOBALGET_granularity
-- function:	LUAGLOBALGET_hppp
-- function:	LUAGLOBALGET_mem_top
-- function:	LUAGLOBALGET_octant
-- function:	LUAGLOBALGET_turning_check
-- function:	LUAGLOBALGET_vppp
-- function:	LUAGLOBALGET_x_offset
-- function:	LUAGLOBALGET_y_offset

--
-- It's handy to have them as local 
--

local link       = MFbuiltin.link
local info       = MFbuiltin.info
local x_coord    = MFbuiltin.x_coord
local y_coord    = MFbuiltin.y_coord
local left_type  = MFbuiltin.left_type
local right_type = MFbuiltin.right_type
local left_x     = MFbuiltin.left_x
local left_y     = MFbuiltin.left_y
local right_x    = MFbuiltin.right_x
local right_y    = MFbuiltin.right_y
local n_sin_cos  = MFbuiltin.n_sin_cos



--------------------------------------------------------------------------------
-- Global table mflua.
-- Important subtable:
-- * mflua.MFbuiltin that stores the hardwired functions;
-- * mflua.MF that stores some constants from MetaFont, as also some functions
--   from PascalWEB rewritten in Lua (almost 1:1 translation).   
-- Use them with care !
--------------------------------------------------------------------------------

mflua = mflua or {}
mflua.MF = mflua.MF  or {}


--------------------------------------------------------------------------------
--
-- As in MetaFont:
--
--------------------------------------------------------------------------------

local min_quarterword = 0	        --{smallest allowable value in a |quarterword|}
local max_quarterword = 255 		--{largest allowable value in a |quarterword|}
local min_halfword = 0 	    		--{smallest allowable value in a |halfword|}
local max_halfword = 65535  		--{largest allowable value in a |halfword|}

local mem_min = 0                       -- {smallest index in the |mem| array, must not be less than |min_halfword|}

local quarter_unit = 2^14  		-- {$2^{14}$, represents 0.250000}
local half_unit = 2^15   		-- {$2^{15}$, represents 0.50000}
local three_quarter_unit = 3*(2^14) 	-- {$3\cdot2^{14}$, represents 0.75000}
local unity = 2^16 			-- {$2^{16}$, represents 1.00000}
local two = 2^17 			-- {$2^{17}$, represents 2.00000}
local three = 2^16+2^16  		-- {$2^{17}+2^{16}$, represents 3.00000}


chMF(mflua.MF,min_quarterword);mflua.MF.min_quarterword = min_quarterword
chMF(mflua.MF,max_quarterword);mflua.MF.max_quarterword = max_quarterword
chMF(mflua.MF,min_halfword);mflua.MF.min_halfword = min_halfword
chMF(mflua.MF,max_halfword);mflua.MF.max_halfword = max_halfword
chMF(mflua.MF,mem_min);mflua.MF.mem_min = mem_min
chMF(mflua.MF,quarter_unit);mflua.MF.quarter_unit = quarter_unit
chMF(mflua.MF,half_unit);mflua.MF.half_unit = half_unit
chMF(mflua.MF,three_quarter_unit);mflua.MF.three_quarter_unit = three_quarter_unit
chMF(mflua.MF,unity);mflua.MF.unity = unity
chMF(mflua.MF,two);mflua.MF.two = two
chMF(mflua.MF,three);mflua.MF.three = three



-- @ Given integers |x| and |y|, not both zero, the |n_arg| function
-- returns the |angle| whose tangent points in the direction $(x,y)$.
-- This subroutine first determines the correct octant, then solves the
-- problem for |0<=y<=x|, then converts the result appropriately to
-- return an answer in the range |-one_eighty_deg<=@t$\theta$@><=one_eighty_deg|.
-- (The answer is |+one_eighty_deg| if |y=0| and |x<0|, but an answer of
-- |-one_eighty_deg| is possible if, for example, |y=-1| and $x=-2^{30}$.)

-- The octants are represented in a ``Gray code,'' since that turns out
-- to be computationally simplest.

negate_x=1
negate_y=2
switch_x_and_y=4
first_octant=1
second_octant=first_octant+switch_x_and_y
third_octant=first_octant+switch_x_and_y+negate_x
fourth_octant=first_octant+negate_x
fifth_octant=first_octant+negate_x+negate_y
sixth_octant=first_octant+switch_x_and_y+negate_x+negate_y
seventh_octant=first_octant+switch_x_and_y+negate_y
eighth_octant=first_octant+negate_y


local negate_x=1
local negate_y=2
local switch_x_and_y=4
local first_octant=1
local second_octant=first_octant+switch_x_and_y
local third_octant=first_octant+switch_x_and_y+negate_x
local fourth_octant=first_octant+negate_x
local fifth_octant=first_octant+negate_x+negate_y
local sixth_octant=first_octant+switch_x_and_y+negate_x+negate_y
local seventh_octant=first_octant+switch_x_and_y+negate_y
local eighth_octant=first_octant+negate_y

chMF(mflua.MF,negate_x);mflua.MF.negate_x = negate_x
chMF(mflua.MF,negate_y);mflua.MF.negate_y = negate_y
chMF(mflua.MF,switch_x_and_y);mflua.MF.switch_x_and_y = switch_x_and_y
chMF(mflua.MF,first_octant);mflua.MF.first_octant = first_octant
chMF(mflua.MF,second_octant);mflua.MF.second_octant = second_octant
chMF(mflua.MF,third_octant);mflua.MF.third_octant = third_octant
chMF(mflua.MF,fourth_octant);mflua.MF.fourth_octant = fourth_octant
chMF(mflua.MF,fifth_octant);mflua.MF.fifth_octant = fifth_octant
chMF(mflua.MF,sixth_octant);mflua.MF.sixth_octant = sixth_octant
chMF(mflua.MF,seventh_octant);mflua.MF.seventh_octant = seventh_octant
chMF(mflua.MF,eighth_octant);mflua.MF.eighth_octant = eighth_octant


local octant_dir = {}
octant_dir[first_octant]="ENE-1"
octant_dir[second_octant]="NNE-2"
octant_dir[third_octant]="NNW-3"
octant_dir[fourth_octant]="WNW-4"
octant_dir[fifth_octant]="WSW-5"
octant_dir[sixth_octant]="SSW-6"
octant_dir[seventh_octant]="SSE-7"
octant_dir[eighth_octant]="ESE-8"

chMF(mflua.MF,octant_dir);mflua.MF.octant_dir = octant_dir

-- @ Finally we come to the last steps of |make_spec|, when boundary nodes
-- are inserted between cubics that move in different octants. The main
-- complication remaining arises from consecutive cubics whose octants
-- are not adjacent; we should insert more than one octant boundary
-- at such sharp turns, so that the envelope-forming routine will work.
-- For this purpose, conversion tables between numeric and Gray codes for
-- octants are desirable.
-- @<Glob...@>=
-- @!octant_number:array[first_octant..sixth_octant] of 1..8;
-- @!octant_code:array[1..8] of first_octant..sixth_octant;

local octant_code = {}
octant_code[1]=first_octant;
octant_code[2]=second_octant;
octant_code[3]=third_octant;
octant_code[4]=fourth_octant;
octant_code[5]=fifth_octant;
octant_code[6]=sixth_octant;
octant_code[7]=seventh_octant;
octant_code[8]=eighth_octant;
chMF(mflua.MF,octant_code);mflua.MF.octant_code = octant_code


local octant_number = {}
for k=1, 8 do octant_number[octant_code[k]]=k; end
chMF(mflua.MF,octant_number);mflua.MF.octant_number = octant_number


-- It is convenient to define a \.{WEB} macro |t_of_the_way| such that
-- |t_of_the_way(a)(b)| expands to |a-(a-b)*t|, i.e., to |t[a,b]|.
-- @d t_of_the_way_end(#)==#,t@=)@>
-- @d t_of_the_way(#)==#-take_fraction@=(@>#-t_of_the_way_end
--
-- TO implement !!
-- function t_of_the_way(a,b,t)
--  return take_fraction(a-b,t)
-- end 

local endpoint = 0 		-- {|left_type| at path beginning and |right_type| at path end} 
local knot_node_size = 7 	-- {number of words in a knot node} */

local left_curl=left_x 		-- {curl information when entering this knot}
local left_given=left_x 	-- {given direction when entering this knot}
local left_tension=left_y 	-- {tension information when entering this knot}
local right_curl=right_x 	-- {curl information when leaving this knot}
local right_given=right_x 	-- {given direction when leaving this knot}
local right_tension=right_y 	-- {tension information when leaving this knot}
local explicit =1 		-- {|left_type| or |right_type| when control points are known}
local given = 2 		-- {|left_type| or |right_type| when a direction is given}
local curl = 3 			-- {|left_type| or |right_type| when a curl is desired}
local open = 4 			-- {|left_type| or |right_type| when \MF\ should choose the direction}

local right_octant=right_x 	-- {the octant code before a transition}
local left_octant=left_x 	-- {the octant after a transition}
local right_transition=right_y 	-- {the type of transition}
local left_transition=left_y 	-- {ditto, either |axis| or |diagonal|}
local axis=0 			-- {a transition across the $x'$- or $y'$-axis}
local diagonal=1 		-- {a transition where $y'=\pm x'$}

local mem_top = MFbuiltin.mem_top()
local sentinel= mem_top 	--{end of sorted lists}
local null = mem_min		--  {the null pointer}
local knil=info			-- {inverse of the |link| field, in a doubly linked list}
local zero_w=4
local void=null+1

local zero_field=4096 		-- {amount added to coordinates to make them positive}


chMF(mflua.MF,endpoint);mflua.MF.endpoint = endpoint
chMF(mflua.MF,knot_node_size);mflua.MF.knot_node_size = knot_node_size

chMF(mflua.MF,left_curl);mflua.MF.left_curl = left_curl
chMF(mflua.MF,left_given);mflua.MF.left_given = left_given
chMF(mflua.MF,left_tension);mflua.MF.left_tension = left_tension
chMF(mflua.MF,right_curl);mflua.MF.right_curl = right_curl
chMF(mflua.MF,right_given);mflua.MF.right_given = right_given
chMF(mflua.MF,right_tension);mflua.MF.right_tension = right_tension
chMF(mflua.MF,explicit);mflua.MF.explicit = explicit
chMF(mflua.MF,given);mflua.MF.given = given
chMF(mflua.MF,curl);mflua.MF.curl = curl
chMF(mflua.MF,open);mflua.MF.open = open

chMF(mflua.MF,right_octant);mflua.MF.right_octant = right_octant
chMF(mflua.MF,left_octant);mflua.MF.left_octant = left_octant
chMF(mflua.MF,right_transition);mflua.MF.right_transition = right_transition
chMF(mflua.MF,left_transition);mflua.MF.left_transition = left_transition
chMF(mflua.MF,axis);mflua.MF.axis = axis
chMF(mflua.MF,diagonal);mflua.MF.diagonal = diagonal

chMF(mflua.MF,mem_top);mflua.MF.mem_top = mem_top
chMF(mflua.MF,sentinel);mflua.MF.sentinel = sentinel
chMF(mflua.MF,null);mflua.MF.null = null
chMF(mflua.MF,knil);mflua.MF.knil = knil
chMF(mflua.MF,zero_w);mflua.MF.zero_w = zero_w
chMF(mflua.MF,void);mflua.MF.void = void

chMF(mflua.MF,zero_field);mflua.MF.zero_field = zero_field


-- @d incr(#) == #:=#+1 {increase a variable by unity}
local function incr(p)
 return p+1
end
chMF(mflua.MF,incr);mflua.MF.incr = incr

-- decr(#) == #:=#-1 {decrease a variable by unity}
local function decr(p)
 return p-1
end 
chMF(mflua.MF,decr);mflua.MF.decr = decr

-- double(#) == #:=#+# {multiply a variable by two}
local function double(p)
 return 2*p
end
chMF(mflua.MF,double);mflua.MF.double = double



-- @ An array of digits in the range |0..9| is printed by |print_the_digs|.
-- @<Basic print...@>=
-- procedure print_the_digs(@!k:eight_bits);
--   {prints |dig[k-1]|$\,\ldots\,$|dig[0]|}
-- begin while k>0 do
--   begin decr(k); print_char("0"+dig[k]);
--   end;
-- end;
local function  print_the_digs(k,dig) 
  local res = ''
  while k > 0 do
   k=k-1
   res= res .. dig[k+1]
  end 
  return res 
end
chMF(mflua.MF,print_the_digs);mflua.MF.print_the_digs = print_the_digs


-- @<Basic print...@>=
-- procedure print_int(@!n:integer); {prints an integer in decimal form}
-- var k:0..23; {index to current digit; we assume that $|n|<10^{23}$}
-- @!m:integer; {used to negate |n| in possibly dangerous cases}
-- begin k:=0;
-- if n<0 then
--   begin print_char("-");
--   if n>-100000000 then negate(n)
--   else  begin m:=-1-n; n:=m div 10; m:=(m mod 10)+1; k:=1;
--     if m<10 then dig[0]:=m
--     else  begin dig[0]:=0; incr(n);
--       end;
--     end;
--   end;
-- repeat dig[k]:=n mod 10; n:=n div 10; incr(k);
-- until n=0;
-- print_the_digs(k);
-- end;
local function print_int(n) -- {prints an integer in decimal form}
 local  k  -- 0..23; {index to current digit; we assume that $|n|<10^{23}$}
 local m  --  {used to negate |n| in possibly dangerous cases}
 local dig = {}
 local done
 local res
 local sign=''
 k=0
 if n<0 then
  --begin print_char("-");
  sign='-'
  if n>-100000000 
   then 
    n=-n 
   else
    m=-1-n; n=math.floor(m/10) ; m=math.fmod(m,10)+1; k=1;  
    if m<10 
     then 
      dig[1]=m  
     else
      dig[1]=0; n=n+1
    end
   end
 end
 done=false
 while not done do
  dig[k+1]=math.fmod(n,10); n=math.floor(n/10); k=k+1;
  if n==0 then done=true end 
 end 
 res = print_the_digs(k,dig)
 return sign .. res 
end 
chMF(mflua.MF,print_int);mflua.MF.print_int = print_int



-- @<Basic printing...@>=
-- procedure print_scaled(@!s:scaled); {prints scaled real, rounded to five
--   digits}
-- var @!delta:scaled; {amount of allowable inaccuracy}
-- begin if s<0 then
--   begin print_char("-"); negate(s); {print the sign, if negative}
--   end;
-- print_int(s div unity); {print the integer part}
-- s:=10*(s mod unity)+5;
-- if s<>5 then
--   begin delta:=10; print_char(".");
--   repeat if delta>unity then
--     s:=s+@'100000-(delta div 2); {round the final digit '}
--   print_char("0"+(s div unity)); s:=10*(s mod unity); delta:=delta*10;
--   until s<=delta;
--   end;
-- end;
local function print_scaled(s)
 local delta
 local res = ''
 local done
 if s== nil then print("\nWarning: print_scale called with  nil argument."); return res end
 if s<0 then 
  res = '-'
  s=-s
 end
 res = res .. print_int(math.floor(s/unity)) -- {print the integer part}
 s=10*(math.fmod(s,unity))+5
 if s ~= 5   then
  delta=10; res = res .. '.'
  done = false
  while not done do 
   if delta>unity then
     s=s+half_unit-(math.floor(delta/2))  -- {round the final digit}
   end 
   res = res .. math.floor(s/unity); s=10*math.fmod(s,unity); delta=delta*10;
   if  s<=delta then done = true end
  end;
 end 
 return res
end
chMF(mflua.MF,print_scaled);mflua.MF.print_scaled = print_scaled



-- @<Basic printing...@>=
-- procedure print_two(@!x,@!y:scaled); {prints `|(x,y)|''}
-- begin print_char("("); print_scaled(x); print_char(","); print_scaled(y);
-- print_char(")");
-- end;
local function print_two(x,y) -- {prints `|(x,y)|'}
 local res 
 -- debug 
 res = '(' .. print_scaled(x) .. ',' .. print_scaled(y) .. ')'
 return res
end
chMF(mflua.MF,print_two);mflua.MF.print_two = print_two

-- procedure unskew(@!x,@!y:scaled;@!octant:small_number);
-- begin case octant of
-- first_octant: set_two(x+y)(y);
-- second_octant: set_two(y)(x+y);
-- third_octant: set_two(-y)(x+y);
-- fourth_octant: set_two(-x-y)(y);
-- fifth_octant: set_two(-x-y)(-y);
-- sixth_octant: set_two(-y)(-x-y);
-- seventh_octant: set_two(y)(-x-y);
-- eighth_octant: set_two(x+y)(-y);
-- end; {there are no other cases}
-- end;
local function unskew ( x , y , octant ) 
  local curx,cury
  if octant == 1 then
      curx = x + y ;
      cury = y ;
  elseif octant == 5 then
      curx = y ;
      cury = x + y ;
  elseif octant == 6 then
      curx = -y ;
      cury = x + y ;
  elseif octant == 2 then
      curx = -x - y ;
      cury = y ;
  elseif octant == 4 then
      curx = -x - y ;
      cury = -y ;
  elseif octant == 8 then
      curx = -y ;
      cury = -x - y ;
  elseif octant == 7 then
      curx = y ;
      cury = -x - y ;
  elseif octant == 3 then
      curx = x + y ;
      cury = -y ;
   end
  return curx,cury 
end
chMF(mflua.MF,unskew);mflua.MF.unskew = unskew

-- print_two_true(#)==unskew(#,octant); print_two(cur_x,cur_y)
local function print_two_true(x,y,octant)
 local cur_x,cur_y ,res
 cur_x,cur_y = unskew ( x , y , octant )
 res = print_two(cur_x,cur_y)
 return res
end
chMF(mflua.MF,print_two_true);mflua.MF.print_two_true = print_two_true
--
-- Debug only
--
-- function mflua_print_path(h)
--    print("mflua_print_path")
--    local p,q
--    local res 
--    local done
--    local done1
--    local f
--    done = false
--    done1 = false 
--    p = h
--    res = '' 
--    while not done do
--       q = link(p)
--       if (p==0) or (q==0) then
-- 	 res = "???"
-- 	 -- do something with res -- 
-- 	 return 0
--       end
--       res = res .. print_two(x_coord(p),y_coord(p)); -- print("res=",res)
--       if right_type(p) == endpoint then
-- 	 if left_type(p)== open then print("{open?}") end -- {can't happen}
-- 	 if (left_type(q) ~= endpoint) or (q ~= h) then q=null end -- {force an error}
-- 	 done1 = true --  goto done1;
--       elseif right_type(p) == explicit then 
-- 	 -- begin "@<Print control points between |p| and |q|, then |goto done1|@>"
-- 	 res = res .. "..controls " ..  print_two(right_x(p),right_y(p)) .. " and ";
-- 	 if left_type(q) ~= explicit then print("??")  -- {can't happen}
-- 	 else res = res .. print_two(left_x(q),left_y(q));
-- 	    done1 = true -- goto done1;
-- 	 end
-- 	 -- end "@<Print control points between |p| and |q|, then |goto done1|@>"
--       elseif right_type(p) == open then 
-- 	 -- begin "@<Print information for a curve that begins |open|@>" 
-- 	 if (left_type(p) ~= explicit) and (left_type(p)~=open) then
-- 	    res = res .. "{open?}" -- {can't happen}
-- 	 end  
-- 	 -- end "@<Print information for a curve that begins |open|@>" 
--       elseif (right_type(p) == curl) or (right_type(p) == given) then 
-- 	 -- @ A curl of 1 is shown explicitly, so that the user sees clearly that
-- 	 -- \MF's default curl is present.
-- 	 -- begin @<Print information for a curve that begins |curl|...@>=
-- 	 if left_type(p)==open then res = res .. "??" end --  {can't happen}
-- 	 if right_type(p)==curl then
-- 	    res = res .. "{curl ".. print_scaled(right_curl(p))
-- 	 else  n_sin_cos(right_given(p)); res = res .."{"
-- 	    res = res .. print_scaled(n_cos) .. "," ..  print_scaled(n_sin)
-- 	 end
-- 	 res = res .."}"
-- 	 -- end @<Print information for a curve that begins |curl|...@>=
--       else res = res .. "???" -- {can't happen}
--       end 
--       if not done1 then -- mimic label done 1
-- 	 if left_type(q)~=explicit then res = res .. "..control?" --   {can't happen}
-- 	 else if (right_tension(p) ~= unity) or (left_tension(q) ~= unity) then
-- 	       -- begin "@<Print tension between |p| and |q|@>;" 
-- 	       res = res .. "..tension "
-- 	       if right_tension(p)<0 then res = res .. "atleast" end 
-- 	       res = res .. print_scaled(math.abs(right_tension(p)))
-- 	       if right_tension(p) ~= left_tension(q) then
-- 		  res = res .. " and "
-- 		  if left_tension(q)<0 then res = res .. "atleast" end
-- 		  res = res .. print_scaled(math.abs(left_tension(q)))
-- 	       end
-- 	    end -- "@<Print tension between |p| and |q|@>;"
-- 	 end
--       end --- LABEL:  done1 
--       -- begin @<Print two dots...@>=
--       p = q
--       res = res .. " .." 
--       if left_type(p)==given then
-- 	 n_sin_cos(left_given(p)); res = res .. "{"
-- 	 res = res  .. print_scaled(n_cos); res = res .. ",";
-- 	 res = res .. print_scaled(n_sin); res = res .. "}";
--       else if left_type(p)==curl then
-- 	    res = res .. "{curl "; res = res .. print_scaled(left_curl(p)) .. "}";
-- 	 end;
--       end
--       -- end @<Print two dots...@>=
--       -- end "@<Print information for adjacent knots |p| and |q|@>"
--       if p == h then done =true end
--    end
--    if left_type(h) ~= endpoint then 
--       res = res .. "cycle" 
--    else 
--       res = res
--    end
--    -- do something with res --
--    return res 
-- end



-- n_max(#)==link(#+1) {maximum row number present, plus |zero_field|}
local function n_max(p)
 return link(p+1)
end 
chMF(mflua.MF,n_max);mflua.MF.n_max = n_max


-- sorted_loc(#)==#+1 {where the |sorted| link field resides}
local function sorted_loc(p)
 return p+1 
end
chMF(mflua.MF,sorted_loc);mflua.MF.sorted_loc = sorted_loc


-- @d sorted(#)==link(sorted_loc(#)) {beginning of the list of sorted edge weights}
local function sorted(p)
 return link(sorted_loc(p))
end
chMF(mflua.MF,sorted);mflua.MF.sorted = sorted


-- @d unsorted(#)==info(#+1) {beginning of the list of unsorted edge weights}
local function unsorted(p)
 return info(p+1)
end
chMF(mflua.MF,unsorted);mflua.MF.unsorted = unsorted

-- @d ho(#)==#-min_halfword
--   {to take a sixteen-bit item from a halfword}
-- See mf.ch 
-- ho(#) == #
local function ho(p) 
 return p
end
chMF(mflua.MF,ho);mflua.MF.ho = ho
--
-- @d m_offset(#)==info(#+3) {translation of $m$ data in edge-weight nodes}
--
function m_offset(p)
 return info(p+3)
end



-- @ @<Declare the procedure called |print_weight|@>=
-- procedure print_weight(@!q:pointer;@!x_off:integer);
-- var @!w,@!m:integer; {unpacked weight and coordinate}
-- @!d:integer; {temporary data register}
-- begin d:=ho(info(q)); w:=d mod 8; m:=(d div 8)-m_offset(cur_edges);
-- if file_offset>max_print_line-9 then print_nl(" ")
-- else print_char(" ");
-- print_int(m+x_off);
-- while w>zero_w do
--   begin print_char("+"); decr(w);
--   end;
-- while w<zero_w do
--   begin print_char("-"); incr(w);
--   end;
-- end;
local function print_weight(q,x_off)
 local w,m	-- {unpacked weight and coordinate}
 local d   	--{temporary data register}
 local cur_edges = MFbuiltin.cur_edges()
 local res = '' 
 local temp
 d=ho(info(q)); w=math.fmod(d,8); m=math.floor(d/8)-m_offset(cur_edges);
 res = tostring(print_int(m+x_off))
 while w>zero_w do
    --print(tostring(print_int(m+x_off)) .. " w=" .. w.. " " .. zero_w .. " " .. (w-zero_w))    
    res = res .. "+" ; w=decr(w); 
 end
 while w<zero_w do
    --print(tostring(print_int(m+x_off)) .. " w=" .. w.. " " .. zero_w .. " " .. (w-zero_w))    
    res = res .. "-" ; w=incr(w)
 end
 --print("res=" .. res .. "w=" .. math.fmod(d,8)-zero_w) 
 return res, math.fmod(d,8)-zero_w,tostring(print_int(m+x_off))
end
chMF(mflua.MF,print_weight);mflua.MF.print_weight = print_weight

-- Others utilities functions  
local function odd(n)
   return  math.fmod(n,2) == 1
end
chMF(mflua.MF,odd);mflua.MF.odd = odd

--------------------------------------------------------------------------------
--
-- Read-only callbacs aka Sensors
--
--------------------------------------------------------------------------------

local function begin_program()
 PRINTDBG("begin_program")
end

local function PRE_start_of_MF() 
 PRINTDBG("PRE_start_of_MF")
end

local function PRE_main_control() 
 PRINTDBG("PRE_main_control")
end

local function POST_main_control() 
 PRINTDBG("POST_main_control")
end

local function mflua_initialize() 
 PRINTDBG("mflua_initialize")
end

local function POST_final_cleanup() 
 PRINTDBG("POST_final_cleanup")
end

local function printpath(h,s,nuline)
 PRINTDBG("printpath")
 local p,q
 local res 
 local done
 local done1
 local f
 done = false
 done1 = false 
 p = h
 res = '' 
 while not done do
  q = link(p)
  if (p==0) or (q==0) then
     res = "???"
     -- do something with res -- 
     return 0
  end
  -- We can choose to follow the pascal-web way
  -- or to follow the C-web2c way
  -- begin "@<Print information for adjacent knots |p| and |q|@>"
  res = res .. print_two(x_coord(p),y_coord(p)); -- print("res=",res)
  if right_type(p) == endpoint then
    if left_type(p)== open then print("{open?}") end -- {can't happen}
    if (left_type(q) ~= endpoint) or (q ~= h) then q=null end -- {force an error}
    done1 = true --  goto done1;
  elseif right_type(p) == explicit then 
      -- begin "@<Print control points between |p| and |q|, then |goto done1|@>"
      res = res .. "..controls " ..  print_two(right_x(p),right_y(p)) .. " and ";
      if left_type(q) ~= explicit then print("??")  -- {can't happen}
      else res = res .. print_two(left_x(q),left_y(q));
      done1 = true -- goto done1;
      end
      -- end "@<Print control points between |p| and |q|, then |goto done1|@>"
  elseif right_type(p) == open then 
      -- begin "@<Print information for a curve that begins |open|@>" 
      if (left_type(p) ~= explicit) and (left_type(p)~=open) then
        res = res .. "{open?}" -- {can't happen}
      end  
      -- end "@<Print information for a curve that begins |open|@>" 
  elseif (right_type(p) == curl) or (right_type(p) == given) then 
      -- @ A curl of 1 is shown explicitly, so that the user sees clearly that
      -- \MF's default curl is present.
      -- begin @<Print information for a curve that begins |curl|...@>=
      if left_type(p)==open then res = res .. "??" end --  {can't happen}
      if right_type(p)==curl then
        res = res .. "{curl ".. print_scaled(right_curl(p))
      else  n_sin_cos(right_given(p)); res = res .."{"
       res = res .. print_scaled(n_cos) .. "," ..  print_scaled(n_sin)
      end
      res = res .."}"
      -- end @<Print information for a curve that begins |curl|...@>=
  else res = res .. "???" -- {can't happen}
  end 
  if not done1 then -- mimic label done 1
  if left_type(q)~=explicit then res = res .. "..control?" --   {can't happen}
  else if (right_tension(p) ~= unity) or (left_tension(q) ~= unity) then
    -- begin "@<Print tension between |p| and |q|@>;" 
    res = res .. "..tension "
    if right_tension(p)<0 then res = res .. "atleast" end 
    res = res .. print_scaled(math.abs(right_tension(p)))
    if right_tension(p) ~= left_tension(q) then
       res = res .. " and "
       if left_tension(q)<0 then res = res .. "atleast" end
       res = res .. print_scaled(math.abs(left_tension(q)))
    end
    end -- "@<Print tension between |p| and |q|@>;"
  end
  end --- LABEL:  done1 
  -- begin @<Print two dots...@>=
  p = q
  res = res .. " .." 
  if left_type(p)==given then
    n_sin_cos(left_given(p)); res = res .. "{"
    res = res  .. print_scaled(n_cos); res = res .. ",";
    res = res .. print_scaled(n_sin); res = res .. "}";
  else if left_type(p)==curl then
      res = res .. "{curl "; res = res .. print_scaled(left_curl(p)) .. "}";
     end;
  end
  -- end @<Print two dots...@>=
  -- end "@<Print information for adjacent knots |p| and |q|@>"
  if p == h then done =true end
 end
 if left_type(h) ~= endpoint then res = res .. "cycle" end
 -- do something with res --
 res = "%%Print path\n" ..  "drawoptions(withcolor black withpen pencircle scaled 1pt);\n" .. "draw " ..  res .. " ;\n" 
 --print(res)
 -- local index = (0+print_int(MFbuiltin.char_code())) +  (0+print_int(MFbuiltin.char_ext()))*256
 -- local char = mflua.chartable[index] or {}
 -- char['char_wd'] = print_scaled(MFbuiltin.char_wd()) 
 -- char['char_ht'] = print_scaled(MFbuiltin.char_ht()) 
 -- char['char_dp'] = print_scaled(MFbuiltin.char_dp()) 
 -- char['char_ic'] = print_scaled(MFbuiltin.char_ic())  
 -- char['res']     =   char['res']  or "" 
 -- char['res']     =   char['res']  .. res 
 -- mflua.chartable[index] = char 
end


-- @ The |print_edges| subroutine gives a symbolic rendition of an edge
-- structure, for use in `\&{show}\' commands. A rather terse output
-- format has been chosen since edge structures can grow quite large.

-- @<Declare subroutines for printing expressions@>=
-- @t\4@>@<Declare the procedure called |print_weight|@>@;@/
-- procedure print_edges(@!s:str_number;@!nuline:boolean;@!x_off,@!y_off:integer);
-- var @!p,@!q,@!r:pointer; {for list traversal}
-- @!n:integer; {row number}
-- begin
-- mflua_printedges(s,nuline,x_off,y_off);
-- print_diagnostic("Edge structure",s,nuline);
-- p:=knil(cur_edges); n:=n_max(cur_edges)-zero_field;
-- while p<>cur_edges do
--   begin q:=unsorted(p); r:=sorted(p);
--   if(q>void)or(r<>sentinel) then
--     begin print_nl("row "); print_int(n+y_off); print_char(":");
--     while q>void do
--       begin print_weight(q,x_off); q:=link(q);
--       end;
--     print(" |");
--     while r<>sentinel do
--       begin print_weight(r,x_off); r:=link(r);
--       end;
--     end;
--   p:=knil(p); decr(n);
--   end;
-- end_diagnostic(true);
-- end;
local function printedges(s,nuline,x_off,y_off)
   PRINTDBG("printedges")
   local p,q,r  --  for list traversal
   local n=0      --  row number
   local cur_edges = MFbuiltin.cur_edges()
   local res =''
   local y =  {} 
   local xr = {}  
   local xq = {} 
   local f, start_row, end_row ,start_row_1, end_row_1 
   local edge
   local w,w_integer,row_weight,xoff
   local chartable = mflua.chartable 
   local index 
   local char
   p = knil(cur_edges)
   n = n_max(cur_edges)-zero_field
   while p ~=  cur_edges do
      xq = {}; xr = {}
      q=unsorted(p); r=sorted(p)
      if(q>void)or(r~=sentinel) then
	 res = "mflua row " .. print_int(n+y_off) ..":"  
	 while (q>void)  do
	    w, w_integer,xoff = print_weight(q,x_off)
	    xq[#xq+1] = {xoff,w_integer}
	    res = res .. w; q=link(q);
	 end
	 res = res .. " |"
	 while r~=sentinel do
	    w,w_integer,xoff = print_weight(r,x_off) 
	    xr[#xr+1]= {xoff,w_integer}
	    res = res .. w .. ' '; r=link(r)
	 end
	 y[#y+1] = {print_int(n+y_off),xq,xr}
      end
      -- print(res)
      p=knil(p);n=decr(n);
   end 
   -- 
   -- local management of y, xq, xr 
   --
   index = (0+print_int(MFbuiltin.char_code())) +  (0+print_int(MFbuiltin.char_ext()))*256
   char = chartable[index] or {}
   --print("#xq=".. #xq)
   for i,v in ipairs(y) do 
      xq,xr = v[2],v[3]
      -- for j=1, #xq, 2 do end ??
      row_weight=0
      for j=1, #xr, 1 do 
	 local xb = xr[j][1]
	 local xwb = xr[j][2]
	 row_weight=row_weight+xwb
	 xr[j][3]=row_weight
	 --print(v[1],xr[j][1],xr[j][2],xr[j][3])
      end
   end
  char['edges'] =   char['edges'] or {}
  char['edges'][#char['edges']+1] = {y,x_off,y_off}

   char['pre_res']     =   char['pre_res']  or "" 
   for i,v in ipairs(y) do 
      xq,xr = v[2],v[3]
      -- for j=1, #xq, 2 do end ??
      row_weight=0
      char['pre_res']     =   char['pre_res']  .. "%% print edges " .. v[1] .. "\n"
      for j=1, #xr-1, 1 do 
	 local xb,xe = xr[j][1],xr[j+1][1]
	 local xsb,xse = xr[j][3],xr[j+1][3]
	 res = ""
	 if xsb>0 then
	    local color = {'0.7white','0.5white','0.4white'}
	    local col = color[xsb] or 'black'
	    res = res .. "drawoptions(withcolor " .. col .. " withpen pencircle scaled 0.1pt);\n" 
	    edge = string.format("fill (%s,%s) -- (%s,%s) -- (%s,%s+1) --  (%s,%s+1) --  cycle  shifted (-(%s),-(%s));\n",
	       xb,v[1],xe,v[1],xe,v[1],xb,v[1],x_off,y_off)
	    res = res .. edge 
	    --
	    res = res .. "drawoptions(withcolor black withpen pencircle scaled 0.1pt);\n" 
	    edge = string.format("draw (%s,%s) -- (%s,%s) -- (%s,%s+1) --  (%s,%s+1) --  cycle  shifted (-(%s),-(%s));\n",
	       xb,v[1],xe,v[1],xe,v[1],xb,v[1],x_off,y_off)
	    res = res .. edge 
	 end
	 -- print(v[1],xr[j][1],xr[j][2],xr[j][3])
	 char['pre_res']     =   char['pre_res']  .. res
      end
   end
end
mflua.MF.printedges = printedges




-- @ The |print_pen| subroutine illustrates these conventions by
-- reconstructing the vertices of a polygon from \MF\'s complicated 
-- internal offset representation.
-- @<Declare subroutines for printing expressions@>=

local function print_pen(p,s,nuline)
   local nothing_printed -- {:boolean has there been any action yet?}
   local k 		--1..8; {octant number}
   local h 		-- pointer; {offset list head}
   local m,n 		-- integer; {offset indices}
   local w,ww  		-- :pointer; {pointers that traverse the offset list}
   local res = ''
  -- begin print_diagnostic("Pen polygon",s,nuline);
   nothing_printed=true; -- print()
   for k=1,8 do
      local octant=octant_code[k]; h=p+octant; n=info(h); w=link(h);
      -- print("%% octant",octant_dir[octant],n,w)
      if not(odd(k)==true) then w=knil(w) end  -- {in even octants, start at $w_{n+1}$}
      for m=1, n+1 do
	 if odd(k)==true then ww=link(w)  else ww=knil(w)    end
	 -- print("%% ".. m .. "/" .. n+1 .. " w=" .. print_two_true(x_coord(w),y_coord(w),octant) .. " ww=" .. print_two_true(x_coord(ww),y_coord(ww),octant))
	 if (x_coord(ww)~=x_coord(w)) or (y_coord(ww)~=y_coord(w)) then
	    ---@<Print the unskewed and unrotated coordinates of node |ww|@>;
	    if nothing_printed then nothing_printed=false else  -- print(" .. ") 
	    end
	    -- print(print_two_true(x_coord(ww),y_coord(ww),octant))
	    res = res .. print_two_true(x_coord(ww),y_coord(ww),octant)
	 end
	 w=ww;
      end -- for m=1, n+1 do
   end -- for k=1,8 do
   if nothing_printed==true then
      w=link(p+first_octant); print(print_two(x_coord(w)+y_coord(w),y_coord(w)));
      res = res .. print_two(x_coord(w)+y_coord(w),y_coord(w))
   end;
   res = res  .. " .. cycle"; --end_diagnostic(true);
   return res 
end -- function
mflua.MF.print_pen = print_pen

mflua.offset_prep = mflua.offset_prep or  {}

local function _get_pen(p)
   local nothing_printed -- {:boolean has there been any action yet?}
   local k 		--1..8; {octant number}
   local h 		-- pointer; {offset list head}
   local m,n 		-- integer; {offset indices}
   local w,ww  		-- :pointer; {pointers that traverse the offset list}
   local res = {}
   nothing_printed=true; 
   for k=1,8 do
      local octant=octant_code[k]; h=p+octant; n=info(h); w=link(h);
      if not(odd(k)==true) then w=knil(w) end  -- {in even octants, start at $w_{n+1}$}
      for m=1, n+1 do
	 if odd(k)==true then ww=link(w)  else ww=knil(w)    end
	 if (x_coord(ww)~=x_coord(w)) or (y_coord(ww)~=y_coord(w)) then
	    ---@<Print the unskewed and unrotated coordinates of node |ww|@>;
	    if nothing_printed then nothing_printed=false else  -- print(" .. ") 
	    end
	    -- print(print_two_true(x_coord(ww),y_coord(ww),octant))
	    res[#res+1] = print_two_true(x_coord(ww),y_coord(ww),octant)
	 end
	 w=ww;
      end -- for m=1, n+1 do
   end -- for k=1,8 do
   if nothing_printed==true then
      w=link(p+first_octant); 
      res[#res+1] = print_two(x_coord(w)+y_coord(w),y_coord(w))
   end;
   -- print(" .. cycle"); --end_diagnostic(true);
   return res 
end
mflua.offset_prep._get_pen = _get_pen

local function _get_offset_coords(p,octant)
  local nothing_printed -- {:boolean has there been any action yet?}
  local k 		--1..8; {octant number}
  local h 		-- pointer; {offset list head}
  local m,n 		-- integer; {offset indices}
  local w,ww  		-- :pointer; {pointers that traverse the offset list}
  local res ={}  
  nothing_printed=true; --print()
  k=octant_number[octant]; 
  h=p+octant; n=info(h); w=link(h);
  if not(odd(k)==true) then w=knil(w) end  -- {in even octants, start at $w_{n+1}$}
  for m=1, n+1 do
   if odd(k)==true then ww=link(w)  else ww=knil(w)    end
   res[m] = print_two_true(x_coord(w),y_coord(w),octant)
   w=ww;
  end -- for m=1, n+1 do
  return res 
end -- function
mflua.offset_prep._get_offset_coords = _get_offset_coords


-- @ Given a pointer |c| to a nonempty list of cubics,
-- and a pointer~|h| to the header information of a pen polygon segment,
-- the |offset_prep| routine changes the list into cubics that are
-- associated with particular pen offsets. Namely, the cubic between |p|
-- and~|q| should be associated with the |k|th offset when |right_type(p)=k|.

-- List |c| is actually part of a cycle spec, so it terminates at the
-- first node whose |right_type| is |endpoint|. The cubics all have
-- monotone-nondecreasing $x(t)$ and $y(t)$.

mflua.do_add_to = mflua.do_add_to or {} 
mflua.do_add_to.bezier_octant          = {}
mflua.do_add_to.bezier_octant_envelope = {}
mflua.do_add_to.bezier_octant_I        = {} 
mflua.do_add_to.bezier_octant_contour  = {} 


local function print_specification(c,h)
   local p,q,n,nh
   local octant
   local cur_spec
   local res,res1 = "",""
   local offsets = {}
   local cubic,cubics ={},{}
   local f   
   local first_point, first_point_offset

   local bezier,beziers ={},{}
   local offset_list = {}
   local path_list ={}
   local bezier_octant
   local pen_key = ''
   local temp1 = mflua.print_specification.temp1

   cur_spec=c
   p=cur_spec 
   --n=info(h) 
   --lh=link(h)	--{now |lh| points to $w_0$}
   octant = left_octant(p)
   offsets =  mflua.offset_prep._get_offset_coords(MFbuiltin.cur_pen(),octant)
   
   -- for l=1,#offsets do print("SPEC " .. offsets[l] )end

   cubics['offsets'] = offsets
   cubics['octant_number'] = octant_number[octant]

   beziers['offsets'] = offsets
   beziers['octant_number'] = octant_number[octant]
   beziers['pen'] = mflua.offset_prep._get_pen(MFbuiltin.cur_pen())  
   
   for i,v in ipairs(beziers['pen']) do  
     --print(  "BEZ pen=",i,v)  
     pen_key = pen_key..v
   end
   --if not(mflua.pen[pen_key] == nil) then
      --table.foreach(mflua.pen[pen_key],print)
   --end   
   --
   --res = res .. "%% cur_pen " .. tostring(MFbuiltin.cur_pen()) .."\n"
   --res = res .. string.format("%%%% current octant %s, octant number %s, offset %s\n",octant_dir[MFbuiltin.octant()],octant_number[octant],print_int(n))
   --res = res .. "pair offset[];\n"
   --for i,v in ipairs(offsets) do
   --  res = res .. string.format("offset%s:=%s;\n",i-1,v)
   --end
   --res = res .. "pair OffSet; OffSet:=offset"..print_int(n) ..";\n" 
   --res = res .. "path p; p:= " .. print_two_true(x_coord(p),y_coord(p),octant) .. "\n"
   --cubic['p'] = print_two_true(x_coord(p),y_coord(p),octant)
   first_point = print_two_true(x_coord(p),y_coord(p),octant)
   first_point_offset = print_int(right_type(p))
   --
   --print(res);
   local end_loop_1 = false
   while end_loop_1 == false do
     local end_loop_2 = false
     while end_loop_2 == false do
     	q=link(p);
        if right_type(p)==endpoint then 
	   end_loop_2=true 
	else    
	   cubic['p'] = print_two_true(x_coord(p),y_coord(p),octant);
           cubic['control1'] = print_two_true(right_x(p),right_y(p),octant)
           cubic['control2'] = print_two_true(left_x(q),left_y(q),octant)
	   cubic['q'] =  print_two_true(x_coord(q),y_coord(q),octant)
	   cubic['offset'] =  print_int(right_type(p))
	   cubic['segment'] =  print_int(left_type(q)-1)
	   cubics[#cubics+1] = cubic
	   cubic = {}
	   bezier['p'] = print_two_true(x_coord(p),y_coord(p),octant);
           bezier['control1'] = print_two_true(right_x(p),right_y(p),octant)
           bezier['control2'] = print_two_true(left_x(q),left_y(q),octant)
	   bezier['q'] =  print_two_true(x_coord(q),y_coord(q),octant)
	   bezier['offset'] =  print_int(right_type(p))
	   bezier['segment'] =  print_int(left_type(q)-1)
	   beziers[#beziers+1] = bezier
	   bezier = {}
	   p=q
       end 
     end 
     -- not_found label 
     if q==cur_spec then 
       end_loop_1=true 
     else
       p=q; octant=left_octant(p); -- print("% entering octant `");
     end
     --  We don't want all the octans of the cubic
     --  only the pieces of the current octant
     end_loop_1 = not(MFbuiltin.octant() == octant)
   end
   if #cubics == 0 then
     cubics['single_point'] = first_point
     cubics['single_point_offset'] = first_point_offset
   end
   -- done label: 
   -- We can now use the results
   --
   -- No curves stored
   if #beziers == 0 then
     beziers['single_point'] = first_point
     beziers['single_point_offset'] = first_point_offset
   end
   if #beziers['offsets'] == 1 then 
      offset_list[#offset_list+1] = {0,beziers['offsets'][1]}
      offset_list[#offset_list+1] = {1,beziers['offsets'][1]}
   else
      for i,v in ipairs(beziers['offsets']) do
	 if odd(beziers['octant_number']) == true then  
	    offset_list[#offset_list+1] = {(i-1),v}
	 else
	    offset_list[#offset_list+1] = {#beziers['offsets']-i+1,v}
	 end
     end
   end
   beziers['offset_list']=offset_list
   beziers['path_list'] = {}
   if #beziers == 0 then
      path_list['p'] = beziers['single_point']
      if odd(beziers['octant_number']) == true then  
	 path_list['offset'] = beziers['single_point_offset']
      else
	 path_list['offset'] = #beziers['offsets']-beziers['single_point_offset']
      end      
      beziers['path_list'][#beziers['path_list']+1] = path_list
      path_list={}
   else   
      for i,v in ipairs(beziers) do
	 bezier = v 
	 path_list['p'] = bezier['p'] 
	 path_list['control1'] = bezier['control1'] 
	 path_list['control2'] = bezier['control2'] 
	 path_list['q'] = bezier['q'] 
	 path_list['offset'] = bezier['offset']
	 beziers['path_list'][#beziers['path_list']+1] = path_list
	 path_list={}
      end
   end
   bezier_octant =mflua.do_add_to.bezier_octant 
   bezier_octant[#bezier_octant+1] = beziers
   res = ""
   res = res .. "%% cur_pen " .. tostring(MFbuiltin.cur_pen()) .."\n"
   --res = res .. string.format("%%%% current octant %s, offset %s\n",octant_dir[MFbuiltin.octant()],print_int(n))
   res = res .. string.format("%%%% current octant %s\n",octant_dir[MFbuiltin.octant()])
   res = res .. "pair offset[];\n"
   if #cubics['offsets'] == 1 then 
      res = res .."%% Only one offset\n"
      res = res ..string.format("offset%s:=%s;\n",0,cubics['offsets'][1])
      res = res ..string.format("offset%s:=%s;\n",1,cubics['offsets'][1])
   else 
      for i,v in ipairs(cubics['offsets']) do
	 if odd(cubics['octant_number']) == true then  
            res = res .. string.format("offset%s:=%s;\n",(i-1),v)
	 else
            res = res .. string.format("offset%s:=%s;\n",#cubics['offsets']-i+1,v)
	 end
     end
   end
   res = res .. "%% cubics['octant_number'])=" .. cubics['octant_number'] .. "\n"
   res = res .. "%% #cubics=" .. #cubics .. "\n"
   if #cubics == 0 then
     res = res .. "path p; p:=" .. cubics['single_point'] .. ";\n"
     res = res .. "drawoptions(withcolor red withpen pencircle scaled 0.1pt);\n"
     temp1 = temp1 +1             
     if odd(cubics['octant_number']) == true then  
     	   res = res .. "draw p shifted offset" .. cubics['single_point_offset']  ..  ";\n"
           res = res .. string.format("pickup pencircle scaled 0.2pt;drawdot(%s) shifted offset%s withcolor 0.75white;label(\"%s\",%s+(-0.5,-0.5)) shifted offset%s;\n",
 	      	 		       	cubics['single_point'],cubics['single_point_offset'],temp1,cubics['single_point'],cubics['single_point_offset'])
     else
            res = res .. "draw p shifted offset" .. #cubics['offsets']-cubics['single_point_offset']  ..  ";\n"
            res = res .. string.format("pickup pencircle scaled 0.2pt;drawdot(%s) shifted offset%s withcolor 0.75white;label(\"%s\",%s+(-0.5,-0.5)) shifted offset%s;\n",
	       	cubics['single_point'],#cubics['offsets']-cubics['single_point_offset'],temp1,cubics['single_point'],#cubics['offsets']-cubics['single_point_offset'])
     end      
   end
   -- if #cubics == 0 then this for loop is never executed
   for i,v in ipairs(cubics) do
     cubic = v 
     res = res .. "path p; p:= " .. cubic['p'] .."\n"
     res = res .. " .. controls " .. cubic['control1'] .." and " .. cubic['control2']
     res = res .. " .. " ..  cubic['q'] .."\n ;\n"
     res = res .. 'label("'.. octant_dir[MFbuiltin.octant()] ..'"' .. ",0.5[" .. cubic['p'] .. "," .. cubic['q']  .."]) shifted offset" .. cubic['offset'] .. ";\n"
     res = res .. "drawoptions(withcolor black withpen pencircle scaled 0.2pt);\n"
     res = res .. "draw p shifted offset" .. cubic['offset'] ..  ";\n"
     temp1 = temp1 +1
     res = res .. string.format("pickup pencircle scaled 0.2pt;drawdot(%s) shifted offset%s withcolor 0.75white;label(\"%s\",%s+(-0.5,-0.5)) shifted offset%s;\n",
 	      	 		       	cubic['p'],cubic['offset'],temp1,cubic['p'],cubic['offset'])
     temp1 = temp1 +1
     res = res .. string.format("pickup pencircle scaled 0.2pt;drawdot(%s) shifted offset%s withcolor 0.75white;label(\"%s\",%s+(0.5,0.5)) shifted offset%s;\n",
 	      	 		       	cubic['q'],cubic['offset'],temp1,cubic['q'],cubic['offset'])

   end
   mflua.print_specification.temp1 = temp1 
   res = res .. string.format("%%%%mflua.print_specification.temp1 = %s\n" ,mflua.print_specification.temp1)
   --print("\n%%POST START\n".. res .. "%%POST END\n")
   -- f = io.open("envelope.tex",'a')
   -- f = mflua.print_specification.outfile1
   -- f:write("\n%%POST START\n".. res .. "\n%%POST END\n")
   --f:close()
   return res
end
mflua.MF.print_specification = print_specification 



local function PRE_offset_prep(c,h)
  PRINTDBG("PRE_offset_prep")
  -- local p = c
  -- print("\nBEZ TEST".. print_int(right_type(p)))
  -- print ("BEZ TEST".. print_two(x_coord(p),y_coord(p)))
  -- print ("BEZ TEST".. print_two(right_x(p),right_y(p)))
  -- p = link(p)
  -- print ("BEZ TEST".. print_two(left_x(p),left_y(p)))
end

local function POST_offset_prep(c,h)
  PRINTDBG("POST_offset_prep")
  -- print("\nPOST print pen"); mflua.MF.print_pen(MFbuiltin.cur_pen(),"" , "")  
  -- print("\nPOST print specification") 
  --res = print_pen(MFbuiltin.cur_pen(),"" , "")  
  --print(" PRINT PEN " .. res )
  mflua.MF.print_specification(c,h)
end


mflua.do_add_to = mflua.do_add_to or {}

local function _get_cycle(h)
   local p,q
   local res = ''
   local done
   local done1
   local f
   local cycle = {}
   done = false
   done1 = false 
   p = h
   while not done do
      q = link(p)
      if (p==0) or (q==0) then
	 return '???'
      end
      cycle[#cycle+1] = {tonumber(print_scaled(x_coord(p))), tonumber(print_scaled(y_coord(p)))} -- p
      --res = res .. print_two(x_coord(p),y_coord(p)); -- print("res=",res)
      if right_type(p) == endpoint then
	 if left_type(p)== open then return "{open?}" end -- {can't happen}
	 if (left_type(q) ~= endpoint) or (q ~= h) then q=null end -- {force an error}
	 done1 = true --  goto done1;
      elseif right_type(p) == explicit then 
	 -- begin "@<Print control points between |p| and |q|, then |goto done1|@>"
	 --res = res .. "..controls " ..  print_two(right_x(p),right_y(p)) .. " and ";
	 cycle[#cycle+1] = {tonumber(print_scaled(right_x(p))), tonumber(print_scaled(right_y(p)))} -- c1
	 if left_type(q) ~= explicit then return "??"  -- {can't happen}
	 else
	    --res = res .. print_two(left_x(q),left_y(q));
	    cycle[#cycle+1] = {tonumber(print_scaled(left_x(q))), tonumber(print_scaled(left_y(q)))} -- c2
	    done1 = true -- goto done1;
	 end
	 -- end "@<Print control points between |p| and |q|, then |goto done1|@>"
      elseif right_type(p) == open then 
	 -- begin "@<Print information for a curve that begins |open|@>" 
	 if (left_type(p) ~= explicit) and (left_type(p)~=open) then
	    return "{open?}" -- {can't happen}
	 end  
	 -- end "@<Print information for a curve that begins |open|@>" 
      elseif (right_type(p) == curl) or (right_type(p) == given) then 
	 -- @ A curl of 1 is shown explicitly, so that the user sees clearly that
	 -- \MF's default curl is present.
	 -- begin @<Print information for a curve that begins |curl|...@>=
	 if left_type(p)==open then res = res .. "??" end --  {can't happen}
	 if right_type(p)==curl then
	    res = res .. "{curl ".. print_scaled(right_curl(p))
	 else  n_sin_cos(right_given(p)); res = res .."{"
	    res = res .. print_scaled(n_cos) .. "," ..  print_scaled(n_sin)
	 end
	 res = res .."}"
	 -- end @<Print information for a curve that begins |curl|...@>=
      else return "???" -- {can't happen}
      end 
      if not done1 then -- mimic label done 1
	 if left_type(q)~=explicit then return  "..control?" --   {can't happen}
	 else if (right_tension(p) ~= unity) or (left_tension(q) ~= unity) then
	       -- begin "@<Print tension between |p| and |q|@>;" 
	       res = res .. "..tension "
	       if right_tension(p)<0 then res = res .. "atleast" end 
	       res = res .. print_scaled(math.abs(right_tension(p)))
	       if right_tension(p) ~= left_tension(q) then
		  res = res .. " and "
		  if left_tension(q)<0 then res = res .. "atleast" end
		  res = res .. print_scaled(math.abs(left_tension(q)))
	       end
	    end -- "@<Print tension between |p| and |q|@>;"
	 end
      end --- LABEL:  done1 
      -- begin @<Print two dots...@>=
      p = q
      --res = res .. " .." 
      if left_type(p)==given then
	 n_sin_cos(left_given(p)); res = res .. "{"
	 res = res  .. print_scaled(n_cos); res = res .. ",";
	 res = res .. print_scaled(n_sin); res = res .. "}";
      else if left_type(p)==curl then
	    res = res .. "{curl "; res = res .. print_scaled(left_curl(p)) .. "}";
	 end;
      end
      -- end @<Print two dots...@>=
      -- end "@<Print information for adjacent knots |p| and |q|@>"
      if p == h then done =true end
   end
   if left_type(h) ~= endpoint then 
      res = res .. "cycle" 
   end
   -- do something with res --
   return res ,cycle
end
mflua.do_add_to._get_cycle = _get_cycle


-- @p procedure print_spec(@!s:str_number);
-- label not_found,done;
-- var @!p,@!q:pointer; {for list traversal}
-- @!octant:small_number; {the current octant code}
-- begin print_diagnostic("Cycle spec",s,true);
-- @.Cycle spec at line...@>
-- p:=cur_spec; octant:=left_octant(p); print_ln;
-- print_two_true(x_coord(cur_spec),y_coord(cur_spec));
-- print(" % beginning in octant `");
-- loop@+  begin print(octant_dir[octant]); print_char("'");
--   loop@+  begin q:=link(p);
--     if right_type(p)=endpoint then goto not_found;
--     @<Print the cubic between |p| and |q|@>;
--     p:=q;
--     end;
-- not_found: if q=cur_spec then goto done;
--   p:=q; octant:=left_octant(p); print_nl("% entering octant `");
--   end;
-- @.entering the nth octant@>
-- done: print_nl(" & cycle"); end_diagnostic(true);
-- end;
local function _print_spec(cur_spec)
 --print("\n.....Hello world from _print_spec!.....")
 local p,q 
 local octant
 --local res = '' 
 local knot = {} 
 local knots = {} 
 -- local res = {}
 local endloop1 = false
 local endloop2 = false

 p=cur_spec; octant=left_octant(p); --print()
 -- res = res .. print_two_true(x_coord(cur_spec),y_coord(cur_spec),octant)
 knot[#knot+1] = print_two_true(x_coord(cur_spec),y_coord(cur_spec),octant)
 while (endloop1 == false)  do
    -- print('%%' .. octant_dir[octant])
    endloop2 = false
    while (endloop2 == false)  do
       q = link(p)
       if right_type(p)==endpoint then 
	  endloop2 = true -- goto not_found;
       else
	  -- print(' @<Print the cubic between |p| and |q|@>;')
	  -- c1
	  knot[#knot+1] = print_two_true(right_x(p),right_y(p),octant)
	  -- c2
	  knot[#knot+1] = print_two_true(left_x(q),left_y(q),octant)
	  -- q
	  knot[#knot+1] = print_two_true(x_coord(q),y_coord(q),octant)
	  -- segment
	  knot[#knot+1] = print_int(left_type(q)-1)
	  knots[#knots+1] = knot
	  knot = {} 
	  -- res = res .." ..controls "
	  -- res = res .. print_two_true(right_x(p),right_y(p),octant)
	  -- res = res .." and "
	  -- res = res ..print_two_true(left_x(q),left_y(q),octant)
	  -- res = res .. "\n .."
	  -- res = res .. print_two_true(x_coord(q),y_coord(q),octant)
	  -- res = res .." % segment " ..print_int(left_type(q)-1) .. "\n";
	  p=q;
	  knot[#knot+1] = print_two_true(x_coord(p),y_coord(p),octant)
       end
    end -- endloop2
    -- not_found
    if q == cur_spec then 
       endloop1 = true 
    else
       p=q; octant=left_octant(p) --  print("% entering octant `");
    end
 end -- endloop1
 --done: 
 -- print(" & cycle") ; end_diagnostic(true);
 -- print("%BEZ TEST\ndraw "..res .. ";\n")
 -- table.foreach(knots,function (k) table.foreach(knots[k],print) end)
 return knots
end
mflua.do_add_to._print_spec = _print_spec


local function _store_current_envelope()
   local bezier_octant_envelope = mflua.do_add_to.bezier_octant_envelope 
   local bezier_octant = mflua.do_add_to.bezier_octant 
   if (#bezier_octant_envelope == 0) then
      local _t = {} 
      for i,v in ipairs(bezier_octant) do _t[i] = v end
      bezier_octant_envelope[1] = _t
   else
      local _cnt=0 
      for i,v in ipairs(bezier_octant_envelope) do _cnt=_cnt+#v end
      local _t = {} 
      for i,v in ipairs(bezier_octant) do if i>_cnt then _t[#_t+1] = v end end
      bezier_octant_envelope[#bezier_octant_envelope+1] = _t
   end
   mflua.do_add_to.bezier_octant_envelope = bezier_octant_envelope 
   return 0
end
mflua.do_add_to._store_current_envelope = _store_current_envelope


local function _postprocessing()
   local bezier_octant
   local beziers,offsets

   local path_list
   local prev_point 
   local path_cnt
   local res = "%% postprocessing envelope\n"
   local f
   local chartable = mflua.chartable 
   local index 
   local char

   index = (0+print_int(MFbuiltin.char_code())) +  (0+print_int(MFbuiltin.char_ext()))*256
   --print("CHAR " .. index)
   --print("%% postprocessing envelope "..index.. ' ' .. #chartable)
   
   res = res .. "path p[];\n"

   --  The last part of envelope added
   bezier_octant = mflua.do_add_to.bezier_octant_envelope[#mflua.do_add_to.bezier_octant_envelope] 

   path_cnt = 1
   for i,v in ipairs(bezier_octant) do
      beziers = v
      offsets = beziers['offsets']
      path_list = beziers['path_list'] 
      local offset_list = beziers['offset_list']
      for i,path in ipairs(path_list) do
	 local shifted 
         local p,c1,c2,q,offset = 
	    path['p'],path['control1'],path['control2'],path['q'],path['offset']  	 
	 for i,v in ipairs(offset_list) do 
	    if v[1] == (0+offset) then 
	       shifted = v[2] 
	       break 
	    end 
	 end 
	 if (q == nil) then
	    res = res .. string.format("p%d:=(%s) shifted %s;%% shifted 1\n",
				       path_cnt,p,shifted)
	 else
	    res = res .. string.format("p%d:=(%s .. controls %s and %s .. %s) shifted %s;%% shifted 2\n",
				       path_cnt,p,c1,c2,q,shifted)
	 end
	 path_cnt = path_cnt +1
      end	 
   end
   res = res .. "%% path_cnt=" .. path_cnt .. " char_code=" .. print_int(MFbuiltin.char_code()) .. " char_ext=" .. print_int(MFbuiltin.char_ext()) 
   res = res ..  " char_wd=" .. print_scaled(MFbuiltin.char_wd()) 
   res = res ..  " char_ht=" .. print_scaled(MFbuiltin.char_ht()) 
   res = res ..  " char_dp=" .. print_scaled(MFbuiltin.char_dp()) 
   res = res ..  " char_ic=" .. print_scaled(MFbuiltin.char_ic())  
   res = res ..  " \n"

   res = res .. "drawoptions(withcolor (" .. math.random().."," .. math.random()..",".. math.random()..  ") withpen pencircle scaled 0.4pt);\n"
   res = res .. "draw p1"
   for i=2,path_cnt-1 do 
      res = res .. string.format(" --  p%d",i)
   end 
   res = res .. " --cycle;\n"

   index = (0+print_int(MFbuiltin.char_code())) +  (0+print_int(MFbuiltin.char_ext()))*256
   char = chartable[index] or {}
   char['char_wd'] = print_scaled(MFbuiltin.char_wd()) 
   char['char_ht'] = print_scaled(MFbuiltin.char_ht()) 
   char['char_dp'] = print_scaled(MFbuiltin.char_dp()) 
   char['char_ic'] = print_scaled(MFbuiltin.char_ic())  
   char['envelope'] = char['envelope'] or {}
   char['envelope'][#char['envelope']+1] = bezier_octant
   char['res']     =   char['res']  or "" 
   char['res']     =   char['res']  .. res 
   char['index'] = index
   chartable[index] = char 
end
mflua.do_add_to._postprocessing = _postprocessing


local function _store_current_contour()
   local bezier_octant_contour = mflua.do_add_to.bezier_octant_contour
   local bezier_octant_I = mflua.do_add_to.bezier_octant_I 
   if (#bezier_octant_contour == 0) then
      local _t = {} 
      for i,v in ipairs(bezier_octant_I) do _t[i] = v end
      bezier_octant_contour[1] = _t; 
   else
      local _cnt=0 
      for i,v in ipairs(bezier_octant_contour) do _cnt=_cnt+#v end
      local _t = {} 
      for i,v in ipairs(bezier_octant_I) do if i>_cnt then _t[#_t+1] = v; end end
      bezier_octant_contour[#bezier_octant_contour+1] = _t
   end
   mflua.do_add_to.bezier_octant_contour = bezier_octant_contour 
   return 0
end
mflua.do_add_to._store_current_contour = _store_current_contour


local function _postprocessing_contour()
   local bezier_octant_contour,contour,path_list
   local chartable = mflua.chartable 
   local index 
   local char
   local res  = ""
   index = (0+print_int(MFbuiltin.char_code())) +  (0+print_int(MFbuiltin.char_ext()))*256
   res = res .. "%% postprocessing contour for " .. index ..";\n"
   res = res .. "path p[];\n"
   --print("\n_postprocessing_contour CHAR " .. index)
   bezier_octant_contour = mflua.do_add_to.bezier_octant_contour[#mflua.do_add_to.bezier_octant_contour]
   
   path_cnt = 1
   for i,v in ipairs(bezier_octant_contour) do
      contour = v
      path_list = contour['path_list']
      for i,path in ipairs(path_list) do
	 local p,c1,c2,q = path['p'],path['control1'],path['control2'],path['q']
	 if (q == nil) then
	    res = res .. string.format("p%d:=(%s);\n",   path_cnt,p)
	 else
	    res = res .. string.format("p%d:=(%s .. controls %s and %s .. %s);\n",path_cnt,p,c1,c2,q)
	 end
	 path_cnt = path_cnt +1
      end	 
   end
   if path_cnt > 1 then 
      res = res .. "drawoptions(withcolor black withpen pencircle scaled 0.3pt);\n"
      res = res .. "draw  p1"
      for i=2,path_cnt-1 do 
	 res = res .. string.format(" --  p%d",i)
      end 
      res = res .. " --cycle ;\n"
   end


   index = (0+print_int(MFbuiltin.char_code())) +  (0+print_int(MFbuiltin.char_ext()))*256
   --print("BEZ index="..index)
   char = chartable[index] or {}
   char['char_wd'] = print_scaled(MFbuiltin.char_wd()) 
   char['char_ht'] = print_scaled(MFbuiltin.char_ht()) 
   char['char_dp'] = print_scaled(MFbuiltin.char_dp()) 
   char['char_ic'] = print_scaled(MFbuiltin.char_ic())  
   char['contour'] = char['contour'] or {}
   char['contour'][#char['contour']+1] = bezier_octant_contour
   char['res']     =   char['res']  or "" 
   char['res']     =   char['res']  .. res ; 
   char['index'] = index
   chartable[index] = char 
  return 0
end
mflua.do_add_to._postprocessing_contour = _postprocessing_contour

-- local function _circular_list_geti(l,i) 
--    local size = #l
--    if size==0 then 
--     return nil
--    end
--    return l[1+(i-1)%size] 
-- end


local function _store_current_cycle(hs) 
   local res, current_cycle 
   res, current_cycle = mflua.do_add_to._get_cycle(hs)
   if res=='cycle' then 
     local index = (0+print_int(MFbuiltin.char_code())) +  (0+print_int(MFbuiltin.char_ext()))*256
     local char = mflua.chartable[index] or {}
     char['cycle'] = char['cycle'] or  {}
     char['cycle'][#char['cycle']+1] = current_cycle
     
     mflua.chartable[index] = char 
   else
      print("Error:"..res)
   end
end      
mflua.do_add_to._store_current_cycle = _store_current_cycle


local function PRE_make_spec_rhs(rhs)
   PRINTDBG("PRE_make_spec_rhs")
   mflua.do_add_to._store_current_cycle(rhs) 
end 

local function POST_make_spec_rhs(rhs)
   PRINTDBG("POST_make_spec_rhs")
   --mflua.do_add_to._store_current_cycle(rhs)
   --print("post rhs MFbuiltin.turning_number=",MFbuiltin.turning_number() ) ;
end 


local function PRE_make_spec_lhs(lhs)
  PRINTDBG("PRE_make_spec_lhs")
  mflua.do_add_to._store_current_cycle(lhs) 
end 

local function POST_make_spec_lhs(lhs)
  PRINTDBG("PRE_make_spec_lhs")
  --mflua.do_add_to._store_current_cycle(lhs) 
  --print("post lhs MFbuiltin.turning_number=",MFbuiltin.turning_number() ) ;
end 

local function PRE_fill_envelope_rhs(rhs)
   PRINTDBG("PRE_fill_envelope_rhs")
   local knots ,knots_list
   local index,char
   local chartable = mflua.chartable 
   knots = mflua.do_add_to._print_spec(rhs)
   index = (0+print_int(MFbuiltin.char_code())) +  (0+print_int(MFbuiltin.char_ext()))*256
   char = chartable[index] or {}
   knots_list = char['knots'] or {}
   knots_list[#knots_list+1] = knots 
   char['knots'] = knots_list
   chartable[index] = char 
end 

local function POST_fill_envelope_rhs(rhs) 
   PRINTDBG("POST_fill_envelope_rhs")
   mflua.do_add_to._store_current_envelope()
   mflua.do_add_to._postprocessing()
end 

local function PRE_fill_envelope_lhs(lhs)
   PRINTDBG("PRE_fill_envelope_lhs")
end 

local function POST_fill_envelope_lhs(lhs) 
   PRINTDBG("POST_fill_envelope_lhs")
   mflua.do_add_to._store_current_envelope()
   mflua.do_add_to._postprocessing()
end 

local function PRE_fill_spec_rhs(rhs)
   PRINTDBG("PRE_fill_spec_rhs")
   --print_specification_contour(rhs)
end 

local function POST_fill_spec_rhs(rhs) 
   PRINTDBG("POST_fill_spec_rhs")
   mflua.do_add_to._store_current_contour()
   mflua.do_add_to._postprocessing_contour()
end 

local function PRE_fill_spec_lhs(lhs)
   PRINTDBG("PRE_fill_spec_lhs")
   --print_specification_contour(lhs)
end 

local function POST_fill_spec_lhs(lhs) 
   PRINTDBG("POST_fill_spec_lhs")
   mflua.do_add_to._store_current_contour()
   mflua.do_add_to._postprocessing_contour()
end 


--------------------------------------------------------------------------------
--
-- fill_spec 
--
--------------------------------------------------------------------------------

mflua.fill_spec = mflua.fill_spec or {}

-- @ Here's a routine that prints a cycle spec in symbolic form, so that it
-- is possible to see what subdivision has been made.  The point coordinates
-- are converted back from \MF's internal ``rotated'' form to the external
-- ``true'' form. The global variable~|cur_spec| should point to a knot just
-- after the beginning of an octant boundary, i.e., such that
-- |left_type(cur_spec)=endpoint|.
local function print_specification_contour(h)
   local p,q,n,nh
   local octant
   local cur_spec
   local res = ""
   local f   
   local first_point, first_point_offset
   local path_cnt
   local bezier_contour,beziers_contour ={},{}
   local offset_list = {}
   local path_list ={}
   local bezier_octant_I
   cur_spec=h
   p=cur_spec 
   octant = left_octant(p)
   beziers_contour['octant_number'] = octant_number[octant]
   first_point = print_two_true(x_coord(p),y_coord(p),octant)
   first_point_offset = print_int(right_type(p))
   local end_loop_1 = false
   while end_loop_1 == false do
     local end_loop_2 = false
     while end_loop_2 == false do
     	q=link(p);
        if right_type(p)==endpoint then 
	   end_loop_2=true 
	else    
	   bezier_contour['p'] = print_two_true(x_coord(p),y_coord(p),octant);
           bezier_contour['control1'] = print_two_true(right_x(p),right_y(p),octant) 
           bezier_contour['control2'] = print_two_true(left_x(q),left_y(q),octant)
	   bezier_contour['q'] =  print_two_true(x_coord(q),y_coord(q),octant)
	   beziers_contour[#beziers_contour+1] = bezier_contour
	   bezier_contour = {}
	   p=q
       end 
     end 
     -- not_found label 
     if q==cur_spec then 
       end_loop_1=true 
     else
       p=q; octant=left_octant(p); -- print("% entering octant `");
     end
     --  We don't want all the octans of the cycle
     --  only the pieces of the current octant
     end_loop_1 = not(MFbuiltin.octant() == octant)
   end
   -- done label: 
   -- We can now use the results
   -- No curves stored
   if #beziers_contour == 0 then
     beziers_contour['single_point'] = first_point
   end
   beziers_contour['path_list'] = {}
   if #beziers_contour == 0 then
      path_list['p'] = beziers_contour['single_point']
      beziers_contour['path_list'][#beziers_contour['path_list']+1] = path_list
      path_list={}
   else   
      for i,v in ipairs(beziers_contour) do
	 bezier_contour = v 
	 path_list['p'] = bezier_contour['p'] 
	 path_list['control1'] = bezier_contour['control1'] 
	 path_list['control2'] = bezier_contour['control2'] 
	 path_list['q'] = bezier_contour['q'] 
	 beziers_contour['path_list'][#beziers_contour['path_list']+1] = path_list
	 path_list={}
      end
   end
   bezier_octant_I =mflua.do_add_to.bezier_octant_I 
   bezier_octant_I[#bezier_octant_I+1] = beziers_contour
  return 0
end
mflua.MF.print_specification_contour = print_specification_contour


local function PRE_move_to_edges(p) 
   PRINTDBG("PRE_move_to_edges")
   mflua.MF.print_specification_contour(p)
end


local function POST_move_to_edges(p) 
   PRINTDBG("POST_move_to_edges")
end


--
-- scan_direction
--

mflua.scan_direction = mflua.scan_direction  or {}

local function _print_path(h,s,nuline)
   local p,q
   local res 
   local done
   local done1
   local f
   done = false
   done1 = false 
   p = h
   res = '' 
   while not done do
      q = link(p)
      if (p==0) or (q==0) then
	 res = "???"
	 -- do something with res -- 
	 return 0
      end
      res = res .. print_two(x_coord(p),y_coord(p)); -- print("res=",res)
      if right_type(p) == endpoint then
	 if left_type(p)== open then print("{open?}") end -- {can't happen}
	 if (left_type(q) ~= endpoint) or (q ~= h) then q=null end -- {force an error}
	 done1 = true --  goto done1;
      elseif right_type(p) == explicit then 
	 -- begin "@<Print control points between |p| and |q|, then |goto done1|@>"
	 res = res .. "..controls " ..  print_two(right_x(p),right_y(p)) .. " and ";
	 if left_type(q) ~= explicit then print("??")  -- {can't happen}
	 else res = res .. print_two(left_x(q),left_y(q));
	    done1 = true -- goto done1;
	 end
	 -- end "@<Print control points between |p| and |q|, then |goto done1|@>"
      elseif right_type(p) == open then 
	 -- begin "@<Print information for a curve that begins |open|@>" 
	 if (left_type(p) ~= explicit) and (left_type(p)~=open) then
	    res = res .. "{open?}" -- {can't happen}
	 end  
	 -- end "@<Print information for a curve that begins |open|@>" 
      elseif (right_type(p) == curl) or (right_type(p) == given) then 
	 -- @ A curl of 1 is shown explicitly, so that the user sees clearly that
	 -- \MF's default curl is present.
	 -- begin @<Print information for a curve that begins |curl|...@>=
	 if left_type(p)==open then res = res .. "??" end --  {can't happen}
	 if right_type(p)==curl then
	    res = res .. "{curl ".. print_scaled(right_curl(p))
	 else  n_sin_cos(right_given(p)); res = res .."{"
	    res = res .. print_scaled(n_cos) .. "," ..  print_scaled(n_sin)
	 end
	 res = res .."}"
	 -- end @<Print information for a curve that begins |curl|...@>=
      else res = res .. "???" -- {can't happen}
      end 
      if not done1 then -- mimic label done 1
	 if left_type(q)~=explicit then res = res .. "..control?" --   {can't happen}
	 else if (right_tension(p) ~= unity) or (left_tension(q) ~= unity) then
	       -- begin "@<Print tension between |p| and |q|@>;" 
	       res = res .. "..tension "
	       if right_tension(p)<0 then res = res .. "atleast" end 
	       res = res .. print_scaled(math.abs(right_tension(p)))
	       if right_tension(p) ~= left_tension(q) then
		  res = res .. " and "
		  if left_tension(q)<0 then res = res .. "atleast" end
		  res = res .. print_scaled(math.abs(left_tension(q)))
	       end
	    end -- "@<Print tension between |p| and |q|@>;"
	 end
      end --- LABEL:  done1 
      -- begin @<Print two dots...@>=
      p = q
      res = res .. " .." 
      if left_type(p)==given then
	 n_sin_cos(left_given(p)); res = res .. "{"
	 res = res  .. print_scaled(n_cos); res = res .. ",";
	 res = res .. print_scaled(n_sin); res = res .. "}";
      else if left_type(p)==curl then
	    res = res .. "{curl "; res = res .. print_scaled(left_curl(p)) .. "}";
	 end;
      end
      -- end @<Print two dots...@>=
      -- end "@<Print information for adjacent knots |p| and |q|@>"
      if p == h then done =true end
   end
   if left_type(h) ~= endpoint then 
      res = res .. "cycle" 
   else 
      res = res
   end
   -- do something with res --
   return res 
   --res = "drawoptions(withcolor black withpen pencircle scaled 1pt);\n" .. "draw " ..  res .. " ;\n" 
   --print(res)
   -- local index = (0+print_int(MFbuiltin.char_code())) +  (0+print_int(MFbuiltin.char_ext()))*256
   -- local char = mflua.chartable[index] or {}
   -- char['char_wd'] = print_scaled(MFbuiltin.char_wd()) 
   -- char['char_ht'] = print_scaled(MFbuiltin.char_ht()) 
   -- char['char_dp'] = print_scaled(MFbuiltin.char_dp()) 
   -- char['char_ic'] = print_scaled(MFbuiltin.char_ic())  
   -- char['res']     =   char['res']  or "" 
   -- char['res']     =   char['res']  .. res 
   -- mflua.chartable[index] = char 
   -- return 0
end
mflua.scan_direction.print_path = _print_path

local function PRE_make_choices(p)
   PRINTDBG("PRE_make_choices")
   -- _print_path(p,"PRE make choice",false)
end

local function POST_make_choices(p)
   PRINTDBG("POST_make_choices")
   --local res = ''
   --res = _print_path(p,"POST make choice",false)
   --print(res)
end

local function print_retrograde_line(x0,y0,cur_x,cur_y)
   PRINTDBG("print_retrograde_line")
   local chartable = mflua.chartable 
   local index = (0+print_int(MFbuiltin.char_code())) +  (0+print_int(MFbuiltin.char_ext()))*256
   local char = chartable[index] or {}
   local tab = char['retrograde_line'] or  {}
   tab[#tab+1] = {print_two(x0,y0),print_two(cur_x,cur_y)}
   char['retrograde_line'] = tab
   --print("%%Retrograde line")
   --print("drawoptions(withcolor (0,0.6,0) withpen pencircle scaled 0.09pt);")
   --print("draw "..  print_two(x0,y0) .. " -- " .. print_two(cur_x,cur_y) ..";")
end


local function PRE_make_ellipse(major_axis,minor_axis,theta,tx,ty,q)
   PRINTDBG("PRE_make_ellipse")
 --print("major_axis,minor_axis,theta,tx,ty,q=",major_axis,minor_axis,theta,tx,ty,q)
end

local function POST_make_ellipse(major_axis,minor_axis,theta,tx,ty,q)
   PRINTDBG("POST_make_ellipse")
   --print("major_axis,minor_axis,theta,tx,ty,q=",print_two(major_axis,minor_axis),theta*(2^-20),print_two(tx,ty),print_two(x_coord(q),y_coord(q)))
   local flag=true
   local p=q
   local res = ''
   local xy
   local i = 0
   while flag do
      i=i+1
  res = res ..print_two(x_coord(p),y_coord(p))
  p=link(p)
  if p==q then flag=false end
   end 
   mflua.pen[res] = {print_two(major_axis,minor_axis),  
		     theta*(2^-20),print_two(tx,ty)}

end



local function print_transition_line_from(x,y)
   PRINTDBG("print_transition_line_from")
   local octant = MFbuiltin.octant()
   --local res = ""
   local index = (0+print_int(MFbuiltin.char_code())) +  (0+print_int(MFbuiltin.char_ext()))*256
   local chartable = mflua.chartable
   local char = chartable[index] or {}
   local p = print_two_true(x,y,octant)
   local c1 = p
   char['transition_lines'] =  char['transition_lines'] or {}
   char['transition_lines'][#char['transition_lines']+1]={p,p} 
   return 0
end 


local function print_transition_line_to(x,y)
   PRINTDBG("print_transition_line_to")
   local octant = MFbuiltin.octant()
   local index = (0+print_int(MFbuiltin.char_code())) +  (0+print_int(MFbuiltin.char_ext()))*256
   local chartable = mflua.chartable
   local char = chartable[index] or {}
   local q = print_two_true(x,y,octant)
   local c2 = q
   char['transition_lines_set'] =  char['transition_lines_set'] or {}
   char['transition_lines'] =  char['transition_lines'] or {}
   local _t = char['transition_lines'][#char['transition_lines']]
   local p = _t[1]
   --
   -- avoid multiple lines
   --
   --print("BEZ char['transition_lines_set'][p..q]=",char['transition_lines_set'][p..q])
   if char['transition_lines_set'][p..q]==true or char['transition_lines_set'][q..p]==true then
      char['transition_lines'][#char['transition_lines']]=nil
      return 0
   end
   char['transition_lines_set'][p..q]=true
   char['transition_lines_set'][q..p]=true
   --
   -- remove curves that degenerate into points
   --
   if p~=q then
      _t[#_t+1] = c2
      _t[#_t+1] = q
      _t[#_t+1] = '(0,0)'
      _t[#_t+1] = 'transition_line'
   else
      char['transition_lines'][#char['transition_lines']]=nil
   end
   return 0
end 


local function end_program()
   PRINTDBG("end_program")
   local f = kpse.find_file('end_program.lua','lua')
   if f==nil then 
      print("Warning: end_program.lua not found")
      return 
   end
   local func,errmsg  = loadfile(f)
   if not(func) then
      print(errmsg)
      os.exit(1)
   end
   status, msg = pcall(func)
   if not(status) then
      print(msg)
      os.exit(status)
   end
end    


--
-- Add local function to mflua
--

mflua.begin_program			 = begin_program
mflua.PRE_start_of_MF			 = PRE_start_of_MF
mflua.PRE_main_control			 = PRE_main_control
mflua.POST_main_control			 = POST_main_control
mflua.mflua_initialize			 = mflua_initialize
mflua.POST_final_cleanup		 = POST_final_cleanup
mflua.printpath				 = printpath
mflua.printedges			 = printedges
mflua.PRE_offset_prep			 = PRE_offset_prep
mflua.POST_offset_prep			 = POST_offset_prep

mflua.PRE_make_spec_rhs			 = PRE_make_spec_rhs
mflua.POST_make_spec_rhs		 = POST_make_spec_rhs
mflua.PRE_make_spec_lhs			 = PRE_make_spec_lhs
mflua.POST_make_spec_lhs		 = POST_make_spec_lhs
mflua.PRE_fill_envelope_rhs		 = PRE_fill_envelope_rhs
mflua.POST_fill_envelope_rhs		 = POST_fill_envelope_rhs
mflua.PRE_fill_envelope_lhs		 = PRE_fill_envelope_lhs
mflua.POST_fill_envelope_lhs		 = POST_fill_envelope_lhs
mflua.PRE_fill_spec_rhs			 = PRE_fill_spec_rhs
mflua.POST_fill_spec_rhs		 = POST_fill_spec_rhs
mflua.PRE_fill_spec_lhs			 = PRE_fill_spec_lhs
mflua.POST_fill_spec_lhs		 = POST_fill_spec_lhs

mflua.PRE_move_to_edges			 = PRE_move_to_edges 
mflua.POST_move_to_edges		 = POST_move_to_edges

mflua.PRE_make_choices			 = PRE_make_choices
mflua.POST_make_choices			 = POST_make_choices

mflua.print_retrograde_line		 = print_retrograde_line

mflua.PRE_make_ellipse			 = PRE_make_ellipse
mflua.POST_make_ellipse			 = POST_make_ellipse

mflua.print_transition_line_from	 = print_transition_line_from
mflua.print_transition_line_to		 = print_transition_line_to

mflua.end_program			 = end_program           


--------------------------------------------------------------------------------
--
-- Other setups, mostly depend on old routines 
--
--------------------------------------------------------------------------------

mflua.max_recursion_level = 32


mflua.bit = 7					 -- should be 4 

mflua.pi = 2*math.atan2(1,0)
mflua.print_specification = mflua.print_specification or {}
mflua.print_specification.temp1 = 0
mflua.print_specification.p  = ""
mflua.print_specification.q  = ""
mflua.threshold_path_removed = 4		 -- how many path we can safely remove 
mflua.threshold_extra_step = 2			 -- add values/mflua.threshold_extra_step time values 
mflua.threshold_small_path_check_point = 3	 -- check  3 pixels for horiz/vert. paths 
mflua.threshold_small_pen_path = 0.001		 -- _fix_wrong_pending_path
mflua.threshold_fix = 1				 -- _fix_wrong_pending_path
mflua.threshold = 1				 -- _remove_small_path
mflua.threshold_degree = 2			 -- _remove_small_path
mflua.threshold_degree_1 = 90			 -- _remove_small_path
mflua.threshold_degree_2 = 270			 -- _remove_small_path
mflua.threshold_small_curve = 2			 -- _remove_reduntant_curves
mflua.threshold_normal_curve = 4		 -- _remove_reduntant_curves
mflua.threshold_min_dist = 0.5			 -- _remove_reduntant_curves
mflua.threshold_pending_path = 0.002		 -- _remove_reduntant_curves
mflua.threshold_pen = 5				 -- _remove_redundant_segments
mflua.threshold_bug = 4				 -- _fix_intersection_bug
mflua.threshold_min_bug = 0.03			 -- _fix_intersection_bug
mflua.threshold_equal_path=0.03			 -- _remove_duplicate_pen_path
mflua.threshold_straight_line = 0.125            -- _is_a_straight_line
mflua.threshold_fix_knots = 0.125		 -- _fix_knots
mflua.threshold_fix_knots_1 = 0.0005		 -- _fix_knots
mflua.threshold_fix_knots_2 = 0.4		 -- _fix_knots
mflua.threshold_remove_redundant_pen = 0.02      -- remove_redundant_pen 
mflua.threshold_remove_redundant_curves = 3      -- _remove_redundant_curves
mflua.threshold_merge_segments =  5e-5		 -- _merge_segments
mflua.threshold_join_curves =  0.049		 -- _build_cycles try and error
mflua.set_poly_done={}
mflua.mflua_exe = 'mflua'
mflua.turningnumber_file='mflua_tn'
mflua.fill_envelope = {}
mflua.fill_envelope.temp_transition = ""
mflua.pen = {}					 -- collect bezier curves of the pens

--

mflua.chartable  ={}
mflua.max_curves =1e4


function mflua.lock(params) 
   if params ==nil then 
      return io.open('LOCK1','w') 
   else 
      return io.open(tostring(params),'w') 
   end 
end

function mflua.unlock(params) 
   if params == nil then 
      return os.remove('LOCK1') 
   else
      return os.remove(tostring(params))
   end
end

function mflua.checklock(params) 
   if params == nil then 
      if io.open("LOCK1") ~= nil then return true else return false end
   else
      if io.open(tostring(params)) ~= nil then return true else return false end
   end
end


function mflua.dot(P1,P2)
   return P1[1]*P2[1]+P1[2]*P2[2]
end

function mflua.angle(p,q) 
   local dot = mflua.dot  
   if math.abs(1 - dot(p,q)/(math.sqrt(dot(p,p))*math.sqrt(dot(q,q)))) <0.0001 then 
      return 0 
   else 
      return math.acos(dot(p,q)/(math.sqrt(dot(p,p))*math.sqrt(dot(q,q))))
   end 
end

-- function mflua.vec(a,w,b1) if b1 == nil then b=w else b = b1 end ; return {b[1]-a[1],b[2]-a[2]} end
-- mflua.vec(a,b) == mflua.vec(a,'->',b)
function mflua.round(p)
   local w=string.gmatch(p,"[-0-9.]+");
   local p p={w(),w()};
   return string.format("(%6.5f,%6.5f)",tostring(p[1]),tostring(p[2]) )
end

function mflua.round5(p)
   local w=string.gmatch(p,"[-0-9.]+");
   local p; p={w(),w()};
   return string.format("(%6.5f,%6.5f)",tostring(p[1]),tostring(p[2]) )
end

function mflua.round2(p)
   local w=string.gmatch(p,"[-0-9.]+");
   local p; p={w(),w()};
   return string.format("(%6.2f,%6.2f)",tostring(p[1]),tostring(p[2]) )
end

function mflua.round1(p)
   local w=string.gmatch(p,"[-0-9.]+");
   local p; p={w(),w()};
   return string.format("(%6.1f,%6.1f)",tostring(p[1]),tostring(p[2]) )
end

function mflua.round0(p)
   local w=string.gmatch(p,"[-0-9.]+");
   local p; p={w(),w()};
   return string.format("(%6.0f,%6.0f)",tostring(p[1]),tostring(p[2]) )
end

function mflua.floor(p)
   local w=string.gmatch(p,"[-0-9.]+");
   local p; p={w(),w()};
   return string.format("(%d,%d)",math.floor(p[1]),math.floor(p[2]) )
end

function mflua.round5_table(p)
   return {tonumber(string.format("%6.5f",tostring(p[1]))),tonumber(string.format("%6.5f",tostring(p[2])))}
end

function mflua.number_to_string_round5(p)
   return string.format("(%6.5f,%6.5f)",p[1],p[2])
end


function mflua.vec(a,b)
   return {b[1]-a[1],b[2]-a[2]}
end

function mflua.modul_vec(a,b)
   local dot = mflua.dot
   local P ={b[1]-a[1],b[2]-a[2]}
   return math.sqrt(dot(P,P))
end

function mflua.approx_curve_lenght(p,c1,c2,q)
   return  mflua.modul_vec(p,c1) + mflua.modul_vec(c1,c2) + mflua.modul_vec(c2,q) + mflua.modul_vec(p,q)
end


-- for multiple instances of mflua one can define a LOCK like this 
-- if io.open('LOCK1')==nil and io.open('LOCK_ELLIPSE')==nil then 
--    mflua.print_specification.filename  = "envelope.tex"
--    mflua.print_specification.outfile1  = io.open(mflua.print_specification.filename,'w')
-- end



--------------------------------------------------------------------------------
--
-- tfm module
--
--------------------------------------------------------------------------------
local tfm = {}
tfm.bits = 
   function(a,l) 
      local bita = {}
      local a = a
      for k=1,l do 
	 local r=math.fmod(a,2)  
	 a=math.floor(a/2) 
	 bita[k]=r
      end 
      return bita
   end
tfm.bitand =
   function(a,b,l)
      local bita,bitb,bitc = tfm.bits(a,l), tfm.bits(b,l),{}
      local c= 0
      for k=1,l do 
	 bit1,bit2 = bita[k],bitb[k]
	 if (bit1==1) and (bit2==1) then 
	    bitc[k]=1
	 else
	    bitc[k]=0
	 end
	 c = c+2^(k-1)*bitc[k]
      end 
      return c,bitc
   end
tfm.bitor =
   function(a,b,l)
      local bita,bitb,bitc = tfm.bits(a,l), tfm.bits(b,l),{}
      local c= 0
      for k=1,l do 
	 bit1,bit2 = bita[k],bitb[k]
	 if (bit1==1) or (bit2==1) then 
	    bitc[k]=1
	 else
	    bitc[k]=0
	 end
	 c = c+ 2^(k-1)*bitc[k]
      end 
      return c,bitc
   end
tfm.bitnot =
   function(a,l)
      local bita,bitb = tfm.bits(a,l),{}
      local b= 0
      for k=1,l do 
	 bit1 = bita[k]
	 if (bit1==1) then 
	    bitb[k]=0
	 else
	    bitb[k]=1
	 end
	 b = b+ 2^(k-1)*bitb[k]
      end 
      return b,bitb
   end
tfm.printbits=
   function(t,l) 
      local r = ''
      local l = l or #t
      if l==0 then return '' end 
      for k=l,1,-1 do 
	 local v = t[k] or '0'
	 r=r..v
      end 
      return r
   end

tfm.stop_flag = 128 
tfm.kern_flag = 128 

tfm.parameters = {}
tfm.parameters.init =
   function()
      -- 8. The first 24 bytes (6 words) of a TFM file contain twelve 16-bit integers that give the lengths of the
      -- various subsequent portions of the file. These twelve integers are, in order:
      -- lf = length of the entire file, in words;
      -- lh = length of the header data, in words;
      -- bc = smallest character code in the font;
      -- ec = largest character code in the font;
      -- nw  = number of words in the width table;
      -- nh  = number of words in the height table;
      -- nd  = number of words in the depth table;
      -- ni = number of words in the italic correction table;
      -- nl = number of words in the lig/kern table;
      -- nk  = number of words in the kern table;
      -- ne  = number of words in the extensible character table;
      -- np  = number of font parameter words.
      -- They are all nonnegative and less than 2^15 . We must have bc - 1<=  ec <= 255, ne<=  256, and
      -- 	 lf = 6 + lh + (ec - bc + 1) + nw + nh + nd + ni + nl + nk + ne + np .
      -- When two or more 8-bit bytes are combined to form an integer of 16 or more bits, the most
      -- significant bytes appear first in the file. This is called BigEndian order.
      
      if tfm.content ==nil or type(tfm.content)~= 'string' then
	 return false, 'Error on file content'
      end
      tfm.parameters.w = string.gmatch(tfm.content,"[%z%Z]")
      local w = tfm.parameters.w
      local W1,W2

      W1,W2=w(),w();if W1==nil or W2==nil then return false, 'Error on parsing content (lf)'  end
      tfm.parameters.lf = 256*string.byte(W1)+string.byte(W2) -- length of the entire file, in words;

      W1,W2=w(),w();if W1==nil or W2==nil then return false, 'Error on parsing content (lh)'  end
      tfm.parameters.lh = 256*string.byte(W1)+string.byte(W2) -- length of the header data, in words;

      W1,W2=w(),w();if W1==nil or W2==nil then return false, 'Error on parsing content (bc)'  end
      tfm.parameters.bc = 256*string.byte(W1)+string.byte(W2) -- smallest character code in the font;

      W1,W2=w(),w();if W1==nil or W2==nil then return false, 'Error on parsing content (ec)'  end
      tfm.parameters.ec = 256*string.byte(W1)+string.byte(W2) -- largest character code in the font;

      W1,W2=w(),w();if W1==nil or W2==nil then return false, 'Error on parsing content (nw)'  end
      tfm.parameters.nw = 256*string.byte(W1)+string.byte(W2) -- number of words in the width table;

      W1,W2=w(),w();if W1==nil or W2==nil then return false, 'Error on parsing content (nh)'  end
      tfm.parameters.nh = 256*string.byte(W1)+string.byte(W2) -- number of words in the height table;

      W1,W2=w(),w();if W1==nil or W2==nil then return false, 'Error on parsing content (nd)'  end
      tfm.parameters.nd = 256*string.byte(W1)+string.byte(W2) -- number of words in the depth table;

      W1,W2=w(),w();if W1==nil or W2==nil then return false, 'Error on parsing content (ni)'  end
      tfm.parameters.ni = 256*string.byte(W1)+string.byte(W2) -- number of words in the italic correction table;

      W1,W2=w(),w();if W1==nil or W2==nil then return false, 'Error on parsing content (nl)'  end
      tfm.parameters.nl = 256*string.byte(W1)+string.byte(W2) -- number of words in the lig/kern table;

      W1,W2=w(),w();if W1==nil or W2==nil then return false, 'Error on parsing content (nk)'  end
      tfm.parameters.nk = 256*string.byte(W1)+string.byte(W2) -- number of words in the kern table;

      W1,W2=w(),w();if W1==nil or W2==nil then return false, 'Error on parsing content (ne)'  end
      tfm.parameters.ne = 256*string.byte(W1)+string.byte(W2) -- number of words in the extensible character table;

      W1,W2=w(),w();if W1==nil or W2==nil then return false, 'Error on parsing content (np)'  end
      tfm.parameters.np = 256*string.byte(W1)+string.byte(W2) -- number of font parameter words.
      return 
   end

tfm.parameters.check = 
   function()
      local w  = tfm.parameters.w  
      local lf = tfm.parameters.lf
      local lh = tfm.parameters.lh
      local bc = tfm.parameters.bc
      local ec = tfm.parameters.ec
      local nw = tfm.parameters.nw
      local nh = tfm.parameters.nh
      local nd = tfm.parameters.nd
      local ni = tfm.parameters.ni
      local nl = tfm.parameters.nl
      local nk = tfm.parameters.nk
      local ne = tfm.parameters.ne
      local np = tfm.parameters.np
      local status = true 
      local status_cnt = 0
      local status_msg = 'OK'
      local function _assert(cond,msg)
	 if cond==false then
	    --print(msg) 
	    status_msg = msg
	    status_cnt = status_cnt +1
	 end
      end
      if status_cnt == 0 then _assert(lf == 6 + lh + (ec - bc + 1) + nw + nh + nd + ni + nl + nk + ne + np ,"Error on lf") end
      if status_cnt == 0 then _assert(0<=lf  and lf <2^15,"lf  out of range") end
      if status_cnt == 0 then _assert(0<=lh  and lh <2^15,"lh  out of range") end
      if status_cnt == 0 then _assert(0<=bc  and bc <2^15,"bc  out of range") end
      if status_cnt == 0 then _assert(0<=ec  and ec <2^15,"ec  out of range") end
      if status_cnt == 0 then _assert(0<=nw  and nw <2^15,"nw  out of range") end
      if status_cnt == 0 then _assert(0<=nh  and nh <2^15,"nh  out of range") end
      if status_cnt == 0 then _assert(0<=nd  and nd <2^15,"nd  out of range") end
      if status_cnt == 0 then _assert(0<=ni  and ni <2^15,"ni  out of range") end
      if status_cnt == 0 then _assert(0<=nl  and nl <2^15,"nl  out of range") end
      if status_cnt == 0 then _assert(0<=nk  and nk <2^15,"nk  out of range") end
      if status_cnt == 0 then _assert(0<=ne  and ne <2^15,"ne  out of range") end
      if status_cnt == 0 then _assert(0<=np  and np <2^15,"np  out of range") end
      if status_cnt == 0 then _assert( (bc-1)<=ec and ec<= 255, "Error on bc and ec") end
      if status_cnt == 0 then _assert(ne<= 256, "ne >256")			      end
      if status_cnt > 0 then status=false end
      return status,status_msg 
   end

   
tfm.int_to_frac = 
   function(d)
      if not (0<=d and d<2^32) then 
	 return nil, string.format("Error: %s out of range (-%s,%s-2^-20)",d,2^31,2^31) 
      else 
	 if d<=2147483647 then 
	    return d/2^20,'ok'
	 else
	    return (d-2^32)/2^20,'ok'
	 end
      end
      
   end
tfm.tag_meaning = 
   function(d)
      local t = {'vanilla character','character has a ligature/kerning program',
		 'character has a successor in a charlist','character is extensible'}
      local d = tonumber(d) or -1
      if d<0 or d>3 then return '' end
      return t[tonumber(d+1)]
   end

tfm.getface = 
   function(d)
      --If the value is less than 18, it has the following
      --interpretation as a "weight, slope, and expansion": Add 0 or 2 or 4 (for medium or bold or light) to
      --0 or 1 (for roman or italic) to 0 or 6 or 12 (for regular or condensed or extended). For example, 13 is
      --0+1+12, so it represents medium italic extended. A three-letter code (e.g., MIE) can be used for such
      --face data.
      -- d = 3⋅rce_bit⋅2^{2,1} + ri_bit + mbl_bit⋅2^{2,1}  , rce_bit,ri_bit,mbl_bit ∈ {0,1}
      -- d%2 = ri_bit := ri 
      -- d%3 - d%2 = mbl⋅2^{2,1} := mbl
      -- d - d%3 =  3⋅rce⋅2^{2,1} := rce
      if d>17 then return '' end
      local mbl,ri, rce = '*','*','*'
      local _ri = d % 2 
      if _ri==0 then ri ='R' else ri ='I' end
      local _mbl = (d%3) -(d%2) 
      if _mbl == 0 then mbl='M' end
      if _mbl == 2 then mbl='B' end
      if _mbl == 4 then mbl='E' end
      local _rce = d - (d%3) 
      if _rce == 0 then rce = 'M' end
      if _rce == 1 then rce = 'I' end
      if _rce == 12 then rce = 'E' end
      return mbl..ri..rce, {_mbl,_ri,_rce}
   end

tfm.printfloat =
   function(d,p)
      if d then
       local d,p = d, tonumber(p) or 6
       local f = string.format("%%.%df",p)
       return string.format(f,d)
      else 
        return tostring(d)
      end
   end 

tfm.array = {}
tfm.array.check = 
   function()
      local width     = tfm.array.width       
      local height    = tfm.array.height      
      local depth     = tfm.array.depth       
      local italic    = tfm.array.italic      
      local status = true 
      local status_cnt = 0
      local status_msg = 'OK'
      local function _assert(cond,msg)
	 if cond==false then
	    --print(msg) 
	    status_msg = msg
	    status_cnt = status_cnt +1
	 end
      end
      _assert(width[0]==0,'Error in width[0]')
      _assert(height[0]==0,'Error in height[0]')
      _assert(depth[0]==0,'Error in depth[0]')
      _assert(italic[0]==0,'Error in italic[0]')
      if status_cnt > 0 then status=false end
      return status,status_msg 

   end

tfm.dump={} 
tfm.dump.kernprogram =
   function(d,c)
      local char_info = tfm.array.char_info
      local lig_kern  = tfm.array.lig_kern 
      local kern      = tfm.array.kern 
      local current_step = d
      local current_char = c
      local skip_byte=lig_kern[current_step][1]
      local next_char=lig_kern[current_step][2]
      local op_byte=lig_kern[current_step][3]
      local remainder=lig_kern[current_step][4]
      local _print = tfm.printdebug()
      _print("-------")
      _print("skip_byte=",skip_byte)
      _print("next_char=",next_char,'C '..string.char(next_char),'O '..string.format("%o",next_char))
      _print("op_byte=",op_byte)
      _print("remainder=",remainder,string.format("O %o",remainder))
      if op_byte <tfm.kern_flag  then -- a ligature step
	 -- op_byte= 4a+2b+c where 0<=a<=b+c and 0<=b,c<=1.
	 local a,b,c
	 c=  op_byte%2
	 b = ((op_byte-c)%4)/2
	 a = (op_byte-2*b-c)/4
	 _print("ligature step: a,b,c=",a,b,c)
	 if b==0 then 
	    _print("current char: deleted") 
	 else 
	    _print("current char: not deleted" ) 
	 end 
	 if c==0 then 
	    _print("next char: deleted") 
	 else 
	    _print("next char: not deleted ") 
	 end
	 if a==0 then 
	    _print("no next char") 
	 else 
	    -- we must pass over the next a characters
	    -- local _,_remainder,_,_,_tag,_ = char_info[current_char+a]
	    -- if _tag==1 then 
	    --    local kern_program = tfm.dump.kernprogram(_remainder,current_char+a)
	    -- end
	 end 
	 tfm.chars[current_char] = tfm.chars[current_char] or {}
	 tfm.chars[current_char].ligature =  tfm.chars[current_char].ligature or {}
	 tfm.chars[current_char].ligature[#tfm.chars[current_char].ligature+1]= 
	    {['next_char']=next_char,['a']=a,['b']=b,['c']=c}

      else -- a kern step
	 local additional_space = kern[256*(op_byte-128)+remainder]
	 _print("kern step:additional_space=",tfm.printfloat(additional_space))
	 tfm.chars[current_char] = tfm.chars[current_char] or {}
	 tfm.chars[current_char].kern =  tfm.chars[current_char].kern or {}
	 tfm.chars[current_char].kern[#tfm.chars[current_char].kern+1] = 
	    {['next_char']=next_char,['additional_space']=additional_space}
      end

      if skip_byte>=tfm.stop_flag then 
	 -- end
	 return 
      else 
	 -- take next instruction	 
	 tfm.dump.kernprogram(current_step+1+skip_byte,current_char)
      end
   end


tfm.build = {}
tfm.build.header =
   function(i,j,w)
      local _i,_j,_a = i,j,{}
      for i=_i,_j do 
	 if i<=1 or i>17 then 
	    _a[i] =2^24*string.byte(w())+2^16*string.byte(w())+2^8*string.byte(w())+string.byte(w())
	 elseif 2<=i and i<=16 then 
	    _a[i] =w()..w()..w()..w()
	 elseif i==17 then 
	    _a[i] ={string.byte(w()),string.byte(w()),string.byte(w()),string.byte(w())}
	 end
      end
      return _a
   end

tfm.build._bytearray = 
   function(i,j,w) 
      local _i,_j,_a = i,j,{}
      for i=_i,_j do 
	 _a[i] ={string.byte(w()),string.byte(w()),string.byte(w()),string.byte(w())}
      end
      return _a
   end
tfm.build.char_info = tfm.build._bytearray
tfm.build.lig_kern  = tfm.build._bytearray
tfm.build.exten     = tfm.build._bytearray

tfm.build._dimension = 
   function(i,j,w) 
      local _i,_j,_a = i,j,{}
      for i=_i,_j do 
	 _a[i] = tfm.int_to_frac(2^24*string.byte(w())+2^16*string.byte(w())+2^8*string.byte(w())+string.byte(w()))
      end
      return _a
   end
tfm.build.width  = tfm.build._dimension
tfm.build.height = tfm.build._dimension
tfm.build.depth  = tfm.build._dimension
tfm.build.italic = tfm.build._dimension
tfm.build.kern   = tfm.build._dimension
tfm.build.param  = tfm.build._dimension

tfm.build.all = 
   function()
      local w  = tfm.parameters.w  
      local lf = tfm.parameters.lf
      local lh = tfm.parameters.lh
      local bc = tfm.parameters.bc
      local ec = tfm.parameters.ec
      local nw = tfm.parameters.nw
      local nh = tfm.parameters.nh
      local nd = tfm.parameters.nd
      local ni = tfm.parameters.ni
      local nl = tfm.parameters.nl
      local nk = tfm.parameters.nk
      local ne = tfm.parameters.ne
      local np = tfm.parameters.np
      tfm.array.header    = tfm.build.header(0,lh-1,w)
      tfm.array.char_info = tfm.build.char_info(bc,ec,w)
      tfm.array.width     = tfm.build.width( 0,nw-1,w)
      tfm.array.height    = tfm.build.height(0,nh-1,w)
      tfm.array.depth     = tfm.build.depth( 0,nd-1,w)
      tfm.array.italic    = tfm.build.italic(0,ni-1,w) 
      tfm.array.lig_kern  = tfm.build.lig_kern(0,nl-1,w)
      tfm.array.kern      = tfm.build.kern(0,nk-1,w)
      tfm.array.exten     = tfm.build.exten(0,ne-1,w)
      tfm.array.param     = tfm.build.param(1,np,w)
   end

tfm.debug = 0
tfm.printdebug = 
   function()
      if tfm.debug==1 then
	 return print
      else
	 return function(...) end
      end
   end

tfm.getdata={}
tfm.getdata.char_info = 
   function(i)
      local char_info = tfm.array.char_info
      local width_index = char_info[i][1]
      local height_index_plus_depth_index = char_info[i][2]
      local italic_index_plus_tag = char_info[i][3]
      local remainder = char_info[i][4]
      local depth_index = height_index_plus_depth_index % 16
      local height_index= (height_index_plus_depth_index -depth_index)/16
      local tag = italic_index_plus_tag % 4
      local italic_index = (italic_index_plus_tag -tag)/4
      return width_index,remainder,depth_index,height_index,tag,italic_index
   end
tfm.chars = {}
tfm.font = {}

tfm.run=
   function(name)
      local name = name
      local _print = tfm.printdebug()
      local header    ={}        
      local char_info ={}    
      local width     ={}    
      local height    ={}    
      local depth     ={}    
      local italic    ={}    
      local lig_kern  ={}    
      local kern      ={}    
      local exten     ={}    
      local param     ={}    
      local w   
      local lf 
      local lh 
      local bc 
      local ec 
      local nw 
      local nh 
      local nd 
      local ni 
      local nl 
      local nk 
      local ne 
      local np 

      if  kpse.find_file(name)==nil then 
	 name = name .. ".tfm"
      end
      if  kpse.find_file(name)==nil then 
	 return false, "Error: file "..tostring(name).." not found" 
      end
      tfm.name = name
      tfm.file = io.open(tfm.name,'rb')
      if tfm.file == nil then 
	 return false, "Error on opening file "..tostring(tfm.name) 
      end 
      tfm.content = tfm.file:read("*a")
      status,status_msg = tfm.parameters.init()
      if status== false then 
	 _print("ERROR="..tostring(status_msg))
	 return false, tostring(status_msg)
      end

      status,status_msg = tfm.parameters.check()
      if status == false then 
	 _print("ERROR="..tostring(status_msg))
	 return false, tostring(status_msg)
      end
      -- Build all arrays
      tfm.build.all() 
      header    = tfm.array.header          
      char_info = tfm.array.char_info   
      width     = tfm.array.width       
      height    = tfm.array.height      
      depth     = tfm.array.depth       
      italic    = tfm.array.italic      
      lig_kern  = tfm.array.lig_kern    
      kern      = tfm.array.kern        
      exten     = tfm.array.exten       
      param     = tfm.array.param       

      w  = tfm.parameters.w  
      lf = tfm.parameters.lf
      lh = tfm.parameters.lh
      bc = tfm.parameters.bc
      ec = tfm.parameters.ec
      nw = tfm.parameters.nw
      nh = tfm.parameters.nh
      nd = tfm.parameters.nd
      ni = tfm.parameters.ni
      nl = tfm.parameters.nl
      nk = tfm.parameters.nk
      ne = tfm.parameters.ne
      np = tfm.parameters.np

      status,status_msg = tfm.array.check()
      if status == false then 
	 _print("ERROR="..tostring(status_msg))
	 return false, tostring(status_msg)
      end
      tfm.font.checksum =header[0]
      tfm.font.designsize = tfm.int_to_frac(header[1])
      local coding_scheme 
      if header[2]~= nil then 
	 coding_scheme = '' 
	 for j=2,11 do coding_scheme=coding_scheme..tostring(header[j]) end 
	 tfm.font.coding_scheme=coding_scheme
	 _print(string.format("CODING SCHEME:%s",tfm.font.coding_scheme))
      end
      local font_identifier
      if header[12]~= nil then 
	 font_identifier = '' 
	 for j=12,16 do font_identifier=font_identifier..tostring(header[j]) end 
	 tfm.font.font_identifier=font_identifier
	 _print(string.format("FONT IDENTIFIER:%s",tfm.font.font_identifier))
      end
      local  seven_bit_safe_flag ,face
      if header[17]~= nil then
	 seven_bit_safe_flag ,face = header[17][1],header[17][4]
	 tfm.font.seven_bit_safe_flag = seven_bit_safe_flag
	 tfm.font.face = face
	 _print(string.format("SEVEN_BIT_SAFE_FLAG=%x",seven_bit_safe_flag))
	 local f,t = tfm.getface(face)
	 face = f 
	 _print(string.format("FACE=%s (mbl=%d,ri=%d,rce=%d)",face,t[1],t[2],t[3]))
      end
      local _pf = tfm.printfloat
      for current_char=bc,ec do
      --for current_char=102,102 do
	 local width_index,remainder,depth_index,height_index,tag,italic_index = tfm.getdata.char_info(current_char) 
	 tfm.chars[current_char] = tfm.chars[current_char] or {}
	 tfm.chars[current_char].width  = width[width_index]
	 tfm.chars[current_char].height = height[height_index]
	 tfm.chars[current_char].depth  = depth[depth_index]
	 tfm.chars[current_char].italic = italic[italic_index]
	 tfm.chars[current_char].tag    = tag
	 _print(string.format("O %o",current_char),string.char(current_char),
	       'WIDTH='.._pf(width[width_index]), 
	       'HEIGHT='.._pf(height[height_index]),
	       'DEPTH='.._pf(depth[depth_index]),
	       'ITALIC='.._pf(italic[italic_index]),
	       'TAG='..tfm.tag_meaning(tag)
	    )
	 if tag==1 then -- character has a ligature/kerning program
	    local kern_program = tfm.dump.kernprogram(remainder,current_char)
	 end
      end

      tfm.font.slant = param[1]
      _print("SLANT=".._pf(tfm.font.slant))

      tfm.font.space  = param[2]
      _print("SPACE=".._pf(tfm.font.space))

      tfm.font.space_stretch  = param[3]
      _print("SPACE_STRETCH=".._pf(tfm.font.space_stretch))

      tfm.font.space_shrink  = param[4]
      _print("SPACE_SHRINK=".._pf(tfm.font.space_shrink))

      tfm.font.x_height = param[5]
      _print("X_HEIGHT=".._pf(tfm.font.x_height))

      tfm.font.quad = param[6]
      _print("QUAD=".._pf(tfm.font.quad))

      tfm.font.extra_space = param[7]
      _print("EXTRA_SPACE=".._pf(tfm.font.extra_space))
      
      return true,'ok'

   end
--------------------------------------------------------------------------------

-- Add tfm to the mflua table
mflua.tfm = tfm 



--------------------------------------------------------------------------------
--
-- gf module
--
--------------------------------------------------------------------------------
--
-- In-memory parser, i.e. reads all the gf into memory
-- and then parses the byte stream
--

local GF =   {}
do 

   local sub	= string.sub
   local len	= string.len
   local byte	= string.byte
   local format	= string.format
   local rep     = string.rep

   local gfdata=''
   local gfdata_index
   local gfdata_len

   local chars={}
   local current_char
   local current_row  -- aka n
   local current_col  -- aka m
   local white=0
   local black=white+1
   local paint_switch
   chars.xxx1 = {}
   chars.xxx2 = {}
   chars.xxx3 = {}
   chars.xxx4 = {}
   chars.yyy  = {}
   chars.nop  = {}
   chars.all_nop = {} -- collect a xxx1 or xxx2 or xxx3 or xxx4 or yyy or nop
   chars.locators = {}




   local defDEBUG = false
   local function DEBUG(k,s)
      print("DEBUG:"..tostring(k).."="..tostring(s))
   end


   local function error_msg(msg)
      io.write(format("! mflua GF: char %s: %s",current_char, msg))
      io.write(format(" position=%s ",gfdata_index))
      print()
   end
   GF.error = error_msg

   local function warning_msg(msg)
      print(format("\n! mflua GF: char %s: %s.",current_char, msg))
   end
   GF.warning = warning_msg



   --
   -- read n bytes starting from from gfdata_index included
   -- and move gfdata_index
   --
   local function readbytes(n)
      if (gfdata_index+n-1)<1 then 
	 GF.warning("attempt to read before the beginning of the file")
	 return nil
      end
      if (gfdata_index+n-1)>gfdata_len then
	 GF.warning("attempt to read beyond the end of the file")
	 return nil
      end
      local s 
      if (n<0) then
	 s = sub(gfdata,gfdata_index+n+1,gfdata_index)
      elseif n>0 then
	 s = sub(gfdata,gfdata_index,gfdata_index+n-1)
      else
	 s =''
      end
      gfdata_index = gfdata_index+n
      return s
   end


   local function read4bytes()
      local b0 = byte(readbytes(1))
      local b1 = byte(readbytes(1))
      local b2 = byte(readbytes(1))
      local b3 = byte(readbytes(1))
      local v =  (b3+b2*256+b1*65536+b0*16777216)
      if v >2147483647 then
	 v = v -4294967296
      end
      return v
   end


   local function readrev4bytes()
      local b3 = byte(readbytes(-1))
      local b2 = byte(readbytes(-1))
      local b1 = byte(readbytes(-1))
      local b0 = byte(readbytes(-1))
      local v =  (b3+b2*256+b1*65536+b0*16777216)
      if v >2147483647 then
	 v = v -4294967296
      end
      return v
   end


   local function readuntileof()
      if gfdata_index>gfdata_len then 
	 GF.warning("wrong index: gfdata_index="..tostring(gfdata_index).."> gfdata_len="..tostring(gfdata_len) )
	 return nil
      end
      local s = sub(gfdata,gfdata_index,gfdata_len)
      gfdata_index = gfdata_len +1
      return s
   end


   local function datanotfinished()
      return (gfdata_index <= gfdata_len)
   end


   local function moveindex_after(p)
      gfdata_index = p+1
   end



   error = {}
   error.ok = 0
   error.base = error.ok
   error.pre	= error.base +1
   error.parse	= error.base +2
   error.post	= error.base +3
   error.boc       = error.base +4
   error.load	= error.base +127
   error.parse    	= error.base +128
   error.parsechar	= error.base +129
   error.paint	= {}
   for j=0,63 do
      error.paint[j]	= error.base +130+j
   end
   error.paint_switch=error.base +130+64
   error.paint1    = error.base +130+64+1
   error.paint2    = error.base +130+64+2
   error.paint3    = error.base +130+64+3
   error.eoc       = error.base +130+64+6
   error.skip0     = error.base +130+64+7
   error.skip1     = error.base +130+64+8
   error.skip2     = error.base +130+64+9
   error.skip3     = error.base +130+64+10
   error.new_row   = {}
   for j=0,164 do
      error.new_row[j]= error.base +130+64+10+1+j
   end

   error.post_post=error.base +130+64+10+1+165

   error.skip223=error.base +130+64+10+1+166


   error_name_t = {}
   for  k,v in pairs(error) do
      error_name_t[v] = k
   end
   local function error_name(res)
      if res==nil then
	 return "unknown error code"
      elseif error_name_t[res] == nil then
	 return "unknown error code"
      else
	 return error_name_t[res]
      end
   end

   local opcodes = {}

   -- {beginning of the \\{paint} commands}
   for j=0,63 do
      local s = format("paint_%d",j)
      opcodes[s] = j
   end
   opcodes.paint1=64	-- {move right a given number of columns, then   black${}\swap{}$white}
   opcodes.paint2=65
   opcodes.paint3=66
   opcodes.boc=67		-- {beginning of a character}
   opcodes.boc1=68		-- {short form of |boc|}
   opcodes.eoc=69		-- {end of a character}
   opcodes.skip0=70	-- {skip no blank rows}
   opcodes.skip1=71	-- {skip over blank rows}
   opcodes.skip2=72	
   opcodes.skip3=73
   --opcodes.new_row_0=74	-- {move down one row and then right}
   --:
   --opcodes.new_row_164=238
   for j=0,164 do
      local s = format("new_row_%d",j)
      opcodes[s] = j+74
   end

   --opcodes.max_new_row=164	-- {the largest \\{new\_row} command is |new_row_164|}
   opcodes.xxx1=239	-- {for \&{special} strings}
   opcodes.xxx2=240
   opcodes.xxx3=241	-- {for long \&{special} strings}
   opcodes.xxx4=242
   opcodes.yyy=243		-- {for \&{numspecial} numbers}
   opcodes.nop=244		-- no operation
   opcodes.char_loc=245	-- {character locators in the postamble}
   opcodes.char_loc0=246
   opcodes.pre=247		-- {preamble}
   opcodes.post=248	-- {postamble beginning}
   opcodes.post_post=249	-- {postamble ending} 

   opcodes.undefined_1=250
   opcodes.undefined_2=251
   opcodes.undefined_3=252
   opcodes.undefined_4=253
   opcodes.undefined_5=254
   opcodes.undefined_6=255

   local revopcode = {}
   for k,v in pairs(opcodes) do revopcode[v] = k end



   complement_paint_switch = function()
      local res = error.ok
      if( paint_switch==black or paint_switch==white) then
	 paint_switch=(paint_switch+1)%2
      else
	 GF.error("wrong value for paint_switch")
	 res = error.paint_switch
      end
      return res
   end

   local paint = function(n)
      local res = error.ok
      local c = chars[current_char]
      local row = c[current_row] 
      if row==nil then
	 GF.error("error in paint_"..tostring(n)..", row is nil")
      end
      if paint_switch==black then
	 for i=current_col,current_col+n-1 do
	    row[i]=true
	 end
      end
      res = complement_paint_switch()
      current_col = current_col + n
      if res~=error.ok then
	 if n<64 then
	    GF.error("error in paint_"..tostring(n))
	    res = error.paint[n]
	 elseif  (64<=n and n<256) then
	    GF.error("error in paint1")
	    res = error.paint1
	 elseif (256<=n and n<65536) then
	    GF.error("error in paint2")
	    res = error.paint2
	 elseif (65536<=n and n<16777216) then
	    GF.error("error in paint3")
	    res = error.paint3
	 end
      end
      return res
   end


   opcodes.func = {}


   opcodes.func[opcodes.paint_0]= function()
      local res = error.ok
      res = complement_paint_switch()
      if res~=error.ok then
	 GF.error("error in paint_0")
	 res = error.paint[0]
      end
      return res
   end

   --opcodes.func[opcodes.paint_1]= function() return paint(1) end
   --:
   --opcodes.func[opcodes.paint_63]= function() return paint(63) end
   for i=1, 63 do
      local key = format("paint_%d",i)
      local index = opcodes[key]
      opcodes.func[index] = function() return paint(i) end
   end 

   opcodes.func[opcodes.paint1]= function()
      local res = error.ok
      local b = byte(readbytes(1))
      if (64<=b and b<256) then
	 paint(b)
      else
	 GF.error("wrong value " ..tostring(b) .. " in paint1")
	 res = error.paint1
      end
      return res
   end


   opcodes.func[opcodes.paint2]= function()
      local res = error.ok
      local b1 = byte(readbytes(1))
      local b2 = byte(readbytes(1))
      local b = b2+b1*256
      if (256<=b and b<65536) then
	 paint(b)
      else
	 GF.error("wrong value " ..tostring(b) .. " in paint2")
	 res = error.paint2
      end
      return res
   end



   opcodes.func[opcodes.paint3]= function()
      local res = error.ok
      local b1 = byte(readbytes(1))
      local b2 = byte(readbytes(1))
      local b3 = byte(readbytes(1))
      local b  = b3+b2*256+ (b1*65536)
      if (65536<=b and b<16777216) then
	 paint(b)
      else
	 GF.error("wrong value " ..tostring(b) .. " in paint3")
	 res = error.paint3
      end
      return res
   end


   opcodes.func[opcodes.boc]= function()
      local pos = gfdata_index
      local c = read4bytes()
      local p = read4bytes()
      local min_m = read4bytes()
      local max_m = read4bytes()
      local min_n = read4bytes()
      local max_n = read4bytes()
      current_char = c
      chars[current_char] = chars[current_char] or {}
      chars[current_char]['p'] = p
      chars[current_char]['max_m'] = max_m
      chars[current_char]['min_m'] = min_m
      chars[current_char]['max_n'] = max_n
      chars[current_char]['min_n'] = min_n
      chars[current_char]['opened'] = true
      chars[current_char]['stream_pos'] = gfdata_index
      chars[current_char]['max_col'] = max_m
      chars[current_char]['min_col'] = min_m
      chars[current_char]['max_row'] = max_n
      chars[current_char]['min_row'] = min_n
      current_col = min_m
      current_row = max_n 
      chars[current_char][current_row]={}
      paint_switch = white
      return error.ok
   end


   opcodes.func[opcodes.boc1]= function()
      local pos = gfdata_index
      local c = byte(readbytes(1))
      local del_m = byte(readbytes(1))
      local max_m = byte(readbytes(1))
      local del_n = byte(readbytes(1))
      local max_n = byte(readbytes(1))
      local p = -1
      local min_m = max_m - del_m
      local min_n = max_n - del_n
      current_char = c
      chars[current_char] = chars[current_char] or {}
      chars[current_char]['p'] = p
      chars[current_char]['max_m'] = max_m
      chars[current_char]['min_m'] = min_m
      chars[current_char]['max_n'] = max_n
      chars[current_char]['min_n'] = min_n
      chars[current_char]['opened'] = true
      chars[current_char]['stream_pos'] = gfdata_index
      chars[current_char]['max_col'] = max_m
      chars[current_char]['min_col'] = min_m
      chars[current_char]['max_row'] = max_n
      chars[current_char]['min_row'] = min_n
      current_col = min_m
      current_row = max_n
      chars[current_char][current_row]={}
      paint_switch = white
      return error.ok
   end


   opcodes.func[opcodes.eoc]= function()
      local res = error.ok
      if chars[current_char].opened == true then
	 chars[current_char].opened = false
	 res = error.ok
      else
	 GF.error("error closing char")
	 res = error.eoc
      end
      --
      -- use them as stack to tie the nop 
      -- opcodes to current_char
      table.insert(chars.xxx1,{current_char,-1})
      table.insert(chars.xxx2,{current_char,-1})
      table.insert(chars.xxx3,{current_char,-1})
      table.insert(chars.xxx4,{current_char,-1})
      table.insert(chars.yyy,{current_char,-1})
      table.insert(chars.nop,{current_char,-1})
      table.insert(chars.all_nop,{'eoc',current_char,-1})
      return res  
   end


   opcodes.func[opcodes.skip0]= function()
      if chars[current_char]==nil or chars[current_char].min_m==nil then
	 GF.error("error in skip0")
	 return error.skip0
      end
      current_row = current_row -1 
      current_col = chars[current_char].min_m
      paint_switch = white
      chars[current_char][current_row]={}
      return error.ok
   end


   opcodes.func[opcodes.skip1]= function()
      if chars[current_char]==nil or chars[current_char].min_m==nil then
	 GF.error("error in skip1")
	 return error.skip1
      end
      local b = byte(readbytes(1))
      current_row = current_row -(b+1) 
      current_col = chars[current_char].min_m
      paint_switch = white
      chars[current_char][current_row]={}
      return error.ok
   end


   opcodes.func[opcodes.skip2]= function()
      if chars[current_char]==nil or chars[current_char].min_m==nil then
	 GF.error("error in skip2")
	 return error.skip2
      end
      local b0 = byte(readbytes(1))
      local b1 = byte(readbytes(1))
      local b =  b1+256*b0
      current_row = current_row - (b+1) 
      current_col = chars[current_char].min_m
      paint_switch = white
      chars[current_char][current_row]={}
      return error.ok
   end


   opcodes.func[opcodes.skip3]= function()
      if chars[current_char]==nil or chars[current_char].min_m==nil then
	 GF.error("error in skip3 ")
	 return error.skip3
      end
      local b0 = byte(readbytes(1))
      local b1 = byte(readbytes(1))
      local b2 = byte(readbytes(1))
      local b =  b2+b1*256+b3*65536
      current_row = current_row - (b+1)
      current_col = chars[current_char].min_m
      paint_switch = white
      chars[current_char][current_row]={}
      return error.ok
   end


   local function new_row(n)
      if chars[current_char]==nil or chars[current_char].min_m==nil then
	 GF.error("error in new_row_"..n)
	 return error.new_row[n]
      end
      current_row = current_row - 1
      current_col  = chars[current_char].min_m+n
      paint_switch = black
      chars[current_char][current_row]={}
      return error.ok
      
   end

   opcodes.func[opcodes.new_row_0]= function()
      if chars[current_char]==nil or chars[current_char].min_m==nil then
	 GF.error("error in new_row_0")
	 return error.new_row[0]
      end
      current_row = current_row - 1
      current_col  = chars[current_char].min_m
      paint_switch = black
      chars[current_char][current_row]={}
      return error.ok
   end

   --opcodes.func[opcodes.new_row_1]= function() new_row(1) end
   --:
   --opcodes.func[opcodes.new_row_164]= function() new_row(i)end
   for i=1, 164 do
      local key = format("new_row_%d",i)
      local index = opcodes[key]
      opcodes.func[index] = function() return new_row(i) end
   end 

   --opcodes.func[opcodes.max_new_row]= function()
   --   return new_row(opcodes.max_new_row)
   --end

   opcodes.func[opcodes.xxx1]= function()
      local stream_pos = gfdata_index
      local b = byte(readbytes(1))
      local k = readbytes(b)
      table.insert(chars.xxx1,{k,stream_pos})
      table.insert(chars.all_nop,{'xxx1',k,stream_pos})
      return error.ok
   end

   opcodes.func[opcodes.xxx2]= function()
      local stream_pos = gfdata_index
      local b0 = byte(readbytes(1))
      local b1 = byte(readbytes(1))
      local k = readbytes(b1+ b0*256)
      table.insert(chars.xxx2,{k,stream_pos})
      table.insert(chars.all_nop,{'xxx2',k,stream_pos})
      return error.ok
   end


   opcodes.func[opcodes.xxx3]= function()
      local stream_pos = gfdata_index
      local b0 = byte(readbytes(1))
      local b1 = byte(readbytes(1))
      local b2 = byte(readbytes(1))
      local k = readbytes(b2+ b1*256+b0*65536)
      table.insert(chars.xxx3,{k,stream_pos})
      table.insert(chars.all_nop,{'xxx3', k,stream_pos})
      return error.ok
   end

   opcodes.func[opcodes.xxx4]= function()
      local stream_pos = gfdata_index
      local k = readbytes(read4bytes())
      -- k must be positive
      if k<=0 then
	 k = k +4294967296
      end
      table.insert(chars.xxx4,{k,stream_pos})
      table.insert(chars.all_nop,{'xxx4',k,stream_pos})
      return error.ok
   end

   opcodes.func[opcodes.yyy]= function()
      local stream_pos = gfdata_index
      local v =  read4bytes()
      table.insert(chars.yyy,{v,stream_pos})
      table.insert(chars.all_nop,{'yyy',v,stream_pos})
      return error.ok
   end

   opcodes.func[opcodes.nop]= function()
      table.insert(chars.nop,gfdata_index)
      table.insert(chars.all_nop,{'nop','',gfdata_index})
      return error.ok
   end

   opcodes.func[opcodes.char_loc]= function()
      local c	=  byte(readbytes(1))
      local dx	=  read4bytes()
      local dy	=  read4bytes()
      local w	=  read4bytes()
      local p	=  read4bytes()
      local char_locator = {}
      char_locator['c']	= c 
      char_locator['dx']	= dx
      char_locator['dy']	= dy
      char_locator['w']	= w
      char_locator['p']	= p
      table.insert(chars.locators,char_locator)
      return error.ok
   end


   opcodes.func[opcodes.char_loc0]= function()
      local c	= byte(readbytes(1))
      local dm	= byte(readbytes(1))
      local w	= read4bytes()
      local p	= read4bytes()
      local dy	= 0
      local dx	= 65536*dm
      local char_locator = {}
      char_locator['c']	= c 
      char_locator['dx']	= dx
      char_locator['dy']	= dy
      char_locator['w']	= w
      char_locator['p']	= p
      table.insert(chars.locators,char_locator)
      return error.ok
   end


   opcodes.func[opcodes.pre]= function()
      local i	=  byte(readbytes(1))
      local k	=  byte(readbytes(1))
      local x	=  readbytes(k)
      chars.GF_format	= i
      chars.GF_comment	= x
      return error.ok
   end

   opcodes.func[opcodes.post]= function()
      local p	=  read4bytes()
      local ds	=  read4bytes()
      local cs	=  read4bytes()
      local hppp	=  read4bytes()
      local vppp	=  read4bytes()
      local min_m  =  read4bytes()
      local max_m  =  read4bytes()
      local min_n  =  read4bytes()
      local max_n  =  read4bytes()
      chars['ds']		= ds
      chars['cs']		= cs
      chars['hppp']	= hppp
      chars['vppp']	= vppp
      chars['min_m']	=  min_m
      chars['max_m']	=  max_m
      chars['min_n']	=  min_n
      chars['max_n']	=  max_n
      return error.ok
   end

   opcodes.func[opcodes.post_post]= function()
      local q	=  read4bytes()
      local i      =  byte(readbytes(1))
      local pad223 =  readbytes(4)
      if ( pad223~='\223\223\223\223' ) then
	 GF.error("error post_post_1 "..pad223)
	 return error.post_post
      end
      if gfdata_index <=gfdata_len then 
       local s = readuntileof()
       local ctr223 = rep('\223',len(s))
        if ctr223 ~= s then
	 GF.error("error post_post_2 "..pad223)
	 return error.post_post
	end
      end	 
      chars.GF_format_post = i
      return error.ok
   end

   opcodes.func[opcodes.undefined_1]= function()
      GF.warning("undefined command 1")
      return error.ok
   end
   opcodes.func[opcodes.undefined_2]= function()
      GF.warning("undefined command 2")
      return error.ok
   end
   opcodes.func[opcodes.undefined_3]= function()
      GF.warning("undefined command 3")
      return error.ok
   end
   opcodes.func[opcodes.undefined_4]= function()
      GF.warning("undefined command 4")
      return error.ok
   end
   opcodes.func[opcodes.undefined_5]= function()
      GF.warning("undefined command 5")
      return error.ok
   end
   opcodes.func[opcodes.undefined_6]= function()
      GF.warning("undefined command 6")
      return error.ok
   end



   --------------------------------------------------------------------------------
   --
   --------------------------------------------------------------------------------

   local function load(gffile)
      gfdata = ''
      local f,res = io.open(gffile,'rb')
      if f==nil then
	 GF.error(res)
	 gfdata_index=0
	 return error.load
      else 
	 gfdata		= f:read("*a")
	 gfdata_len		= len(gfdata)
	 gfdata_index	=1 
      end  
      f:close()
      return  error.ok
   end


   local function parse_pre()
      gfdata_index=1 
      local res = error.ok
      local b = byte(readbytes(1))
      if b==opcodes.pre then
	 local i=byte(readbytes(1))
	 local k=byte(readbytes(1))
	 local x=readbytes(k)
	 chars.GF_format=i
	 chars.comments=x
      else 
	 GF.error("error parsing pre")
	 res = error.pre
      end
      return res
   end


   local function parse_rest()
      local b = byte(readbytes(1))
      local func = opcodes.func
      local res = error.ok
      local cond = true
      while (cond) do
	 if func[b] then
	    res = (func[b])()
	    if res~=error.ok then
	       cond = false  
	    elseif datanotfinished() then  
	       b = byte(readbytes(1))
	    else
	       res = error.ok
	       cond = false
	    end
	 else
	    cond = false
	    GF.error("wrong opcode while parsing char")  
	    res = error.parsechar
	 end
      end   
      return res
   end


   local function  parse()
      if gfdata_len == 0 then  
	 GF.error("no data")
	 return error.parse
      end
      local res 
      res = parse_pre()
      if res~=error.ok then
	 GF.error("wrong preamble")
	 return res
      end
      res = parse_rest()
      if not(res==error.ok) then
	 GF.error("error while reading characters")
	 return res
      end
      return res  
   end


   ----------------------------------------------------------------------------------
   -- Parse from the end 
   ----------------------------------------------------------------------------------

   local function skip223()
      local i = gfdata_len
      local cond = true
      while cond do
	 local c = byte(sub(gfdata,i,i))
	 if c==223 then
	    i=i-1
	 else
	    cond=false
	 end
      end
      if i<3 then
	 GF.error("error skip suffix ")
	 res = error.skip223
      else
	 res = error.ok
	 gfdata_index =  i
      end
      return res
   end


   local function parse_char_fromlocator()
      local cond = true
      local func = opcodes.func
      local b,res
      while cond do
	 b = byte(readbytes(1))
	 if func[b] then
	    res = (func[b])()
	    if res ~= error.ok then
	       cond = false 
	       return res
	    elseif b==opcodes.eoc then
	       cond = false
	    else
	       cond = true 
	    end
	 else
	    GF.error("wrong char from locator")
	    cond = false
	    return error.parse
	 end
      end
      if chars[current_char] and chars[current_char].p == -1 then
	 return error.ok
      end
      moveindex_after(chars[current_char].p)
      return parse_char_fromlocator()
      
   end

   local function parse_from_end()
      if gfdata_len == 0 then  
	 GF.error("no data")
	 return error.parse
      end
      local res = error.ok
      gfdata_index = gfdata_len
      res = skip223()
      if res ~= error.ok then
	 return res
      end
      --
      -- read identification byte
      --
      chars.GF_format_post = byte(readbytes(-1))
      --
      -- read post_post
      -- 
      local q = readrev4bytes()
      if q<=0 then
	 q = q +4294967296
      end
      if q<1 or q>gfdata_len then
	 GF.error("wrong pointer to post")
	 return error.parse
      end
      --
      -- reading post
      -- 
      moveindex_after(q)
      q = byte(readbytes(1))
      if q~=opcodes.post then
	 GF.error("expected  post opcode")
	 return error.parse
      end
      --
      res = opcodes.func[opcodes.post]()
      if res~=error.ok then
	 GF.error("reading postamble")
	 return error.parse
      end
      --
      -- read char locators 
      --
      local cond		= true
      local char_loc	= opcodes.char_loc
      local char_loc0	= opcodes.char_loc0
      local post_post	= opcodes.post_post
      local f		= opcodes.func
      while cond do
	 q = byte(readbytes(1))
	 if q==char_loc or q==char_loc0 then
	    f[q]()
	 elseif q==post_post then
	    cond = false
	    res = error.ok
	 else
	    GF.error("reading char locators")
	    cond = false 
	    return error.parse
	 end
      end
      --
      -- read  chars
      --
      if #chars.locators == 0 then
	 GF.error("no char locators")
	 return error.parse
      end
      for i,v in ipairs(chars.locators) do
	 local p  = v.p
	 if p>-1 then
	    moveindex_after(p)
	    res = parse_char_fromlocator()
	    if res ~= error.ok then
	       break;
	    end
	 end
      end
      return res
   end


   --------------------------------------------------------------------------------

   -- Already assigned before:
   -- GF.warning = warning_msg
   -- GF.error = error_msg
   GF.load		= load
   GF.parse	= parse
   GF.chars        = chars
   GF.errorcode    = error
   GF.error_name   = error_name
   GF.parse_from_end = parse_from_end
   --
   -- TODO: parsing by reading a file
   -- 


end
-- Add GF to the mflua table
mflua.GF = GF
