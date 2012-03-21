/* Copyright (c) 1987, 1989, 2012 University of Maryland Department of
   Computer Science.
   
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation, the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions: The above copyright notice, this permission
   notice and the disclaimer statement shall be included in all copies
   or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/* search structures and routines for 32-bit key, arbitrary data */

struct search {
	unsigned s_dsize;	/* data object size (includes key size) */
	unsigned s_space;	/* space left (in terms of objects) */
	unsigned s_n;		/* number of objects in the table */
	char	*s_data;	/* data area */
};

/* returns a pointer to the search table (for future search/installs) */
struct	search *SCreate(unsigned int dsize);	/* create a search table */

/* returns a pointer to the data object found or created */
char	*SSearch(struct search *s, i32 key, int *disp);		/* search for a data object */

/* clears a search table (wipes out all entries) */
#define	SClear(s)	((s)->s_space += (s)->s_n, (s)->s_n = 0)

/* the third argument to SSearch controls operation as follows: */
#define	S_LOOKUP	0x00	/* pseudo flag */
#define	S_CREATE	0x01	/* create object if not found */
#define	S_EXCL		0x02	/* complain if already exists */

/* in addition, it is modified before return to hold status: */
#define	S_COLL		0x04	/* collision (occurs iff S_EXCL set) */
#define	S_FOUND		0x08	/* found (occurs iff existed already) */
#define	S_NEW		0x10	/* created (occurs iff S_CREATE && !S_EXCL) */
#define	S_ERROR		0x20	/* problem creating (out of memory) */

void SEnumerate(struct search *s, void(*f)(char*,i32));
