#include "support/util.h"
#include "otfcc/font.h"
#include "table/all.h"
#include "otfcc/sfnt-builder.h"
#include "consolidate/consolidate.h"

#define OTFCC_CHR(a,b,c,d) ( ((a)<<24) | ((b)<<16) | ((c)<<8) | (d) )

static void *createFontTable(otfcc_Font *font, const uint32_t tag) {
	switch (tag) {
		case OTFCC_CHR('n','a','m','e'):
			return table_iName.create();
		case OTFCC_CHR('G','S','U','B'):
		case OTFCC_CHR('G','P','O','S'):
			return table_iOTL.create();
		default:
			return NULL;
	}
}

static void deleteFontTable(otfcc_Font *font, const uint32_t tag) {
	switch (tag) {
		case OTFCC_CHR('h','e','a','d'):
			if (font->head) DELETE(table_iHead.free, font->head);
			return;
		case OTFCC_CHR('h','h','e','a'):
			if (font->hhea) DELETE(table_iHhea.free, font->hhea);
			return;
		case OTFCC_CHR('m','a','x','p'):
			if (font->maxp) DELETE(table_iMaxp.free, font->maxp);
			return;
		case OTFCC_CHR('O','S','_','2'):
		case OTFCC_CHR('O','S','/','2'):
			if (font->OS_2) DELETE(table_iOS_2.free, font->OS_2);
			return;
		case OTFCC_CHR('n','a','m','e'):
			if (font->name) DELETE(table_iName.free, font->name);
			return;
		case OTFCC_CHR('m','e','t','a'):
			if (font->meta) DELETE(table_iMeta.free, font->meta);
			return;
		case OTFCC_CHR('h','m','t','x'):
			if (font->hmtx) DELETE(table_iHmtx.free, font->hmtx);
			return;
		case OTFCC_CHR('v','m','t','x'):
			if (font->vmtx) DELETE(table_iVmtx.free, font->vmtx);
			return;
		case OTFCC_CHR('p','o','s','t'):
			if (font->post) DELETE(iTable_post.free, font->post);
			return;
#if 0
		case OTFCC_CHR('h','d','m','x'):
			if (font->hdmx) DELETE(otfcc_deleteHdmx, font->hdmx);
			return;
#endif
		case OTFCC_CHR('v','h','e','a'):
			if (font->vhea) DELETE(table_iVhea.free, font->vhea);
			return;
		case OTFCC_CHR('f','p','g','m'):
			if (font->fpgm) DELETE(table_iFpgm_prep.free, font->fpgm);
			return;
		case OTFCC_CHR('p','r','e','p'):
			if (font->prep) DELETE(table_iFpgm_prep.free, font->prep);
			return;
		case OTFCC_CHR('c','v','t','_'):
		case OTFCC_CHR('c','v','t',' '):
			if (font->cvt_) DELETE(table_iCvt.free, font->cvt_);
			return;
		case OTFCC_CHR('g','a','s','p'):
			if (font->gasp) DELETE(table_iGasp.free, font->gasp);
			return;
		case OTFCC_CHR('C','F','F','_'):
		case OTFCC_CHR('C','F','F',' '):
			if (font->CFF_) DELETE(table_iCFF.free, font->CFF_);
			return;
		case OTFCC_CHR('g','l','y','f'):
			if (font->glyf) DELETE(table_iGlyf.free, font->glyf);
			return;
		case OTFCC_CHR('c','m','a','p'):
			if (font->cmap) DELETE(table_iCmap.free, font->cmap);
			return;
		case OTFCC_CHR('L','T','S','H'):
			if (font->LTSH) DELETE(table_iLTSH.free, font->LTSH);
			return;
		case OTFCC_CHR('G','S','U','B'):
			if (font->GSUB) DELETE(table_iOTL.free, font->GSUB);
			return;
		case OTFCC_CHR('G','P','O','S'):
			if (font->GPOS) DELETE(table_iOTL.free, font->GPOS);
			return;
		case OTFCC_CHR('G','D','E','F'):
			if (font->GDEF) DELETE(table_iGDEF.free, font->GDEF);
			return;
		case OTFCC_CHR('B','A','S','E'):
			if (font->BASE) DELETE(table_iBASE.free, font->BASE);
			return;
		case OTFCC_CHR('V','O','R','G'):
			if (font->VORG) DELETE(table_iVORG.free, font->VORG);
			return;
		case OTFCC_CHR('C','P','A','L'):
			if (font->CPAL) DELETE(table_iCPAL.free, font->CPAL);
			return;
		case OTFCC_CHR('C','O','L','R'):
			if (font->COLR) DELETE(table_iCOLR.free, font->COLR);
			return;
		case OTFCC_CHR('S','V','G',' '):
		case OTFCC_CHR('S','V','G','_'):
			if (font->SVG_) DELETE(table_iSVG.free, font->SVG_);
			return;
		case OTFCC_CHR('T','S','I','0'):
		case OTFCC_CHR('T','S','I','1'):
			if (font->TSI_01) DELETE(table_iTSI.free, font->TSI_01);
			return;
		case OTFCC_CHR('T','S','I','2'):
		case OTFCC_CHR('T','S','I','3'):
			if (font->TSI_23) DELETE(table_iTSI.free, font->TSI_23);
			return;
		case OTFCC_CHR('T','S','I','5'):
			if (font->TSI5) DELETE(otl_iClassDef.free, font->TSI5);
			return;
	}
}

static INLINE void initFont(otfcc_Font *font) {
	memset(font, 0, sizeof(*font));
}
static INLINE void disposeFont(otfcc_Font *font) {
	deleteFontTable(font, OTFCC_CHR('h','e','a','d'));
	deleteFontTable(font, OTFCC_CHR('h','h','e','a'));
	deleteFontTable(font, OTFCC_CHR('m','a','x','p'));
	deleteFontTable(font, OTFCC_CHR('O','S','_','2'));
	deleteFontTable(font, OTFCC_CHR('n','a','m','e'));
	deleteFontTable(font, OTFCC_CHR('m','e','t','a'));
	deleteFontTable(font, OTFCC_CHR('h','m','t','x'));
	deleteFontTable(font, OTFCC_CHR('v','m','t','x'));
	deleteFontTable(font, OTFCC_CHR('p','o','s','t'));
	deleteFontTable(font, OTFCC_CHR('h','d','m','x'));
	deleteFontTable(font, OTFCC_CHR('v','h','e','a'));
	deleteFontTable(font, OTFCC_CHR('f','p','g','m'));
	deleteFontTable(font, OTFCC_CHR('p','r','e','p'));
	deleteFontTable(font, OTFCC_CHR('c','v','t','_'));
	deleteFontTable(font, OTFCC_CHR('g','a','s','p'));
	deleteFontTable(font, OTFCC_CHR('C','F','F','_'));
	deleteFontTable(font, OTFCC_CHR('g','l','y','f'));
	deleteFontTable(font, OTFCC_CHR('c','m','a','p'));
	deleteFontTable(font, OTFCC_CHR('L','T','S','H'));
	deleteFontTable(font, OTFCC_CHR('G','S','U','B'));
	deleteFontTable(font, OTFCC_CHR('G','P','O','S'));
	deleteFontTable(font, OTFCC_CHR('G','D','E','F'));
	deleteFontTable(font, OTFCC_CHR('B','A','S','E'));
	deleteFontTable(font, OTFCC_CHR('V','O','R','G'));
	deleteFontTable(font, OTFCC_CHR('C','P','A','L'));
	deleteFontTable(font, OTFCC_CHR('C','O','L','R'));
	deleteFontTable(font, OTFCC_CHR('S','V','G','_'));
	deleteFontTable(font, OTFCC_CHR('T','S','I','0'));
	deleteFontTable(font, OTFCC_CHR('T','S','I','2'));
	deleteFontTable(font, OTFCC_CHR('T','S','I','5'));

	GlyphOrder.free(font->glyph_order);
}
caryll_standardRefTypeFn(otfcc_Font, initFont, disposeFont);

caryll_ElementInterfaceOf(otfcc_Font) otfcc_iFont = {
    caryll_standardRefTypeMethods(otfcc_Font),
    .createTable = createFontTable,
    .deleteTable = deleteFontTable,
    .consolidate = otfcc_consolidateFont,
};
