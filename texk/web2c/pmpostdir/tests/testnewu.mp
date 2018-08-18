prologues:=0;
beginfig(1);
  u:=.5cm; h:=0; v:=0;
  picture pct; string FONT;
  bboxmargin:=-0pt;
  def TEST(expr a)=
    pct:=(a if length(FONT)>0: infont FONT fi);
    draw bbox pct shifted (h,v) withcolor (blue+green); 
	draw pct shifted (h,v);
	h:=h+xpart(urcorner pct-ulcorner pct); v:=v+0.1u;
  enddef;
  
  FONT:="cmr10";
  TEST("VAr10");
  h:=0; v:=v+u; FONT:="cmss10";
  TEST("VAss10");
  h:=0; v:=v+u; FONT:="uprml-h";
  TEST("あ，臼𦥑あ"); % upjisr-h: error
  h:=0; v:=v+u;
  TEST("あ" );
  TEST("，" );
  TEST("臼" );
  TEST("𦥑" );
  TEST("あ" );
  h:=0; v:=v+u; FONT:="testnewu";
  TEST("あ，臼𦥑あ");
  h:=0; v:=v+u;
  TEST("あ" );
  TEST("，" );
  TEST("臼" );
  TEST("𦥑" );
  TEST("あ" );
  h:=0; v:=v+u; FONT:="";
  TEST(btex あ，臼𦥑あ etex);
endfig;
end;
