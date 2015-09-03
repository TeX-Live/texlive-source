/*
 * Copyright (C) 2008-2015 The Gregorio Project (see CONTRIBUTORS.md)
 *
 * This file is part of Gregorio.
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "config.h"
#include <stdlib.h>
#include <assert.h>
#include "bool.h"
#include "struct.h"

#include "gregoriotex.h"

/* TODO: The numbers in the comments are a vestige of the older code; I'm
 * leaving them here for now, in case the refactor missed an instance
 * somewhere, but the numbers should evenually be removed */

/* (loose) naming convention, employing camel case to be TeX-csname-compliant:
 * {specific-glyph-shape}{note-position}{note-shape}{first-ambitus}{second-ambitus}
 */
OFFSET_CASE(FinalPunctum);
OFFSET_CASE(FinalDeminutus);
OFFSET_CASE(PenultBeforePunctumWide);
OFFSET_CASE(PenultBeforeDeminutus);
OFFSET_CASE(AntepenultBeforePunctum);
OFFSET_CASE(AntepenultBeforeDeminutus);
OFFSET_CASE(InitialPunctum);
OFFSET_CASE(InitioDebilis);
OFFSET_CASE(PorrNonAuctusInitialWide);
OFFSET_CASE(PorrNonAuctusInitialOne);
OFFSET_CASE(PorrAuctusInitialAny);
OFFSET_CASE(FinalInclinatum);
OFFSET_CASE(FinalInclinatumDeminutus);
OFFSET_CASE(FinalStropha);
OFFSET_CASE(FinalQuilisma);
OFFSET_CASE(FinalOriscus);
OFFSET_CASE(PenultBeforePunctumOne);
OFFSET_CASE(FinalUpperPunctum);
OFFSET_CASE(InitialOriscus);
OFFSET_CASE(InitialQuilisma);
OFFSET_CASE(TorcResNonAuctusSecondWideWide);
OFFSET_CASE(TorcResNonAuctusSecondOneWide);
OFFSET_CASE(TorcResDebilisNonAuctusSecondAnyWide);
OFFSET_CASE(FinalLineaPunctum);
OFFSET_CASE(TorcResQuilismaNonAuctusSecondWideWide);
OFFSET_CASE(TorcResOriscusNonAuctusSecondWideWide);
OFFSET_CASE(TorcResQuilismaNonAuctusSecondOneWide);
OFFSET_CASE(TorcResOriscusNonAuctusSecondOneWide);
OFFSET_CASE(TorcResNonAuctusSecondWideOne);
OFFSET_CASE(TorcResDebilisNonAuctusSecondAnyOne);
OFFSET_CASE(TorcResQuilismaNonAuctusSecondWideOne);
OFFSET_CASE(TorcResOriscusNonAuctusSecondWideOne);
OFFSET_CASE(TorcResNonAuctusSecondOneOne);
OFFSET_CASE(TorcResQuilismaNonAuctusSecondOneOne);
OFFSET_CASE(TorcResOriscusNonAuctusSecondOneOne);
OFFSET_CASE(TorcResAuctusSecondWideAny);
OFFSET_CASE(TorcResDebilisAuctusSecondAnyAny);
OFFSET_CASE(TorcResQuilismaAuctusSecondWideAny);
OFFSET_CASE(TorcResOriscusAuctusSecondWideAny);
OFFSET_CASE(TorcResAuctusSecondOneAny);
OFFSET_CASE(TorcResQuilismaAuctusSecondOneAny);
OFFSET_CASE(TorcResOriscusAuctusSecondOneAny);
OFFSET_CASE(ConnectedPenultBeforePunctumWide);
OFFSET_CASE(ConnectedPenultBeforePunctumOne);
OFFSET_CASE(InitialConnectedPunctum);
OFFSET_CASE(InitialConnectedVirga);
OFFSET_CASE(InitialConnectedQuilisma);
OFFSET_CASE(InitialConnectedOriscus);
OFFSET_CASE(FinalConnectedPunctum);
OFFSET_CASE(FinalConnectedAuctus);
OFFSET_CASE(FinalVirgaAuctus);
OFFSET_CASE(FinalConnectedVirga);
OFFSET_CASE(InitialVirga);
OFFSET_CASE(SalicusOriscusWide);
OFFSET_CASE(SalicusOriscusOne);

static __inline const char *note_before_last_note_case(
        const gregorio_glyph *const current_glyph,
        const gregorio_note *const current_note)
{
    if ((current_glyph->u.notes.liquescentia == L_DEMINUTUS_INITIO_DEBILIS
                    || current_glyph->u.notes.liquescentia == L_DEMINUTUS)
            && current_note->next) {
        return PenultBeforeDeminutus;
    } else {
        if ((current_note->u.note.pitch - current_note->next->u.note.pitch) == 1
                || (current_note->u.note.pitch -
                        current_note->next->u.note.pitch) == -1) {
            if (!current_note->previous || current_note->u.note.pitch -
                    current_note->previous->u.note.pitch > 1) {
                return ConnectedPenultBeforePunctumOne;
            } else {
                return PenultBeforePunctumOne;
            }
        } else {
            if (!current_note->previous || current_note->u.note.pitch -
                    current_note->previous->u.note.pitch > 1) {
                return ConnectedPenultBeforePunctumWide;
            } else {
                return PenultBeforePunctumWide;
            }
        }
    }
}

/* num can be FinalPunctum or FinalUpperPunctum according if the last note is a
 * standard punctum or a smaller punctum (for pes, porrectus and torculus
 * resupinus */
static __inline const char *last_note_case(
        const gregorio_glyph *const current_glyph, const char* offset_pos,
        gregorio_note *current_note, bool no_ambitus_one)
{
    if (current_glyph->u.notes.liquescentia & L_DEMINUTUS) {
        /*
         * may seem strange, but it is unlogical to typeset a small horizontal
         * episemus at the end of a flexus deminutus 
         */
        return FinalDeminutus;
    }
    if (!current_note->previous
            || (!no_ambitus_one && (offset_pos == FinalUpperPunctum
                    || current_note->previous->u.note.pitch -
            current_note->u.note.pitch == 1 ||
            current_note->u.note.pitch - current_note->previous->u.note.pitch
            == 1))) {
        return offset_pos;
    }
    if (current_note->previous->u.note.pitch < current_note->u.note.pitch) {
        if (current_glyph->u.notes.liquescentia &
                (L_AUCTUS_ASCENDENS | L_AUCTUS_DESCENDENS | L_AUCTA)) {
            return FinalConnectedAuctus;
        } else {
            return FinalConnectedVirga;
        }
    } else {
        return FinalConnectedPunctum;
    }
}

static __inline const char *first_note_case(
        const gregorio_note *const current_note,
        const gregorio_glyph *const current_glyph)
{
    bool ambitus_one = true;
    if (current_glyph->u.notes.liquescentia >= L_INITIO_DEBILIS) {
        return InitioDebilis;
    } else {
        if (current_note->next && current_note->next->u.note.pitch -
                current_note->u.note.pitch != 1 &&
                current_note->u.note.pitch - current_note->next->u.note.pitch
                != 1) {
            ambitus_one = false;
        }
        if (current_note->u.note.shape == S_ORISCUS) {
            return ambitus_one ? InitialOriscus : InitialConnectedOriscus;
        }
        if (current_note->u.note.shape == S_QUILISMA) {
            return ambitus_one ? InitialQuilisma : InitialConnectedQuilisma;
        }
        if (ambitus_one) {
            return InitialPunctum;
        }
        if (current_note->u.note.shape > current_note->next->u.note.shape) {
            return InitialConnectedVirga;
        } else {
            return InitialConnectedPunctum;
        }
    }
}

static __inline gregorio_vposition above_if_auctus(
        const gregorio_glyph *const glyph)
{
    if (glyph->u.notes.liquescentia &
            (L_AUCTUS_ASCENDENS | L_AUCTUS_DESCENDENS | L_AUCTA)) {
        return VPOS_ABOVE;
    }
    return VPOS_BELOW;
}

static __inline gregorio_vposition below_if_auctus(
        const gregorio_glyph *const glyph)
{
    if (glyph->u.notes.liquescentia &
            (L_AUCTUS_ASCENDENS | L_AUCTUS_DESCENDENS | L_AUCTA)) {
        return VPOS_BELOW;
    }
    return VPOS_ABOVE;
}

static __inline gregorio_vposition above_if_h_episemus(
        const gregorio_note *const note)
{
    if (note && note->h_episemus_above) {
        return VPOS_ABOVE;
    }
    return VPOS_BELOW;
}

static __inline gregorio_vposition above_if_either_h_episemus(
        const gregorio_note *const note)
{
    if ((note->previous && note->previous->h_episemus_above)
            || (note->next && note->next->h_episemus_above)) {
        return VPOS_ABOVE;
    }
    return VPOS_BELOW;
}

static __inline gregorio_vposition below_if_next_ambitus_allows(
        const gregorio_note *const note)
{
    assert(note->next);

    if (!note->next || note->u.note.pitch - note->next->u.note.pitch > 2) {
        return VPOS_BELOW;
    }
    return VPOS_ABOVE;
}

static __inline void low_high_set_lower(const gregorio_glyph *const glyph,
        gregorio_note *const note)
{
    if ((glyph->u.notes.liquescentia & L_DEMINUTUS) ||
            !(glyph->u.notes.liquescentia
                & (L_AUCTUS_ASCENDENS|L_AUCTUS_DESCENDENS|L_AUCTA))) {
        note->is_lower_note = true;
    }
}

static __inline void low_high_set_upper(const gregorio_glyph *const glyph,
        gregorio_note *const note)
{
    if ((glyph->u.notes.liquescentia & L_DEMINUTUS) ||
            !(glyph->u.notes.liquescentia
                & (L_AUCTUS_ASCENDENS|L_AUCTUS_DESCENDENS|L_AUCTA))) {
        note->is_upper_note = true;
    }
}

static __inline void high_low_set_upper(const gregorio_glyph *const glyph,
        gregorio_note *const note)
{
    if (glyph->u.notes.liquescentia & L_DEMINUTUS) {
        note->is_upper_note = true;
    }
}

static __inline void high_low_set_lower(const gregorio_glyph *const glyph,
        gregorio_note *const note)
{
    if (glyph->u.notes.liquescentia & L_DEMINUTUS) {
        note->is_lower_note = true;
    }
}

/* a function that finds the good sign (additional line, vepisemus or
 * hepisemus) number, according to the gregoriotex convention (described in
 * gregoriotex.tex)
 * this function is REALLY a pain in the ass, but it is sadly necessary */
/*
 *
 * For the first note of a porrectus (flexus), this table summarizes the sign
 * number (amb2 is the ambitus after the diagonal stroke):
 *
 *  Porrectus       | Porrectus       | Porrectus    | Porrectus
 *  non-auctus      | non-auctus      | auctus       | Flexus
 *  amb2=1          | amb2>1          |              |
 * =================|=================|==============|==============
 *  PorrNonAuctus   | PorrNonAuctus   | PorrAuctus   | PorrAuctus
 *   ~InitialOne    |  ~InitialWide   |  ~InitialAny |  ~InitialAny
 *
 * For the second note of a torculus resupinus (flexus), this table summarizes
 * the number (amb1 is the ambitus before the diagonal stroke and amb2 is the
 * ambitus after the diagonal stroke):
 *
 *       |            | Torculus    | Torculus    | Torculus   | Torculus
 *       |            | Resupinus   | Resupinus   | Resupinus  | Resupinus
 *       | Initial    | non-auctus  | non-auctus  | auctus     | Flexus
 *  amb1 | Note       | amb2=1      | amb2>1      |            |
 * ======|============|=============|=============|============|============
 *  1    | Punctum    | TorcRes     | TorcRes     | TorcRes    | TorcRes
 *       |            |  ~NonAuctus |  ~NonAuctus |  ~Auctus   |  ~Auctus
 *       |            |  ~Second    |  ~Second    |  ~Second   |  ~Second
 *       |            |  ~OneOne    |  ~OneWide   |  ~OneAny   |  ~OneAny
 * ------+------------+-------------+-------------+------------+------------
 *  1    | I. Debilis | TorcRes     | TorcRes     | TorcRes    | TorcRes
 *       |            |  ~Debilis   |  ~Debilis   |  ~Debilis  |  ~Debilis
 *       |            |  ~NonAuctus |  ~NonAuctus |  ~Auctus   |  ~Auctus
 *       |            |  ~Second    |  ~Second    |  ~Second   |  ~Second
 *       |            |  ~AnyOne    |  ~AnyWide   |  ~AnyAny   |  ~AnyAny
 * ------+------------+-------------+-------------+------------+------------
 *  1    | Quilisma   | TorcRes     | TorcRes     | TorcRes    | TorcRes
 *       |            |  ~Quilisma  |  ~Quilisma  |  ~Quilisma |  ~Quilisma
 *       |            |  ~NonAuctus |  ~NonAuctus |  ~Auctus   |  ~Auctus
 *       |            |  ~Second    |  ~Second    |  ~Second   |  ~Second
 *       |            |  ~OneOne    |  ~OneWide   |  ~OneAny   |  ~OneAny
 * ------+------------+-------------+-------------+------------+------------
 *  1    | Oriscus    | TorcRes     | TorcRes     | TorcRes    | TorcRes
 *       |            |  ~Oriscus   |  ~Oriscus   |  ~Oriscus  |  ~Oriscus
 *       |            |  ~NonAuctus |  ~NonAuctus |  ~Auctus   |  ~Auctus
 *       |            |  ~Second    |  ~Second    |  ~Second   |  ~Second
 *       |            |  ~OneOne    |  ~OneWide   |  ~OneAny   |  ~OneAny
 * ======|============|=============|=============|============|============
 *  >1   | Punctum    | TorcRes     | TorcRes     | TorcRes    | TorcRes
 *       |            |  ~NonAuctus |  ~NonAuctus |  ~Auctus   |  ~Auctus
 *       |            |  ~Second    |  ~Second    |  ~Second   |  ~Second
 *       |            |  ~WideOne   |  ~WideWide  |  ~WideAny  |  ~WideAny
 * ------+------------+-------------+-------------+------------+------------
 *  >1   | I. Debilis | TorcRes     | TorcRes     | TorcRes    | TorcRes
 *       |            |  ~Debilis   |  ~Debilis   |  ~Debilis  |  ~Debilis
 *       |            |  ~NonAuctus |  ~NonAuctus |  ~Auctus   |  ~Auctus
 *       |            |  ~Second    |  ~Second    |  ~Second   |  ~Second
 *       |            |  ~AnyOne    |  ~AnyWide   |  ~AnyAny   |  ~AnyAny
 * ------+------------+-------------+-------------+------------+------------
 *  >1   | Quilisma   | TorcRes     | TorcRes     | TorcRes    | TorcRes
 *       |            |  ~Quilisma  |  ~Quilisma  |  ~Quilisma |  ~Quilisma
 *       |            |  ~NonAuctus |  ~NonAuctus |  ~Auctus   |  ~Auctus
 *       |            |  ~Second    |  ~Second    |  ~Second   |  ~Second
 *       |            |  ~WideOne   |  ~WideWide  |  ~WideAny  |  ~WideAny
 * ------+------------+-------------+-------------+------------+------------
 *  >1   | Oriscus    | TorcRes     | TorcRes     | TorcRes    | TorcRes
 *       |            |  ~Oriscus   |  ~Oriscus   |  ~Oriscus  |  ~Oriscus
 *       |            |  ~NonAuctus |  ~NonAuctus |  ~Auctus   |  ~Auctus
 *       |            |  ~Second    |  ~Second    |  ~Second   |  ~Second
 *       |            |  ~WideOne   |  ~WideWide  |  ~WideAny  |  ~WideAny
 *
 */
static gregorio_vposition advise_positioning(const gregorio_glyph *const glyph,
        gregorio_note *const note, int i, const gtex_type type)
{
    gregorio_vposition h_episemus = VPOS_AUTO, v_episemus = VPOS_AUTO;
    bool v_episemus_below_is_lower = false, done;

    /* no need to clear is_lower_note/is_upper_note because we used calloc */

    switch (type) {
    case T_PES:
    case T_PESQUILISMA:
        /*
         * in the case of a pes, we put the episemus just under the bottom note 
         */
        if (i == 1) {
            if (glyph->u.notes.liquescentia & L_INITIO_DEBILIS) {
                note->gtex_offset_case = InitioDebilis;
                h_episemus = above_if_h_episemus(note->next);
            } else {
                note->is_lower_note = true;
                if (note->u.note.shape == S_QUILISMA) {
                    note->gtex_offset_case = FinalQuilisma;
                } else {
                    note->gtex_offset_case = FinalPunctum;
                }
                h_episemus = above_if_auctus(glyph);
            }
            v_episemus = VPOS_BELOW;
        } else { /* i=2  */
            if (!(glyph->u.notes.liquescentia & L_INITIO_DEBILIS)) {
                note->is_upper_note = true;
            }
            if (glyph->u.notes.liquescentia & L_DEMINUTUS) {
                note->gtex_offset_case = FinalDeminutus;
            } else {
                note->gtex_offset_case = FinalUpperPunctum;
            }
            h_episemus = VPOS_ABOVE;
            v_episemus = below_if_auctus(glyph);
        }
        break;
    case T_PESQUADRATUM:
    case T_PESQUASSUS:
    case T_PESQUILISMAQUADRATUM:
        if (i == 1) {
            note->gtex_offset_case = first_note_case(note, glyph);
            h_episemus = above_if_h_episemus(note->next);
            v_episemus = VPOS_BELOW;
        } else { /* i=2 */
            if (glyph->u.notes.liquescentia & L_DEMINUTUS) {
                note->gtex_offset_case = InitioDebilis;
            } else {
                note->gtex_offset_case = last_note_case(glyph, FinalPunctum,
                        note, true);
            }
            h_episemus = VPOS_ABOVE;
            v_episemus = VPOS_BELOW;
        }
        break;
    case T_FLEXUS:
    case T_FLEXUS_LONGQUEUE:
    case T_FLEXUS_ORISCUS:
        if (i == 1) {
            high_low_set_upper(glyph, note);
            if (type == T_FLEXUS_ORISCUS) {
                note->gtex_offset_case = InitialConnectedOriscus;
            } else {
                if (note->next && note->u.note.pitch - 
                        note->next->u.note.pitch == 1) {
                    note->gtex_offset_case = InitialVirga;
                } else {
                    note->gtex_offset_case = InitialConnectedVirga;
                }
            }
            h_episemus = VPOS_ABOVE;
            if (glyph->u.notes.liquescentia & L_DEMINUTUS) {
                v_episemus = below_if_next_ambitus_allows(note);
            } else {
                v_episemus = VPOS_BELOW;
            }
        } else { /* i=2 */
            high_low_set_lower(glyph, note);
            note->gtex_offset_case = last_note_case(glyph, FinalPunctum, note,
                    false);
            if (glyph->u.notes.liquescentia & L_DEMINUTUS) {
                h_episemus = VPOS_BELOW;
            } else {
                h_episemus = above_if_h_episemus(note->previous);
            }
            v_episemus = VPOS_BELOW;
        }
        break;
    case T_TORCULUS_RESUPINUS_FLEXUS:
        done = true;
        switch (i) {
        case 1:
            note->gtex_offset_case = first_note_case(note, glyph);
            h_episemus = above_if_h_episemus(note->next);
            v_episemus = VPOS_BELOW;
            break;
        case HEPISEMUS_FIRST_TWO:
        case 2:
            if (glyph->u.notes.liquescentia & L_INITIO_DEBILIS) {
                note->gtex_offset_case = TorcResDebilisAuctusSecondAnyAny;
            } else {
                if (note->u.note.pitch - note->previous->u.note.pitch == 1) {
                    switch (note->previous->u.note.shape) {
                    case S_QUILISMA:
                        note->gtex_offset_case =
                                TorcResQuilismaAuctusSecondOneAny;
                        break;
                    case S_ORISCUS:
                        note->gtex_offset_case =
                                TorcResOriscusAuctusSecondOneAny;
                        break;
                    default:
                        note->gtex_offset_case = TorcResAuctusSecondOneAny;
                        break;
                    }
                } else {
                    switch (note->previous->u.note.shape) {
                    case S_QUILISMA:
                        note->gtex_offset_case =
                                TorcResQuilismaAuctusSecondWideAny;
                        break;
                    case S_ORISCUS:
                        note->gtex_offset_case =
                                TorcResOriscusAuctusSecondWideAny;
                        break;
                    default:
                        note->gtex_offset_case = TorcResAuctusSecondWideAny;
                        break;
                    }
                }
            }
            h_episemus = VPOS_ABOVE;
            v_episemus = VPOS_BELOW;
            v_episemus_below_is_lower = true;
            break;
        default:
            --i;
            done = false;
            break;
        }

        if (done) {
            break;
        }
        /* else fallthrough to the next case! */
    case T_PORRECTUS_FLEXUS:
        switch (i) {
        case HEPISEMUS_FIRST_TWO:
        case 1:
            note->gtex_offset_case = PorrAuctusInitialAny;
            h_episemus = VPOS_ABOVE;
            v_episemus = VPOS_BELOW;
            v_episemus_below_is_lower = true;
            break;
        case 2:
            if (glyph->u.notes.liquescentia & L_DEMINUTUS) {
                note->gtex_offset_case = AntepenultBeforeDeminutus;
            } else {
                note->gtex_offset_case = AntepenultBeforePunctum;
            }
            h_episemus = above_if_either_h_episemus(note);
            v_episemus = VPOS_BELOW;
            break;
        case 3:
            high_low_set_upper(glyph, note);
            h_episemus = VPOS_ABOVE;
            if ((glyph->u.notes.liquescentia & L_DEMINUTUS) && note->next) {
                v_episemus = below_if_next_ambitus_allows(note);
                note->gtex_offset_case = PenultBeforeDeminutus;
            } else {
                v_episemus = VPOS_BELOW;
                note->gtex_offset_case = note_before_last_note_case(glyph,
                        note);
            }
            break;
        default:
            high_low_set_lower(glyph, note);
            note->gtex_offset_case = last_note_case(glyph, FinalPunctum, note,
                    false);
            h_episemus = above_if_h_episemus(note->previous);
            v_episemus = VPOS_BELOW;
            break;
        }
        break;
    case T_TORCULUS_RESUPINUS:
        done = true;
        switch (i) {
        case 1:
            note->gtex_offset_case = first_note_case(note, glyph);
            h_episemus = above_if_h_episemus(note->next);
            v_episemus = VPOS_BELOW;
            break;
        case HEPISEMUS_FIRST_TWO:
        case 2:
            if (glyph->u.notes.liquescentia &
                    (L_AUCTUS_ASCENDENS | L_AUCTUS_DESCENDENS | L_AUCTA)) {
                /* auctus */
                if (glyph->u.notes.liquescentia & L_INITIO_DEBILIS) {
                    note->gtex_offset_case = TorcResDebilisAuctusSecondAnyAny;
                } else {
                    if (note->u.note.pitch -
                            note->previous->u.note.pitch == 1) {
                        switch (note->previous->u.note.shape) {
                        case S_QUILISMA:
                            note->gtex_offset_case =
                                    TorcResQuilismaAuctusSecondOneAny;
                            break;
                        case S_ORISCUS:
                            note->gtex_offset_case =
                                    TorcResOriscusAuctusSecondOneAny;
                            break;
                        default:
                            note->gtex_offset_case = TorcResAuctusSecondOneAny;
                            break;
                        }
                    } else {
                        switch (note->previous->u.note.shape) {
                        case S_QUILISMA:
                            note->gtex_offset_case =
                                    TorcResQuilismaAuctusSecondWideAny;
                            break;
                        case S_ORISCUS:
                            note->gtex_offset_case =
                                    TorcResOriscusAuctusSecondWideAny;
                            break;
                        default:
                            note->gtex_offset_case = TorcResAuctusSecondWideAny;
                            break;
                        }
                    }
                }
            } else if (note->next->next->u.note.pitch -
                    note->next->u.note.pitch == 1) {
                /* non-auctus with a second ambitus of 1 */
                if (glyph->u.notes.liquescentia & L_INITIO_DEBILIS) {
                    note->gtex_offset_case =
                            TorcResDebilisNonAuctusSecondAnyOne;
                } else {
                    if (note->u.note.pitch -
                            note->previous->u.note.pitch == 1) {
                        switch (note->previous->u.note.shape) {
                        case S_QUILISMA:
                            note->gtex_offset_case =
                                    TorcResQuilismaNonAuctusSecondOneOne;
                            break;
                        case S_ORISCUS:
                            note->gtex_offset_case =
                                    TorcResOriscusNonAuctusSecondOneOne;
                            break;
                        default:
                            note->gtex_offset_case =
                                    TorcResNonAuctusSecondOneOne;
                            break;
                        }
                    } else {
                        switch (note->previous->u.note.shape) {
                        case S_QUILISMA:
                            note->gtex_offset_case =
                                    TorcResQuilismaNonAuctusSecondWideOne;
                            break;
                        case S_ORISCUS:
                            note->gtex_offset_case =
                                    TorcResOriscusNonAuctusSecondWideOne;
                            break;
                        default:
                            note->gtex_offset_case =
                                    TorcResNonAuctusSecondWideOne;
                            break;
                        }
                    }
                }
            } else {
                /* non-auctus with a second ambitus of at least 2 */
                if (glyph->u.notes.liquescentia & L_INITIO_DEBILIS) {
                    note->gtex_offset_case =
                            TorcResDebilisNonAuctusSecondAnyWide;
                } else {
                    if (note->u.note.pitch -
                            note->previous->u.note.pitch == 1) {
                        switch (note->previous->u.note.shape) {
                        case S_QUILISMA:
                            note->gtex_offset_case =
                                    TorcResQuilismaNonAuctusSecondOneWide;
                            break;
                        case S_ORISCUS:
                            note->gtex_offset_case =
                                    TorcResOriscusNonAuctusSecondOneWide;
                            break;
                        default:
                            note->gtex_offset_case =
                                    TorcResNonAuctusSecondOneWide;
                            break;
                        }
                    } else {
                        switch (note->previous->u.note.shape) {
                        case S_QUILISMA:
                            note->gtex_offset_case =
                                    TorcResQuilismaNonAuctusSecondWideWide;
                            break;
                        case S_ORISCUS:
                            note->gtex_offset_case =
                                    TorcResOriscusNonAuctusSecondWideWide;
                            break;
                        default:
                            note->gtex_offset_case =
                                    TorcResNonAuctusSecondWideWide;
                            break;
                        }
                    }
                }
            }
            h_episemus = VPOS_ABOVE;
            v_episemus = VPOS_BELOW;
            v_episemus_below_is_lower = true;
            break;
        default:
            --i;
            done = false;
            break;
        }

        if (done) {
            break;
        }
        /* else fallthrough to the next case! */
    case T_PORRECTUS:
        switch (i) {
        case HEPISEMUS_FIRST_TWO:
        case 1:
            if (glyph->u.notes.liquescentia &
                    (L_AUCTUS_ASCENDENS | L_AUCTUS_DESCENDENS | L_AUCTA)) {
                /* auctus */
                note->gtex_offset_case = PorrAuctusInitialAny;
            } else if (note->next->next->u.note.pitch -
                    note->next->u.note.pitch == 1) {
                /* non-auctus with a second ambitus of 1 */
                note->gtex_offset_case = PorrNonAuctusInitialOne;
            } else {
                /* non-auctus with a second ambitus of at least 2 */
                note->gtex_offset_case = PorrNonAuctusInitialWide;
            }
            h_episemus = VPOS_ABOVE;
            v_episemus = VPOS_BELOW;
            v_episemus_below_is_lower = true;
            break;
        case 2:
            low_high_set_lower(glyph, note);
            if (glyph->u.notes.liquescentia & L_DEMINUTUS) {
                note->gtex_offset_case = PenultBeforeDeminutus;
            } else if (glyph->u.notes.liquescentia
                    & (L_AUCTUS_ASCENDENS|L_AUCTUS_DESCENDENS|L_AUCTA)) {
                note->gtex_offset_case = PenultBeforePunctumWide;
            } else {
                note->gtex_offset_case = FinalPunctum;
            }
            h_episemus = above_if_h_episemus(note->previous);
            v_episemus = VPOS_BELOW;
            break;
        default: /* case 3 */
            low_high_set_upper(glyph, note);
            note->gtex_offset_case = last_note_case(glyph, FinalUpperPunctum,
                    note, true);
            h_episemus = VPOS_ABOVE;
            v_episemus = below_if_auctus(glyph);
            break;
        }
        break;
    case T_SCANDICUS:
        switch (i) {
        case 1:
            note->gtex_offset_case = first_note_case(note, glyph);
            h_episemus = VPOS_BELOW;
            v_episemus = VPOS_BELOW;
            break;
        case 2:
            low_high_set_lower(glyph, note);
            if (note->u.note.pitch - note->previous->u.note.pitch == 1) {
                note->gtex_offset_case = FinalPunctum;
            } else {
                note->gtex_offset_case = FinalConnectedPunctum;
            }
            h_episemus = VPOS_BELOW;
            v_episemus = VPOS_BELOW;
            break;
        default:
            low_high_set_upper(glyph, note);
            note->gtex_offset_case = last_note_case(glyph, FinalPunctum, note,
                    true);
            h_episemus = VPOS_ABOVE;
            v_episemus = VPOS_BELOW;
            break;
        }
        break;
    case T_SALICUS:
    case T_SALICUS_LONGQUEUE:
        v_episemus = VPOS_BELOW;
        switch (i) {
        case 1:
            note->gtex_offset_case = InitialConnectedPunctum;
            h_episemus = above_if_h_episemus(note->next);
            break;
        case 2:
            if (note->next->u.note.pitch - note->u.note.pitch == 1) {
                note->gtex_offset_case = SalicusOriscusOne;
            } else {
                note->gtex_offset_case = SalicusOriscusWide;
            }
            h_episemus = VPOS_ABOVE;
            break;
        default:
            note->gtex_offset_case = FinalConnectedVirga;
            h_episemus = VPOS_ABOVE;
            break;
        }
        break;
    case T_ANCUS:
    case T_ANCUS_LONGQUEUE:
        switch (i) {
        case 1:
            note->gtex_offset_case = first_note_case(note, glyph);
            h_episemus = above_if_h_episemus(note->next);
            v_episemus = VPOS_BELOW;
            break;
        case 2:
            note->gtex_offset_case = note_before_last_note_case(glyph, note);
            h_episemus = VPOS_ABOVE;
            v_episemus = VPOS_BELOW;
            break;
        case 3:
            note->is_upper_note = true;
            note->gtex_offset_case = last_note_case(glyph, FinalPunctum, note,
                    false);
            h_episemus = VPOS_ABOVE;
            v_episemus = VPOS_BELOW;
            break;
        default:
            note->is_lower_note = true;
            note->gtex_offset_case = last_note_case(glyph, FinalPunctum, note,
                    false);
            h_episemus = VPOS_BELOW;
            v_episemus = VPOS_BELOW;
            break;
        }
        break;
    case T_TORCULUS:
        switch (i) {
        case 1:
            note->gtex_offset_case = first_note_case(note, glyph);
            h_episemus = above_if_h_episemus(note->next);
            v_episemus = VPOS_BELOW;
            break;
        case 2:
            high_low_set_upper(glyph, note);
            note->gtex_offset_case = note_before_last_note_case(glyph, note);
            h_episemus = VPOS_ABOVE;
            if (glyph->u.notes.liquescentia & L_DEMINUTUS) {
                v_episemus = below_if_next_ambitus_allows(note);
            } else {
                v_episemus = VPOS_BELOW;
            }
            break;
        default:
            high_low_set_lower(glyph, note);
            note->gtex_offset_case = last_note_case(glyph, FinalPunctum, note,
                    false);
            h_episemus = above_if_h_episemus(note->previous);
            v_episemus = VPOS_BELOW;
            break;
        }
        break;
    default: /* case T_ONE_NOTE */
        h_episemus = VPOS_ABOVE;
        v_episemus = VPOS_BELOW;
        switch (note->u.note.shape) {
        case S_PUNCTUM_INCLINATUM_DEMINUTUS:
            note->gtex_offset_case = FinalInclinatumDeminutus;
            break;
        case S_PUNCTUM_INCLINATUM_AUCTUS:
        case S_PUNCTUM_INCLINATUM:
        case S_PUNCTUM_CAVUM_INCLINATUM_AUCTUS:
        case S_PUNCTUM_CAVUM_INCLINATUM:
            note->gtex_offset_case = FinalInclinatum;
            break;
        case S_STROPHA:
            note->gtex_offset_case = FinalStropha;
            break;
        case S_QUILISMA:
            note->gtex_offset_case = FinalQuilisma;
            break;
        case S_ORISCUS:
        case S_ORISCUS_AUCTUS:
        case S_ORISCUS_DEMINUTUS:
            note->gtex_offset_case = FinalOriscus;
            break;
        case S_VIRGA:
            note->gtex_offset_case = InitialVirga;
            break;
        case S_VIRGA_REVERSA:
            if (glyph->u.notes.liquescentia == L_AUCTUS_DESCENDENS) {
                note->gtex_offset_case = FinalVirgaAuctus;
            } else {
                note->gtex_offset_case = InitialVirga;
            }
            break;
        case S_LINEA_PUNCTUM:
        case S_LINEA_PUNCTUM_CAVUM:
            note->gtex_offset_case = FinalLineaPunctum;
            break;
        case S_LINEA:
            note->gtex_offset_case = FinalPunctum;
            break;
        default:
            note->gtex_offset_case = last_note_case(glyph, FinalPunctum, note,
                    true);
            break;
        }
        break;
    }

    if (note->signs & _V_EPISEMUS) {
        if (note->v_episemus_height) {
            if (note->v_episemus_height > note->u.note.pitch) {
                /* above is always higher because of GregorioTeX's design */
                note->v_episemus_height += (int)VPOS_ABOVE;
            }
        }
        else {
            note->v_episemus_height = note->u.note.pitch + (int)v_episemus;
            if ((v_episemus == VPOS_BELOW && v_episemus_below_is_lower)
                    || v_episemus == VPOS_ABOVE) {
                /* above is always higher because of GregorioTeX's design */
                note->v_episemus_height += (int)v_episemus;
            }
        }
    }

    return h_episemus;
}

static __inline char compute_h_episemus_height(
        const gregorio_glyph *const glyph, const gregorio_note *const note,
        const gregorio_vposition vpos)
{
    char height = note->u.note.pitch;

    if (note->signs & _V_EPISEMUS) {
        if ((vpos == VPOS_ABOVE && note->v_episemus_height >= height)
                || (vpos == VPOS_BELOW && note->v_episemus_height <= height)) {
            height = note->v_episemus_height;
        }
    }

    if (vpos == VPOS_ABOVE && note->choral_sign
            && !choral_sign_here_is_low(glyph, note, NULL)) {
        ++height;
        if (!is_on_a_line(note->u.note.pitch)) {
            ++height;
        }
    }

    return height + (int)vpos;
}

static bool is_bridgeable_space(const gregorio_element *const element)
{
    if (element->type == GRE_SPACE) {
        switch (element->u.misc.unpitched.info.space) {
        case SP_NEUMATIC_CUT:
        case SP_LARGER_SPACE:
        case SP_NEUMATIC_CUT_NB:
        case SP_LARGER_SPACE_NB:
            return true;
        default:
            /* do nothing */
            break;
        }
    }

    return false;
}

typedef struct height_computation {
    const gregorio_vposition vpos;
    bool (*const is_applicable)(const gregorio_note *);
    bool (*const is_shown)(const gregorio_note *);
    bool (*const is_connected)(const gregorio_note *);
    grehepisemus_size (*const get_size)(const gregorio_note *);
    bool (*const is_better_height)(signed char, signed char);
    void (*const position)(gregorio_note *, signed char, bool);

    bool active;
    char height;
    bool connected;
    const gregorio_element *start_element;
    const gregorio_glyph *start_glyph;
    gregorio_note *start_note;
    gregorio_note *last_connected_note;
} height_computation;

static bool is_h_episemus_above_applicable(const gregorio_note *const note)
{
    return note->h_episemus_above || !note->is_lower_note;
}

static bool is_h_episemus_below_applicable(const gregorio_note *const note)
{
    return note->h_episemus_below || !note->is_upper_note;
}

bool gtex_is_h_episemus_above_shown(const gregorio_note *const note)
{
    return note->h_episemus_above;
}

bool gtex_is_h_episemus_below_shown(const gregorio_note *const note)
{
    return note->h_episemus_below;
}

static bool is_h_episemus_above_connected(const gregorio_note *const note)
{
    return note->h_episemus_above_connect;
}

static bool is_h_episemus_below_connected(const gregorio_note *const note)
{
    return note->h_episemus_below_connect;
}

static grehepisemus_size get_h_episemus_above_size(
        const gregorio_note *const note)
{
    return note->h_episemus_above_size;
}

static grehepisemus_size get_h_episemus_below_size(
        const gregorio_note *const note)
{
    return note->h_episemus_below_size;
}

static bool is_h_episemus_above_better_height(const signed char new_height,
        const signed char old_height)
{
    return new_height > old_height;
}

static bool is_h_episemus_below_better_height(const signed char new_height,
        const signed char old_height)
{
    return new_height < old_height;
}

static __inline void start_h_episemus(height_computation *const h,
        const gregorio_element *const element,
        const gregorio_glyph *const glyph, gregorio_note *const note)
{
    h->start_element = element;
    h->start_glyph = glyph;
    h->start_note = note;
    h->active = true;
    h->height = compute_h_episemus_height(glyph, note, h->vpos);
}

static __inline void set_h_episemus_height(const height_computation *const h,
        gregorio_note *const end)
{
    gregorio_note *last_note = NULL;

    const gregorio_element *element = h->start_element;
    const gregorio_glyph *glyph = h->start_glyph;
    gregorio_note *note = h->start_note;

    for ( ; element; element = element->next) {
        if (element->type == GRE_ELEMENT) {
            for (glyph = glyph? glyph : element->u.first_glyph; glyph;
                    glyph = glyph->next) {
                if (glyph && glyph->type == GRE_GLYPH) {
                    for (note = note? note : glyph->u.notes.first_note; note;
                            note = note->next) {
                        if (end && note == end) {
                            if (last_note) {
                                h->position(last_note, h->height, false);
                            }
                            return;
                        }
                        if (h->is_applicable(note)) {
                            h->position(note, h->height, true);
                            last_note = note;
                        }
                    }
                }
            }
        } else if (!is_bridgeable_space(element)) {
            break;
        }
    }

    if (last_note) {
        h->position(last_note, h->height, false);
    }
}

static __inline bool is_connected_left(const grehepisemus_size size) {
    return size == H_NORMAL || size == H_SMALL_LEFT;
}

static __inline bool is_connected_right(const grehepisemus_size size) {
    return size == H_NORMAL || size == H_SMALL_RIGHT;
}

static __inline bool is_connectable_interglyph_ambitus(
        const gregorio_note *const first, const gregorio_note *const second)
{
    return first && second
            && abs(first->u.note.pitch - second->u.note.pitch) < 3;
}

static __inline bool has_space_to_left(const gregorio_note *const note) {
    switch (note->u.note.shape) {
    case S_PUNCTUM_INCLINATUM:
    case S_PUNCTUM_INCLINATUM_DEMINUTUS:
    case S_PUNCTUM_INCLINATUM_AUCTUS:
        return !is_connectable_interglyph_ambitus(note->previous, note);

    default:
        return !note->previous;
    }
}

static __inline void end_h_episemus(height_computation *const h,
        gregorio_note *const end)
{
    signed char proposed_height;

    if (h->active) {
        /* don't let the episemus clash with the note before or after */
        if (is_connected_left(h->get_size(h->start_note))
                && h->start_note->previous
                && h->start_note->previous->type == GRE_NOTE
                && is_connected_right(h->get_size(h->start_note->previous))
                && !has_space_to_left(h->start_note)) {
            proposed_height = h->start_note->previous->u.note.pitch + h->vpos;
            if (h->is_better_height(proposed_height, h->height)) {
                h->height = proposed_height;
            }
        }
        /* end->previous checks that it's within the same glyph */
        if (end && end->type == GRE_NOTE && end->previous
                && end->previous->type == GRE_NOTE
                && is_connected_left(h->get_size(end))
                && !has_space_to_left(end) && h->last_connected_note
                && is_connected_right(h->get_size(h->last_connected_note))) {
            proposed_height = end->u.note.pitch + h->vpos;
            if (h->is_better_height(proposed_height, h->height)) {
                h->height = proposed_height;
            }
        }

        set_h_episemus_height(h, end);

        h->active = false;
        h->height = 0;
        h->connected = false;
        h->start_element = NULL;
        h->start_glyph = NULL;
        h->start_note = NULL;
        h->last_connected_note = NULL;
    }
}

static __inline void compute_h_episemus(height_computation *const h,
        const gregorio_element *const element,
        const gregorio_glyph *const glyph, gregorio_note *const note,
        const int i)
{
    signed char next_height;
    grehepisemus_size size;

    if (h->is_applicable(note)) {
        if (h->is_shown(note)) {
            size = h->get_size(note);

            if (h->active) {
                if (h->connected && is_connected_left(size)
                        && (i != 1 || is_connectable_interglyph_ambitus(
                                note, h->last_connected_note))) {
                    next_height = compute_h_episemus_height(glyph, note,
                            h->vpos);
                    if (h->is_better_height(next_height, h->height)) {
                        h->height = next_height;
                    }
                }
                else {
                    end_h_episemus(h, note);
                    start_h_episemus(h, element, glyph, note);
                }
            } else {
                start_h_episemus(h, element, glyph, note);
            }

            h->connected = h->is_connected(note) && is_connected_right(size);
            h->last_connected_note = note;
        } else {
            end_h_episemus(h, note);
        }
    }
}

static __inline void compute_note_positioning(height_computation *const above,
        height_computation *const below, const gregorio_element *const element,
        const gregorio_glyph *const glyph, gregorio_note *const note,
        const int i, const gtex_type type)
{
    gregorio_vposition default_vpos = advise_positioning(glyph, note, i, type);

    if (note->h_episemus_above == HEPISEMUS_AUTO
            && note->h_episemus_below == HEPISEMUS_AUTO) {
        if (default_vpos == VPOS_BELOW) {
            note->h_episemus_above = HEPISEMUS_NONE;
        }
        else { /* default_vpos == VPOS_ABOVE */
            note->h_episemus_below = HEPISEMUS_NONE;
        }
    }

    compute_h_episemus(above, element, glyph, note, i);
    compute_h_episemus(below, element, glyph, note, i);
}

void gregoriotex_compute_positioning(const gregorio_element *element)
{
    height_computation above = {
        /*.vpos =*/ VPOS_ABOVE,
        /*.is_applicable =*/ &is_h_episemus_above_applicable,
        /*.is_shown =*/ &gtex_is_h_episemus_above_shown,
        /*.is_connected =*/ &is_h_episemus_above_connected,
        /*.get_size =*/ &get_h_episemus_above_size,
        /*.is_better_height =*/ &is_h_episemus_above_better_height,
        /*.position =*/ &gregorio_position_h_episemus_above,

        /*.active =*/ false,
        /*.height =*/ 0,
        /*.connected =*/ false,
        /*.start_element =*/ NULL,
        /*.start_glyph =*/ NULL,
        /*.start_note =*/ NULL,
        /*.last_connected_note =*/ NULL,
    };
    height_computation below = {
        /*.vpos =*/ VPOS_BELOW,
        /*.is_applicable =*/ &is_h_episemus_below_applicable,
        /*.is_shown =*/ &gtex_is_h_episemus_below_shown,
        /*.is_connected =*/ &is_h_episemus_below_connected,
        /*.get_size =*/ &get_h_episemus_below_size,
        /*.is_better_height =*/ &is_h_episemus_below_better_height,
        /*.position =*/ &gregorio_position_h_episemus_below,

        /*.active =*/ false,
        /*.height =*/ 0,
        /*.connected =*/ false,
        /*.start_element =*/ NULL,
        /*.start_glyph =*/ NULL,
        /*.start_note =*/ NULL,
        /*.last_connected_note =*/ NULL,
    };
    int i;
    gtex_alignment ignored;
    gtex_type type;

    for (; element; element = element->next) {
        if (element->type == GRE_ELEMENT) {
            const gregorio_glyph *glyph;
            for (glyph = element->u.first_glyph; glyph;
                    glyph = glyph->next) {
                if (glyph->type == GRE_GLYPH) {
                    gregorio_note *note;
                    i = 0;
                    gregoriotex_determine_glyph_name(glyph, element, &ignored,
                            &type);
                    for (note = glyph->u.notes.first_note; note;
                            note = note->next) {
                        if (note->type == GRE_NOTE) {
                            compute_note_positioning(&above, &below, element,
                                    glyph, note, ++i, type);
                        }
                    }
                }
            }
        } else if (!is_bridgeable_space(element)) {
            end_h_episemus(&above, NULL);
            end_h_episemus(&below, NULL);
        }
    }
    end_h_episemus(&above, NULL);
    end_h_episemus(&below, NULL);
}

