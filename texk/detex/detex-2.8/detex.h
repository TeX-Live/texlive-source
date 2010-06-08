/*
 * Copyright (c) 1986-2007 Purdue University
 * All rights reserved.
 * 
 * Developed by:  Daniel Trinkle
 *                Department of Computer Science, Purdue University
 *                http://www.cs.purdue.edu/
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal with the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * o Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimers.
 * 
 * o Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimers in the
 *   documentation and/or other materials provided with the distribution.
 * 
 * o Neither the names of Daniel Trinkle, Purdue University, nor the
 *   names of its contributors may be used to endorse or promote products
 *   derived from this Software without specific prior written
 *   permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.
 */

/******
** The following parameters should be modified as necessary
**	MAXINCLIST - maximum number of files allowed in an \includeonly list
**
**	DEFAULTINPUTS - this should be the same as the default TEXINPUTS
**	CHPATHSEP - the path separator character in TEXINPUTS
**	MAXINPUTPATHS - (arbitrary) number of separate paths in TEXINPUTS
**
**	DEFAULTENV - list of LaTeX environments ignored
**	CHENVSEP - the list separator character in the ignore envronment list
**	MAXENVS - maximum number of environments listed in the ignore list
**	CCHMAXENV - maximum count of characters in an environment name (LaTex)
******/

#define	MAXINCLIST	40

#ifdef OS2
#define	DEFAULTINPUTS	".;/emtex/texinput"
#define	CHPATHSEP	';'
#else
#define	DEFAULTINPUTS	".:/usr/local/tex/inputs"
#define	CHPATHSEP	':'
#endif
#define	MAXINPUTPATHS	10

#define	DEFAULTENV	"array,eqnarray,equation,figure,mathmatica,picture,table,verbatim"
#define	CHENVSEP	','
#define	MAXENVS		10
#define	CCHMAXENV	100

/******
** These probably should not be changed
******/

#define	CHOPT		'-'
#define	CHCITEOPT	'c'
#define	CHENVOPT	'e'
#define	CHLATEXOPT	'l'
#define	CHNOFOLLOWOPT	'n'
#define	CHSPACEOPT	's'
#define	CHTEXOPT	't'
#define	CHWORDOPT	'w'

#define	ERROR		-1
