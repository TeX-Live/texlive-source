%{
/* pl-parser.y: Grammar for reading property list files

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

#include "cpascal.h"
#include "parser.h"
#include "pl-parser.h"
#include "manifests.h"
#include "list_routines.h"
#include "error_routines.h"
#include "out_routines.h"
#include "char_routines.h"
#include "header_routines.h"
#include "param_routines.h"
#include "dimen_routines.h"
#include "ligkern_routines.h"
#include "print_routines.h"
#include "font_routines.h"
#include "extra_routines.h"

%}

/* Basic tokens */

%token LEFT
%token RIGHT
%token NUMBER
%token FIX
%token COMMENT

/* Tokens found in PL, OPL, VF and OVF files */

%token CHECKSUM
%token DESIGNSIZE
%token DESIGNUNITS
%token CODINGSCHEME	/* implicit string */
%token FAMILY		/* implicit string */
%token FACE
%token SEVENBITSAFEFLAG	/* implicit 0 (FALSE) or 1 (TRUE) */
%token HEADER
%token FONTDIMEN
%token LIGTABLE
%token BOUNDARYCHAR
%token CHARACTER

%token NAMEDPARAMETER	/* implicit integer */
%token PARAMETER

%token CHARMEASURE	/* implicit integer */
%token NEXTLARGER
%token VARCHAR
%token EXTEN		/* implicit integer */

%token LABEL
%token LIG		/* implicit integer */
%token KRN
%token STOP
%token SKIP

/* Tokens found only in VF and OVF files */

%token VTITLE

%token MAPFONT
%token FONTNAME
%token FONTAREA
%token FONTCHECKSUM
%token FONTAT
%token FONTDSIZE

%token MAP
%token SELECTFONT
%token SETCHAR
%token SETRULE
%token MOVE
%token PUSH
%token POP
%token SPECIAL
%token SPECIALHEX

/* Tokens found only in OPL and OVF files */

%token CHARREPEAT
%token FONTIVALUE
%token FONTFVALUE
%token FONTMVALUE
%token FONTPENALTY
%token FONTRULE
%token FONTGLUE

%token CLABEL
%token CKRN
%token CGLUE
%token CPENALTY
%token CPENGLUE

%token CHARIVALUE
%token CHARFVALUE
%token CHARMVALUE
%token CHARPENALTY
%token CHARRULE
%token CHARGLUE

%token IVALUE
%token IVALUEVAL

%token MVALUE
%token MVALUEVAL

%token FVALUE
%token FVALUEVAL

%token PENALTY
%token PENALTYVAL

%token RULE
%token RULEMEASURE

%token GLUE
%token GLUEWD
%token GLUETYPE
%token GLUEKIND		/* Implicit integer */
%token GLUERULE
%token GLUECHAR
%token GLUESHRINKSTRETCH
%token GLUEORDER	/* Implicit integer */


%token OFMLEVEL
%token FONTDIR
%token ACCENT

%%

File:
	Entries
;
	
Entries :
	/* */
|	Entries
	LEFT OneEntry RIGHT
;

OneEntry :

/* The following entries are found in PL, OPL, VP and OVP files */

	CHECKSUM NUMBER
		{ set_check_sum($2.yint); }
|	DESIGNSIZE FIX
		{ set_design_size($2.yfix); }
|	DESIGNUNITS FIX
		{ set_design_units($2.yfix); }
|	CODINGSCHEME
		{ set_coding_scheme($1.ystring); }
|	FAMILY
		{ set_family($1.ystring); }
|	FACE NUMBER
		{ set_face($2.yint); }
|	SEVENBITSAFEFLAG
		{ set_seven_bit_safe_flag($1.yint); }
|	HEADER NUMBER NUMBER
		{ set_header_word($2.yint, $3.yint); }
|	FONTDIMEN
		{ init_parameters(); }
	FontParameters
|	LIGTABLE
		{ init_ligkern(); }
	LigKernEntries
|	BOUNDARYCHAR NUMBER
		{ set_boundary_character($2.yint); }
|	CHARACTER NUMBER
		{ init_character($2.yint, NULL); }
	CharacterEntries
|	COMMENT 

/* The following entries are found only in VP and OVP files */

|	VTITLE
		{ set_vtitle($1.ystring); }
|	MAPFONT NUMBER
		{ init_map_font($2.yint); }
	MapFontEntries

/* The following entries are found only in OPL and OVP files */

|	OFMLEVEL NUMBER
		{ set_ofm_level($2.yint); }
|	FONTDIR
		{ set_font_dir($1.yint); }
|	CHARREPEAT NUMBER NUMBER
		{ init_character($2.yint,NULL); }
	CharacterEntries
		{ copy_characters($2.yint,$3.yint); }
|	FONTIVALUE NUMBER
		{init_font_ivalue($2.yint);}
	FontIvalueEntries
|	FONTMVALUE NUMBER
		{ init_font_mvalue($2.yint);}
	FontMvalueEntries
|	FONTFVALUE NUMBER
		{ init_font_fvalue($2.yint);}
	FontFvalueEntries
|	FONTPENALTY NUMBER
		{ init_font_penalty($2.yint);}
	FontPenaltyEntries
|	FONTRULE NUMBER
		{ init_font_rule($2.yint);}
	FontRuleEntries
|	FONTGLUE NUMBER
		{ init_font_glue($2.yint);}
	FontGlueEntries
;

FontParameters :
	/* */
|	FontParameters
	LEFT OneFontParameter RIGHT
;

OneFontParameter :
	NAMEDPARAMETER FIX
		{ set_param_word($1.yint, $2.yfix); }
|	PARAMETER NUMBER FIX
		{ set_param_word($2.yint, $3.yfix); }
|	COMMENT 
;

LigKernEntries :
	/* */
|	LigKernEntries
	LEFT OneLigKernEntry RIGHT
;

OneLigKernEntry :
	LABEL NUMBER
		{ set_label_command($2.yint); }
|	LABEL BOUNDARYCHAR
		{ set_label_command(CHAR_BOUNDARY); }
|	LIG NUMBER NUMBER
		{ set_ligature_command($1.yint, $2.yint, $3.yint); }
|	KRN NUMBER FIX
		{ set_kerning_command($2.yint, $3.yfix); }
|	STOP
		{ set_stop_command(); }
|	SKIP NUMBER
		{ set_skip_command($2.yint); }
|	COMMENT 

/* The following entries are found only in OPL and OVP files */

|	CLABEL NUMBER
		{ set_c_label_command($2.yint); }
|	CKRN NUMBER FIX
		{ set_c_kerning_command($2.yint, $3.yfix); }
|	CGLUE NUMBER NUMBER
		{ set_c_glue_command($2.yint, $3.yint); }
|	CPENALTY NUMBER NUMBER
		{ set_c_penalty_command($2.yint, $3.yint); }
|	CPENGLUE NUMBER NUMBER NUMBER
		{ set_c_penglue_command($2.yint, $3.yint, $4.yint); }
;

CharacterEntries :
	/* */
|	CharacterEntries
	LEFT OneCharacterEntry RIGHT 
;

OneCharacterEntry :
	CHARMEASURE FIX
		{ set_character_measure($1.yint, $2.yfix); }
|	NEXTLARGER NUMBER
		{ set_next_larger($2.yint); }
|	VARCHAR
		{ init_var_character(); }
	VarCharParameters
|	COMMENT 

/* The following character entries are found only in VP and OVP files */

|	MAP
		{ init_map(); }
	MapEntries
		{ end_map(); }

/* The following entries are found only in OPL and OVP files */

|	ACCENT
		{ set_accent($1.yint); }
|	CHARIVALUE  NUMBER NUMBER
		{ set_character_ivalue($2.yint, $3.yint); }
|	CHARMVALUE  NUMBER NUMBER
		{ set_character_mvalue($2.yint, $3.yint); }
|	CHARFVALUE  NUMBER NUMBER
		{ set_character_fvalue($2.yint, $3.yint); }
|	CHARPENALTY NUMBER NUMBER
		{ set_character_penalty($2.yint, $3.yint); }
|	CHARRULE    NUMBER NUMBER
		{ set_character_rule($2.yint, $3.yint); }
|	CHARGLUE    NUMBER NUMBER
		{ set_character_glue($2.yint, $3.yint); }

;

VarCharParameters :
	/* */
|	VarCharParameters
	LEFT OneVarCharParameter RIGHT
;

OneVarCharParameter :
	EXTEN NUMBER
	    { set_extensible_piece($1.yint, $2.yint); }
|	COMMENT 
;


/* The following four nonterminals are found only in VP and OVP files */

MapFontEntries :
	/* */
|	MapFontEntries
	LEFT OneMapFontEntry RIGHT
;

OneMapFontEntry :
	FONTNAME
		{ set_font_name($1.ystring); }
|	FONTAREA
		{ set_font_area($1.ystring); }
|	FONTCHECKSUM NUMBER
		{ set_font_check_sum($2.yint); }
|	FONTAT FIX
		{ set_font_at($2.yfix); }
|	FONTDSIZE FIX
		{ set_font_design_size($2.yfix); }
|	COMMENT
;

MapEntries :
	/* */
|	MapEntries
	LEFT OneMapEntry RIGHT
;

OneMapEntry :
	SELECTFONT NUMBER
		{ set_select_font($2.yint); }
|	SETCHAR NUMBER
		{ set_set_char($2.yint); }
|	SETRULE FIX FIX
		{ set_set_rule($2.yfix, $3.yfix); }
|	MOVE FIX
		{ set_move($1.yint, $2.yfix); }
|	PUSH
		{ set_push(); }
|	POP
		{ set_pop(); }
|	SPECIAL
		{ set_special($1.ystring); }
|	SPECIALHEX
		{ set_special_hex($1.ystring); }
;

/* The remaining nonterminals are found only in OPL and OVP files */

FontIvalueEntries :
	/* */
|	FontIvalueEntries
	LEFT OneFontIvalueEntry RIGHT
;

OneFontIvalueEntry :
	IVALUE NUMBER
		{ init_font_ivalue_entry($2.yint); }
	IvalueDefinition
|	COMMENT
;

IvalueDefinition :
	/* */
|	IvalueDefinition
	LEFT OneIvalueDefinition RIGHT
;

OneIvalueDefinition :
	IVALUEVAL NUMBER
		{ set_font_ivalue_definition($2.yint); }
|	COMMENT
;

FontMvalueEntries :
	/* */
|	FontMvalueEntries
	LEFT OneFontMvalueEntry RIGHT
;

OneFontMvalueEntry :
	MVALUE NUMBER
		{ init_font_mvalue_entry($2.yint); }
	MvalueDefinition
|	COMMENT
;

MvalueDefinition :
	/* */
|	MvalueDefinition
	LEFT OneMvalueDefinition RIGHT
;

OneMvalueDefinition :
	MVALUEVAL FIX
		{ set_font_mvalue_definition($2.yfix); }
|	COMMENT
;

FontFvalueEntries :
	/* */
|	FontFvalueEntries
	LEFT OneFontFvalueEntry RIGHT
;

OneFontFvalueEntry :
	FVALUE NUMBER
		{ init_font_fvalue_entry($2.yint); }
	FvalueDefinition
|	COMMENT
;

FvalueDefinition :
	/* */
|	FvalueDefinition
	LEFT OneFvalueDefinition RIGHT
;

OneFvalueDefinition :
	FVALUEVAL FIX
		{ set_font_fvalue_definition($2.yfix); }
|	COMMENT
;

FontPenaltyEntries :
	/* */
|	FontPenaltyEntries
	LEFT OneFontPenaltyEntry RIGHT
;

OneFontPenaltyEntry :
	PENALTY NUMBER
		{ init_font_penalty_entry($2.yint); }
	PenaltyDefinition
|	COMMENT
;

PenaltyDefinition :
	/* */
|	PenaltyDefinition
	LEFT OnePenaltyDefinition RIGHT
;

OnePenaltyDefinition :
	PENALTYVAL NUMBER
		{ set_font_penalty_definition($2.yint); }
|	COMMENT
;

FontRuleEntries :
	/* */
|	FontRuleEntries
	LEFT OneFontRuleEntry RIGHT
;

OneFontRuleEntry :
	RULE NUMBER
		{ init_font_rule_entry($2.yint); }
	RuleDefinition
|	COMMENT
;

RuleDefinition :
	/* */
|	RuleDefinition
	LEFT OneRuleDefinition RIGHT
;

OneRuleDefinition :
	RULEMEASURE FIX
		{ set_font_rule_measure($1.yint, $2.yfix); }
|	COMMENT
;

FontGlueEntries :
	/* */
|	FontGlueEntries
	LEFT OneFontGlueEntry RIGHT
;

OneFontGlueEntry :
	GLUE NUMBER
		{ init_font_glue_entry($2.yint); }
	GlueDefinition
|	COMMENT
;

GlueDefinition :
	/* */
|	GlueDefinition
	LEFT OneGlueDefinition RIGHT
;

OneGlueDefinition :
	GLUETYPE GLUEKIND
		{ set_font_glue_type($2.yint); }
|       GLUEWD FIX
                { set_font_glue_width($2.yfix); }
|	GLUECHAR NUMBER
		{ set_font_glue_character($2.yint); }
|	GLUERULE NUMBER NUMBER
		{ set_font_glue_rule($2.yint, $3.yint); }
|	GLUESHRINKSTRETCH FIX GLUEORDER
		{ set_font_glue_shrink_stretch($1.yint, $2.yfix, $3.yint); }
|	COMMENT
;
