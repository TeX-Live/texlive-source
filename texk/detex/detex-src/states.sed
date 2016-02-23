# Copyright (c) 1986-2007 Purdue University
# All rights reserved.
# 
# Developed by:  Daniel Trinkle
#                Department of Computer Science, Purdue University
#                http://www.cs.purdue.edu/
# 
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal with the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# o Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimers.
# 
# o Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimers in the
#   documentation and/or other materials provided with the distribution.
# 
# o Neither the names of Daniel Trinkle, Purdue University, nor the
#   names of its contributors may be used to endorse or promote products
#   derived from this Software without specific prior written
#   permission.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR
# ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
# CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.
#
#
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
s/LaPicture/SQ/g
s/LaBreak/SR/g
s/LaOptArg2/SS/g
s/LaOptArg/ST/g
