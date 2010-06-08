# convert long labels to a shorter form so lex(1) won't overflow
s/LaBegin/SA/g
s/LaDisplay/SB/g
s/LaEnd/SC/g
s/LaEnv/SD/g
s/LaFormula/SE/g
s/LaInclude/SF/g
s/LaMacro2/SG/g
s/LaMacro/SH/g
s/LaVerbatim/SI/g
s/Define/SJ/g
s/Display/SK/g
s/IncludeOnly/SL/g
s/\([ <]\)Input\([ >;]\)/\1SM\2/g
s/Math/SN/g
s/Normal/SO/g
s/Control/SP/g
