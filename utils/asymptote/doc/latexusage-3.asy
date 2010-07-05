if(!settings.multipleView)
 settings.batchView=false;
defaultfilename="latexusage-3";
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


pair z0=(0,0);
pair z1=(2,0);
pair z2=(5,0);
pair zf=z1+0.75*(z2-z1);

draw(z1--z2);
dot(z1,red+0.15cm);
dot(z2,darkgreen+0.3cm);
label("$m$",z1,1.2N,red);
label("$M$",z2,1.5N,darkgreen);
label("$\hat{\ }$",zf,0.2*S,fontsize(24pt)+blue);

pair s=-0.2*I;
draw("$x$",z0+s--z1+s,N,red,Arrows,Bars,PenMargins);
s=-0.5*I;
draw("$\bar{x}$",z0+s--zf+s,blue,Arrows,Bars,PenMargins);
s=-0.95*I;
draw("$X$",z0+s--z2+s,darkgreen,Arrows,Bars,PenMargins);
size(390.0pt,0);
viewportsize=(390.0pt,0);
