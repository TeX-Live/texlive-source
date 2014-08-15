/* manifests.h: Various constants used everywhere.

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous

Omega is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Omega is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Omega; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

*/

/* Character measures */

#define C_MIN 		 0

#define C_WD  		 0
#define C_HT  		 1 
#define C_DP  		 2
#define C_IC  		 3

#define C_SECWD 	 4
#define C_SECHT  	 5 
#define C_SECDP  	 6
#define C_SECIC  	 7

#define C_P_TOPAXIS	 8
#define C_P_TOPAXISBIs	 9
#define C_P_BOTAXIS	10
#define C_P_BOTAXISBIS	11
#define C_P_MIDHOR	12
#define C_P_MIDVERT	13
#define C_P_BASESLANT	14

#define C_S_TOPAXIS	16
#define C_S_TOPAXISBIs	17
#define C_S_BOTAXIS	18
#define C_S_BOTAXISBIS	19
#define C_S_MIDHOR	20
#define C_S_MIDVERT	21
#define C_S_BASESLANT	22

#define C_MAX		22

/* Extensible pieces */

#define E_MIN  0

#define E_TOP  0
#define E_MID  1
#define E_BOT  2
#define E_REP  3

#define E_MAX  3

/* Ligature commands */

#define L_MIN    0

#define L_0      0	/*   LIG     */
#define L_B      1	/*   LIG/    */
#define L_A      2	/*  /LIG     */
#define L_AB     3	/*  /LIG/    */
#define L_Bx     5	/*   LIG/>   */
#define L_Ax     6	/*  /LIG>    */
#define L_ABx    7	/*  /LIG/>   */
#define L_ABxx  11	/*  /LIG/>>  */

#define L_MAX	11

/* Lig/Kern activities */
#define A_UNREACHABLE   0	/* a program step not known to be reachable */
#define A_PASS_THROUGH  1	/* a program step passed through on initialization */
#define A_ACCESSIBLE    2	/* a program step that can be relevant */

/* Xerox faces */

#define F_MIN   0

#define F_MRR   0
#define F_MIR   1
#define F_BRR   2
#define F_BIR   3
#define F_LRR   4
#define F_LIR   5
#define F_MRC   6
#define F_MIC   7
#define F_BRC   8
#define F_BIC   9
#define F_LRC  10
#define F_LIC  11
#define F_MRE  12
#define F_MIE  13
#define F_BRE  14
#define F_BIE  15
#define F_LRE  16
#define F_LIE  17

#define F_MAX  17

/* Named parameters */

#define P_MIN		 1

#define P_SLANT		 1
#define P_SPACE		 2
#define P_STRETCH	 3
#define P_SHRINK	 4
#define P_XHEIGHT	 5
#define P_QUAD		 6
#define P_EXTRASPACE	 7

#define P_MAX		 7

/* Named math symbol parameters */

#define P_MATHSY_MIN	 8

#define P_NUM1		 8
#define P_NUM2		 9
#define P_NUM3		10
#define P_DENOM1	11
#define P_DENOM2	12
#define P_SUP1		13
#define P_SUP2		14
#define P_SUP3		15
#define P_SUB1		16
#define P_SUB2		17
#define P_SUPDROP	18
#define P_SUBDROP	19
#define P_DELIM1	20
#define P_DELIM2	21
#define P_AXISHEIGHT	22

#define P_MATHSY_MAX	22

/* Named math extension parameters */

#define P_MATHEX_MIN		 8

#define P_DEFAULTRULETHICKNESS	 8
#define P_BIGOPSPACING1		 9
#define P_BIGOPSPACING2		10
#define P_BIGOPSPACING3		11
#define P_BIGOPSPACING4		12
#define P_BIGOPSPACING5		13

#define P_MATHEX_MAX		13

/* Typesetting directions */

#define DIR_MIN  0

#define DIR_ORD  0
#define DIR_NAT  8

#define DIR_TL   0
#define DIR_LT   1
#define DIR_TR   2
#define DIR_LB   3
#define DIR_BL   4
#define DIR_RT   5
#define DIR_BR   6
#define DIR_RB   7

#define DIR_MAX  15

/* Kinds of accent */

#define ACC_NONE 0
#define ACC_TOP  1
#define ACC_MID  2
#define ACC_BOT  3

/* Move directions */

#define M_MIN    0

#define M_RIGHT  0
#define M_LEFT   1
#define M_UP     2
#define M_DOWN   3

#define M_MAX    3

/* Rule measures */

#define RULE_MIN 0

#define RULE_WD  0
#define RULE_HT  1
#define RULE_DP  2

#define RULE_MAX 2

/* Glue shrink or stretch */

#define GLUE_MIN 0

#define GLUE_SHRINK  0
#define GLUE_STRETCH 1

#define GLUE_MAX 1

/* Glue orders */

#define O_MIN	0

#define O_UNIT	0
#define O_FI	1
#define O_FIL	2
#define O_FILL	3
#define O_FILLL	4

#define O_MAX	4


/* Glue kinds */

#define K_MIN      0

#define K_NORMAL   0
#define K_ALEADERS 1
#define K_CLEADERS 2
#define K_XLEADERS 3

#define K_MAX      3


/* Font types */

#define FT_MIN		0

#define FT_VANILLA	0
#define FT_MATHSY	1
#define FT_MATHEX	2

#define FT_MAX		2


#define UNITY     0x100000
#define WEB_INFINITY  0x7fffffff

#define LEN_CODING_SCHEME  39
#define LEN_FAMILY         19
#define LEN_VTITLE        255
#define LEN_FONT_NAME     255
#define LEN_FONT_AREA     255

#define TAG_NONE  0
#define TAG_LIG   1
#define TAG_LIST  2
#define TAG_EXT   3

#define CHAR_MINIMUM	0x0
#define CHAR_MAXIMUM	0x7fffffff
#define CHAR_BOUNDARY	0x80000000
#define CHAR_ERROR	0x80000001

#define LIG_SIMPLE      0       /* f(x,y) = z                 */
#define LIG_LEFT_Z      1       /* f(x,y) = f(z,y)            */
#define LIG_RIGHT_Z     2       /* f(x,y) = f(x,z)            */
#define LIG_BOTH_Z      3       /* f(x,y) = f(f(x,z),y)       */
#define LIG_PENDING     4       /* f(x,y) is being evaluated  */

#define GLUEARG_NONE    0
#define GLUEARG_CHAR    1
#define GLUEARG_RULE    2

#define OFM_NOLEVEL	0
#define OFM_TFM		1
#define OFM_LEVEL0	2
#define OFM_LEVEL1	3
#define OFM_LEVEL2	4
