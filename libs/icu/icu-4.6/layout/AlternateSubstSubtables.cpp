/*
 *
 * (C) Copyright IBM Corp. 1998-2004 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphFilter.h"
#include "OpenTypeTables.h"
#include "GlyphSubstitutionTables.h"
#include "AlternateSubstSubtables.h"
#include "GlyphIterator.h"
#include "LESwaps.h"

U_NAMESPACE_BEGIN

le_uint32 AlternateSubstitutionSubtable::process(GlyphIterator *glyphIterator, const LEGlyphFilter *filter) const
{
    // NOTE: For now, we'll just pick the first alternative...
    LEGlyphID glyph = glyphIterator->getCurrGlyphID();
    le_int32 coverageIndex = getGlyphCoverage(glyph);

    if (coverageIndex >= 0) {
        le_uint16 altSetCount = SWAPW(alternateSetCount);

        if (coverageIndex < altSetCount) {
            Offset alternateSetTableOffset = SWAPW(alternateSetTableOffsetArray[coverageIndex]);
            const AlternateSetTable *alternateSetTable =
                (const AlternateSetTable *) ((char *) this + alternateSetTableOffset);
            le_int32 altIndex = glyphIterator->getFeatureParam();

            if (altIndex < SWAPW(alternateSetTable->glyphCount)) {
	            TTGlyphID alternate = SWAPW(alternateSetTable->alternateArray[altIndex]);

                if (filter == NULL || filter->accept(LE_SET_GLYPH(glyph, alternate))) {
                    glyphIterator->setCurrGlyphID(alternate);
                }

                return 1;
            }
            
	    // feature param was out of range for the glyph
        }

        // XXXX If we get here, the table's mal-formed...
    }

    return 0;
}

U_NAMESPACE_END
