prologues:=3;
outputformat:="eps";
outputtemplate:="%j.%o";
beginfig(1);
  u=1cm;
  drawarrow (-.5u,0)--(4u,0);
  drawarrow (0,-.5u)--(0,2u);
  pickup pencircle scaled 1pt;
  draw (0,0){up}
    for i=1 upto 8: ..(i/2,sqrt(i/2))*u endfor;
  label.lrt(btex 関数$y=\sqrt{x}$ etex, (3,sqrt(3))*u);
  label.rt(btex $x$軸 etex, (4,0)*u);
  label.top(btex $y$軸 etex, (0,2)*u);
  label.llft(btex 原点$O$ etex, (0,0));
endfig;
end.
