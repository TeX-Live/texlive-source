if(!settings.multipleView)
 settings.batchView=false;
defaultfilename="latexusage-2";
if(settings.render < 0) settings.render=4;
settings.inlineimage=true;
settings.embed=true;
settings.outformat="";
settings.toolbar=false;
viewportmargin=(2,2);

// Global Asymptote definitions can be put here.
import three;
usepackage("bm");
texpreamble("\def\V#1{\bm{#1}}");
// One can globally override the default toolbar settings here:
// settings.toolbar=true;


currentprojection=orthographic(5,4,2);
draw(unitcube,blue);
label("$V-E+F=2$",(0,1,0.5),3Y,blue+fontsize(17pt));
size(0,4cm);
viewportsize=(390.0pt,0);
