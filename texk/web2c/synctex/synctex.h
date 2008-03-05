/* synctex.h

This file is part of the SyncTeX package.

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE

*/

# ifndef __SYNCTEX_HEADER__
#  define __SYNCTEX_HEADER__

/*  Send this message when starting a new input.  */
extern void synctexstartinput(void);

/*  Recording the "s:..." line.  In *tex.web, use synctex_sheet(pdf_output) at
 *  the very beginning of the ship_out procedure.
*/
extern void synctexsheet(integer pdf_output);

/*  Send this message when an hlist will be shipped out, more precisely at
 *  the beginning of the hlist_out procedure in *TeX.web.  It must be balanced
 *  by a synctex_tsilh, sent at the end of the hlist_out procedure.  p is the
 *  address of the hlist. */
extern void synctexhlist(halfword p);

/*  Sent this message at the end of the hlist_out procedure in *TeX.web
 *  to balance a former synctex_hlist.    */
extern void synctextsilh(halfword p);

/*  Send this message whenever an inline math node will ship out. */
extern void synctexmath(halfword p);

/*  Send this message whenever a glue node will ship out. */
extern void synctexglue(halfword p);

/*  Send this message whenever a kern node will ship out. */
extern void synctexkern(halfword p);

/*  Send this message to clean memory.  */
extern void synctex_terminate(void);

# endif