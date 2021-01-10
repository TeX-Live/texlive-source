#include "support/util.h"
#include "otfcc/font.h"
#include "table/all.h"
#include "otfcc/sfnt-builder.h"
#include "stat.h"

#define OTFCC_CHR(a,b,c,d) ( ((a)<<24) | ((b)<<16) | ((c)<<8) | (d) )

static void *serializeToOTF(otfcc_Font *font, const otfcc_Options *options) {
	// do stat before serialize
	otfcc_statFont(font, options);

	otfcc_SFNTBuilder *builder =
	    otfcc_newSFNTBuilder(font->subtype == FONTTYPE_CFF ? OTFCC_CHR('O','T','T','O') : 0x00010000, options);
	// Outline data
	if (font->subtype == FONTTYPE_TTF) {
		table_GlyfAndLocaBuffers pair = otfcc_buildGlyf(font->glyf, font->head, options);
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('g','l','y','f'), pair.glyf);
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('l','o','c','a'), pair.loca);
	} else {
		table_CFFAndGlyf r = {font->CFF_, font->glyf};
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('C','F','F',' '), otfcc_buildCFF(r, options));
	}

	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('h','e','a','d'), otfcc_buildHead(font->head, options));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('h','h','e','a'), otfcc_buildHhea(font->hhea, options));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('O','S','/','2'), otfcc_buildOS_2(font->OS_2, options));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('m','a','x','p'), otfcc_buildMaxp(font->maxp, options));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('n','a','m','e'), otfcc_buildName(font->name, options));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('m','e','t','a'), otfcc_buildMeta(font->meta, options));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('p','o','s','t'),
	                            otfcc_buildPost(font->post, font->glyph_order, options));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('c','m','a','p'), otfcc_buildCmap(font->cmap, options));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('g','a','s','p'), otfcc_buildGasp(font->gasp, options));

	if (font->subtype == FONTTYPE_TTF) {
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('f','p','g','m'), otfcc_buildFpgmPrep(font->fpgm, options));
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('p','r','e','p'), otfcc_buildFpgmPrep(font->prep, options));
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('c','v','t',' '), otfcc_buildCvt(font->cvt_, options));
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('L','T','S','H'), otfcc_buildLTSH(font->LTSH, options));
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('V','D','M','X'), otfcc_buildVDMX(font->VDMX, options));
	}

	if (font->hhea && font->maxp && font->hmtx) {
		uint16_t hmtx_counta = font->hhea->numberOfMetrics;
		uint16_t hmtx_countk = font->maxp->numGlyphs - font->hhea->numberOfMetrics;
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('h','m','t','x'),
		                            otfcc_buildHmtx(font->hmtx, hmtx_counta, hmtx_countk, options));
	}

	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('v','h','e','a'), otfcc_buildVhea(font->vhea, options));
	if (font->vhea && font->maxp && font->vmtx) {
		uint16_t vmtx_counta = font->vhea->numOfLongVerMetrics;
		uint16_t vmtx_countk = font->maxp->numGlyphs - font->vhea->numOfLongVerMetrics;
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('v','m','t','x'),
		                            otfcc_buildVmtx(font->vmtx, vmtx_counta, vmtx_countk, options));
	}
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('V','O','R','G'), otfcc_buildVORG(font->VORG, options));

	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('G','S','U','B'), otfcc_buildOtl(font->GSUB, options, "GSUB"));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('G','P','O','S'), otfcc_buildOtl(font->GPOS, options, "GPOS"));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('G','D','E','F'), otfcc_buildGDEF(font->GDEF, options));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('B','A','S','E'), otfcc_buildBASE(font->BASE, options));

	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('C','P','A','L'), otfcc_buildCPAL(font->CPAL, options));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('C','O','L','R'), otfcc_buildCOLR(font->COLR, options));
	otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('S','V','G',' '), otfcc_buildSVG(font->SVG_, options));

	{
		tsi_BuildTarget target = otfcc_buildTSI(font->TSI_01, options);
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('T','S','I','0'), target.indexPart);
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('T','S','I','1'), target.textPart);
	}
	{
		tsi_BuildTarget target = otfcc_buildTSI(font->TSI_23, options);
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('T','S','I','2'), target.indexPart);
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('T','S','I','3'), target.textPart);
	}
	if (font->glyf) {
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('T','S','I','5'),
		                            otfcc_buildTSI5(font->TSI5, options, font->glyf->length));
	}

	if (options->dummy_DSIG) {
		caryll_Buffer *dsig = bufnew();
		bufwrite32b(dsig, 0x00000001);
		bufwrite16b(dsig, 0);
		bufwrite16b(dsig, 0);
		otfcc_SFNTBuilder_pushTable(builder, OTFCC_CHR('D','S','I','G'), dsig);
	}

	caryll_Buffer *otf = otfcc_SFNTBuilder_serialize(builder);
	otfcc_deleteSFNTBuilder(builder);
	otfcc_unstatFont(font, options);
	return otf;
}
static void freeFontWriter(otfcc_IFontSerializer *self) {
	free(self);
}
otfcc_IFontSerializer *otfcc_newOTFWriter() {
	otfcc_IFontSerializer *writer;
	NEW(writer);
	writer->serialize = serializeToOTF;
	writer->free = freeFontWriter;
	return writer;
}
