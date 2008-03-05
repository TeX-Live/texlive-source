This file contains XeTeX changes for synctex support,
closely based on a patch to pdftex.web by Jérôme Laurens.

******************************************************************************
Permission is hereby granted, free of charge, to any person obtaining  
a copy of this software and associated documentation files (the  
"Software"), to deal in the Software without restriction, including  
without limitation the rights to use, copy, modify, merge, publish,  
distribute, sublicense, and/or sell copies of the Software, and to  
permit persons to whom the Software is furnished to do so, subject to  
the following conditions:

The above copyright notice and this permission notice shall be  
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,  
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF  
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND  
NONINFRINGEMENT. IN NO EVENT SHALL SIL INTERNATIONAL BE LIABLE FOR  
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION  
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of SIL International  
shall not be used in advertising or otherwise to promote the sale,  
use or other dealings in this Software without prior written  
authorization from SIL International.
******************************************************************************

@x
@d synctex_field_size=MISSING IMPLEMENTATION
@y
@d synctex_field_size=1 {Declare the {\sl synctex} field size to store the {\sl synctex} information:
                         we will put file tag and line into lh and rh fields of one word}
@z

@x
    MISSING IMPLEMENTATION: 0 synctag for medium sized node at mem[t]
@y
    mem[t+medium_node_size-synctex_field_size].hh.lh:=0; {{\sl synctex}: do nothing, it is too late}
@z

@x
MISSING IMPLEMENTATION: 0 synctag for medium sized node at mem[p]
@y
mem[p+medium_node_size-synctex_field_size].hh.lh:=0; {{\sl synctex}: do nothing, it is too late}
@z

@x
  MISSING IMPLEMENTATION: update the synctex information for node at mem[r] with size s
@y
  mem[r+s-synctex_field_size].hh.lh := synctex_tag;
  mem[r+s-synctex_field_size].hh.rh := line;
@z

@x
MISSING IMPLEMENTATION: copy the synctex information for box node at mem[r] from node at mem[p]
@y
mem[r+box_node_size-synctex_field_size].hh.lh:=mem[p+box_node_size-synctex_field_size].hh.lh;
mem[r+box_node_size-synctex_field_size].hh.rh:=mem[p+box_node_size-synctex_field_size].hh.rh;
@z

@x
MISSING IMPLEMENTATION: copy the synctex information for glue node at mem[r] from node at mem[p]
@y
mem[r+medium_node_size-synctex_field_size].hh.lh:=mem[p+medium_node_size-synctex_field_size].hh.lh;
mem[r+medium_node_size-synctex_field_size].hh.rh:=mem[p+medium_node_size-synctex_field_size].hh.rh;
@z
