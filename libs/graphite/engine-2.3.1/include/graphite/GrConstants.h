/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: constants.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Various global constants.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GRCONSTANTS_INCLUDED
#define GRCONSTANTS_INCLUDED

//:End Ignore

namespace gr
{

// Maximum table versions handled by this engine:
enum {

	kSilfVersion		= 0x00030000,
	kRuleVersion		= 0x00030000,
	kGlatVersion		= 0x00010000,
	kGlocVersion		= 0x00010000,
	kFeatVersion		= 0x00020000,
	kSileVersion		= 0x00010000,
	kSillVersion		= 0x00010000

};

enum {

	kMaxFeatures				= 64,
	kMaxComponentsPerGlyph		= 32,
	kFieldsPerComponent			= 4,
	kMaxSlotsPerRule			= 64,
	kMaxSubTablesInFont			= 256,
	kMaxUserDefinableSlotAttrs	= 64,
	kMaxJLevels					= 4

};

//	Maxima permitted by the binary format of the TrueType tables.
enum {

	kMaxSubTables			= 256,
	kMaxRenderingBehavior	= 65536,	// ie, the maximum value allowed for an ID
	kMaxGlyphAttrs			= 65536,	// Gloc table
	kMinGlyphAttrValue		=-32768,
	kMaxGlyphAttrValue		= 32768,
	kMaxPasses				= 128,		// Sil_sub table allows 256
	kMaxPseudos				= 65536,
	kMaxRendBehaviors		= 256,
	kMaxReplcmtClasses		= 65536,	// Class map
	kMaxReplcmtClassesV1_2	= 256,
	kMaxComponents			= 16383

};


enum {

	kPosInfinity	= 0x3FFFFFF,
	kNegInfinity	= kPosInfinity * -1,

	kInvalid		= kNegInfinity

};

#define kPosInfFloat (float)kPosInfinity
#define kNegInfFloat (float)kNegInfinity


enum {
	kGpointZero = -2
};


//	 for stream processing
enum {
	kBacktrack = -2,
	kNextPass = -1
};


// internal justification modes
enum {
	kjmodiNormal = 0,
	kjmodiMeasure,
	kjmodiJustify,
	kjmodiCanShrink // like normal, but shrinking is possible
};


enum DirCode {  // Hungarian: dirc

	kdircUnknown	= -1,
	kdircNeutral	=  0,	// other neutrals (default) - ON
	kdircL			=  1,	// left-to-right, strong - L
	kdircR			=  2,	// right-to-left, strong - R
	kdircRArab		=  3,	// Arabic letter, right-to-left, strong, AR
	kdircEuroNum	=  4,	// European number, left-to-right, weak - EN
	kdircEuroSep	=  5,	// European separator, left-to-right, weak - ES
	kdircEuroTerm	=  6,	// European number terminator, left-to-right, weak - ET
	kdircArabNum	=  7,	// Arabic number, left-to-right, weak - AN
	kdircComSep		=  8,	// Common number separator, left-to-right, weak - CS
	kdircWhiteSpace	=  9,	// white space, neutral - WS
	kdircBndNeutral = 10,	// boundary neutral - BN

	kdircLRO		= 11,	// LTR override
	kdircRLO		= 12,	// RTL override
	kdircLRE		= 13,	// LTR embedding
	kdircRLE		= 14,	// RTL embedding
	kdircPDF		= 15,	// pop directional format

	//	I think we need this too:
	kdircNSM		= 16,	// non-space mark

	//	Special values for internal use:
	kdircLlb		= 32,	// left-to-right line-break
	kdircRlb		= 33,	// right-to-left line-break
	kdircPdfL		= 34,	// PDF marker matching an LRO or LRE
	kdircPdfR		= 35	// PDF marker matching an RLO or RLE

};


bool StrongDir(DirCode dirc);
bool WeakDir(DirCode dirc);
bool NeutralDir(DirCode dirc);
bool RightToLeftDir(DirCode dirc);


//	Unicode characters with special treatments
enum DefinedChars {

	knSpace			= 0x0020,		// space
	knHyphen		= 0x002D,		// hyphen

	knTab			= 0x0009,		// tab (horizontal)
	knLF			= 0x000A,		// line feed
	knCR			= 0x000D,		// carriage return
	knLineSep		= 0x2028,		// line separator (soft return)
	knParaSep		= 0x2029,		// paragraph separator

	knLRM			= 0x200E,		// left-to-right mark
	knRLM			= 0x200F,		// right-to-left mark
	knLRE			= 0x202A,		// left-to-right embedding
	knRLE			= 0x202B,		// right-to-left embedding
	knPDF			= 0x202C,		// pop directional format
	knLRO			= 0x202D,		// left-to-right override
	knRLO			= 0x202E,		// right-to-left override

	knORC			= 0xFFFC		// Object Replacement Character
};

bool BidiCode(int nUnicode);


typedef enum SpecialSlots {

	kspslNone		= 0,

	kspslLbInitial	= 1,
	kspslLbFinal	= 2,

	kspslLRM		= 3,
	kspslRLM		= 4,
	kspslLRO		= 5,
	kspslRLO		= 6,
	kspslLRE		= 7,
	kspslRLE		= 8,
	kspslPDF		= 9

} SpecialSlots;


// version 1.0 breakweights
enum BreakWeightsV1
{
	klbv1WordBreak		= 1,
	klbv1HyphenBreak	= 2,
	klbv1LetterBreak	= 3,
	klbv1ClipBreak		= 4
};


typedef enum SlotAttrName {

	kslatAdvX = 0,		kslatAdvY,
	kslatAttTo,
	kslatAttAtX,		kslatAttAtY,		kslatAttAtGpt,
	kslatAttAtXoff,		kslatAttAtYoff,
	kslatAttWithX,		kslatAttWithY,		kslatAttWithGpt,
	kslatAttWithXoff,	kslatAttWithYoff,
	kslatAttLevel,
	kslatBreak,
	kslatCompRef,
	kslatDir,
	kslatInsert,
	kslatPosX,			kslatPosY,
	kslatShiftX,		kslatShiftY,
	kslatUserDefnV1, // version 1.0 of the font tables
	kslatMeasureSol,	kslatMeasureEol,
	kslatJStretch,		kslatJShrink,	kslatJStep,		kslatJWeight,	kslatJWidth,
	// this must be last:
	kslatUserDefn = kslatJStretch + 30,
	// I think 30 is somewhat arbitrary. We at least need to save 15 slots for more levels of
	// justification attributes.

	kslatMax,

	kslatNoEffect = kslatMax + 1	// for internal use
} SlotAttrName;


typedef enum GlyphMetric {
	kgmetLsb = 0,		kgmetRsb,
	kgmetBbTop,			kgmetBbBottom,		kgmetBbLeft,		kgmetBbRight,
	kgmetBbHeight,		kgmetBbWidth,
	kgmetAdvWidth,		kgmetAdvHeight,
	kgmetAscent,		kgmetDescent
} GlyphMetric;


enum ProcessStates
{
	kpstatJustifyMode	= 1,
	kpstatJustifyLevel	= 2
};

// user justification modes: these must match the values in stddef.gdh
enum JustifyModes
{
	kjmoduNormal	= 0,
	kjmoduMeasure	= 1,
	kjmoduJustify	= 2
};

enum TruetypeTableId {
    kttiCmap = 0x636D6170,
    kttiFeat = 0x46656174,
    kttiGlat = 0x476C6174,
    kttiGloc = 0x476C6F63,
    kttiGlyf = 0x676C7966,
    kttiHead = 0x68656164,
    kttiHhea = 0x68686561,
    kttiHmtx = 0x686D7478,
    kttiLoca = 0x6C6F6361,
    kttiMaxp = 0x6D617870,
    kttiName = 0x6E616D65,
    kttiOs2  = 0x4F532F32,
    kttiPost = 0x706F7374,
    kttiSile = 0x53696C65,
    kttiSilf = 0x53696C66,
    kttiSill = 0x53696C6C
};

} // namespace gr

#endif // CONSTANTS_INCLUDED
