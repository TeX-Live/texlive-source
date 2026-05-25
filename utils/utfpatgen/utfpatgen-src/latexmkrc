# This file is used for generating the PDF report in Overleaf
# in other cases use Makefile

system("cweave utfpatgen.w");
system("cp utfpatgen.idx output.idx");
system("cp utfpatgen.scn output.scn");

# Redirect pdflatex to pdftex
$pdflatex = 'pdftex %O %S';