%% tests/mpclip.mp
%%
%% Regression test for missing "n" after the clipping operator "W".
%% Compile: mpost -s 'outputtemplate="%j.mps"' mpclip
%%
%% The clip is followed by setrgbcolor, not newpath, so without
%% the fix the PDF has "h W 1 0 0 RG ..." (W not followed by n).
%%
prologues := 0;
beginfig(1);
  draw (0,0)--(100,100) withcolor red;
  clip currentpicture to (0,0)--(100,0)--(50,100)--cycle;
endfig;
end
