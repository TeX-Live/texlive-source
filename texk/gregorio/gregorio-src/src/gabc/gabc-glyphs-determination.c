/*
 * Gregorio is a program that translates gabc files to GregorioTeX
 * This file provides functions for determining glyphs from notes.
 *
 * Copyright (C) 2006-2015 The Gregorio Project (see CONTRIBUTORS.md)
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
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <stdio.h>
#include <assert.h>
#include "bool.h"
#include "struct.h"
#include "messages.h"

#include "gabc.h"

static __inline gregorio_scanner_location *copy_note_location(
        const gregorio_note *const note, gregorio_scanner_location *const loc)
{
    loc->first_line = note->src_line;
    loc->first_column = note->src_column;
    loc->first_offset = note->src_offset;
    loc->last_line = note->src_line;
    loc->last_column = note->src_column;
    loc->last_offset = note->src_offset;
    return loc;
}

/****************************
 *
 * First see the comments of
 * gabc_det_glyphs_from_notes. This function is used when
 * we have finished to determine a glyph. We have the last glyph that
 * have been added: last_glyph. The glyph we want to add is given by
 * glyph_type and liquescentia.
 *
 * The glyph we want to add goes from first_note to current_note, we
 * isolate these notes from the notes that won't be in the glyph, and
 * we add the glyph to the list_of_glyphs.
 *
****************************/

static gregorio_note *close_glyph(gregorio_glyph **last_glyph,
        gregorio_glyph_type glyph_type, gregorio_note **first_note,
        gregorio_liquescentia liquescentia, gregorio_note *current_note)
{
    gregorio_note *new_current_note = current_note;
    gregorio_scanner_location loc;
    /* a variable necessary for the patch for G_BIVIRGA & co. */
    gregorio_note *added_notes = NULL;
    gregorio_note *next_note = NULL;

    /* patch to have good glyph type in the case where a glyph ends by a note
     * with shape S_QUADRATUM */
    if (glyph_type == G_PES_QUADRATUM_FIRST_PART
            || glyph_type == G_PES_QUILISMA_QUADRATUM_FIRST_PART) {
        glyph_type = G_PUNCTUM;
    }

    gregorio_add_glyph(last_glyph, glyph_type, *first_note, liquescentia);
    if (current_note->next) {
        current_note->next->previous = NULL;
        *first_note = current_note->next;
        current_note->next = NULL;
    }
    /* here we "patch" the structure for bivirga, tristropha, etc. */
    /* the idea is not to have a S_BIVIRGA in the shape of the note (which is
     * dirty) but rather a G_BIVIRGA in the glyph (which is the case now) and
     * two virgas */

    if (glyph_type == G_BIVIRGA || glyph_type == G_DISTROPHA
            || glyph_type == G_TRIVIRGA || glyph_type == G_TRISTROPHA
            || glyph_type == G_DISTROPHA_AUCTA
            || glyph_type == G_TRISTROPHA_AUCTA) {
        gregorio_go_to_first_note(&current_note);
        while (current_note) {
            if (current_note->type == GRE_NOTE) {
                switch (current_note->u.note.shape) {
                case S_TRIVIRGA:
                    gregorio_add_note(&added_notes, current_note->u.note.pitch,
                            S_VIRGA, _NO_SIGN, L_NO_LIQUESCENTIA, current_note,
                            copy_note_location(current_note, &loc));
                case S_BIVIRGA:
                    gregorio_add_note(&added_notes, current_note->u.note.pitch,
                            S_VIRGA, _NO_SIGN, L_NO_LIQUESCENTIA, current_note,
                            copy_note_location(current_note, &loc));
                    gregorio_add_note(&added_notes, current_note->u.note.pitch,
                            S_VIRGA, current_note->signs,
                            current_note->u.note.liquescentia, current_note,
                            copy_note_location(current_note, &loc));
                    break;
                case S_TRISTROPHA:
                    gregorio_add_note(&added_notes, current_note->u.note.pitch,
                            S_STROPHA, _NO_SIGN, L_NO_LIQUESCENTIA,
                            current_note,
                            copy_note_location(current_note, &loc));
                case S_DISTROPHA:
                    gregorio_add_note(&added_notes, current_note->u.note.pitch,
                            S_STROPHA, _NO_SIGN, L_NO_LIQUESCENTIA,
                            current_note,
                            copy_note_location(current_note, &loc));
                    gregorio_add_note(&added_notes, current_note->u.note.pitch,
                            S_STROPHA, current_note->signs,
                            current_note->u.note.liquescentia, current_note,
                            copy_note_location(current_note, &loc));
                    break;
                case S_TRISTROPHA_AUCTA:
                    gregorio_add_note(&added_notes, current_note->u.note.pitch,
                            S_STROPHA, _NO_SIGN, L_NO_LIQUESCENTIA,
                            current_note,
                            copy_note_location(current_note, &loc));
                case S_DISTROPHA_AUCTA:
                    gregorio_add_note(&added_notes, current_note->u.note.pitch,
                            S_STROPHA, _NO_SIGN, L_NO_LIQUESCENTIA,
                            current_note,
                            copy_note_location(current_note, &loc));
                    gregorio_add_note(&added_notes, current_note->u.note.pitch,
                            S_STROPHA_AUCTA, current_note->signs,
                            current_note->u.note.liquescentia, current_note,
                            copy_note_location(current_note, &loc));
                    break;
                default:
                    break;
                }
            }
            /* this is the case of two separate virga that have been spotted
             * as a bivirga */
            if (!added_notes) {
                break;
            }
            next_note = current_note->next;
            /* now we have what we want, we set up the links and free the old
             * note */
            if (next_note) {
                current_note->next->previous = added_notes;
                added_notes->next = next_note;
            }
            gregorio_go_to_first_note(&added_notes);
            if (current_note->previous) {
                current_note->previous->next = added_notes;
                added_notes->previous = current_note->previous;
            } else {
                new_current_note = added_notes;
            }
            /* Detaching current_note is not strictly necessary here because we
             * are effectively plucking out added_notes into its own glyph;
             * however, detaching the note is safer if this behavior changes in
             * the future because gregorio_free_one_note nullifies surrounding
             * pointers */
            current_note->next = NULL;
            current_note->previous = NULL;
            gregorio_free_one_note(&current_note);
            if (!next_note) {
                current_note = added_notes;
                break;
            }
            current_note = next_note;
        }
        gregorio_go_to_first_note(&current_note);
        /* finally we set the just added glyph first_note to current_note */
        (*last_glyph)->u.notes.first_note = current_note;
    }
    return new_current_note;
}

/* a small function to automatically determine the pitch of a custo : it is
 * the pitch of the next note, but we must take care of the clef changes, as
 * custo are (normally and for now) only present before clef changes. */
/* TODO: there may be a side effect with the flated keys... */

static char gabc_determine_custo_pitch(gregorio_note *current_note,
        int current_key)
{
    int pitch_difference = 0;
    int newkey;
    while (current_note) {
        if (current_note->type == GRE_C_KEY_CHANGE
                || current_note->type == GRE_C_KEY_CHANGE_FLATED) {
            newkey = gregorio_calculate_new_key(C_KEY,
                    current_note->u.note.pitch - '0');
            pitch_difference = newkey - current_key;
        }
        if (current_note->type == GRE_F_KEY_CHANGE
                || current_note->type == GRE_F_KEY_CHANGE_FLATED) {
            newkey = gregorio_calculate_new_key(F_KEY,
                    current_note->u.note.pitch - '0');
            pitch_difference = newkey - current_key;
        }
        if (current_note->type == GRE_NOTE) {
            pitch_difference =
                    (int) current_note->u.note.pitch - pitch_difference;
            while (pitch_difference < LOWEST_PITCH) {
                pitch_difference += 7;
            }
            while (pitch_difference > HIGHEST_PITCH) {
                pitch_difference -= 7;
            }
            assert(pitch_difference >= LOWEST_PITCH
                    && pitch_difference <= HIGHEST_PITCH);
            return (char) pitch_difference;
        }
        current_note = current_note->next;
    }
    return DUMMY_PITCH;
}

/****************************
 *
 * This function is the basis of all the determination of glyphs. The
 * phylosophy of the function is to say : We have a glyph that we have
 * determined, and we have the following note, can we "add" it to the
 * glyph, or will it be the first note of another glyph ?
 *
 * current_glyph_type is the type of the current_glyph (current_glyph
 * as meant in gabc_det_glyphs_from_notes).  curent_pitch
 * is the height of the note that we want to "add" to the glyph.
 * last_pitch is the height of the last note of current_glyph.  shape
 * is the shape of the note we want to "add" to the glyph.
 *
 * The function returns a char, which meaning is explained below.
 *
 * end_of_glyph is a pointer to the result of the determination, here
 * are the meanings :
 *
 * DET_NO_END: we have successfully added the note to the glyph, and
 * we return the new type of the glyph. We may again add notes to the
 * glyph.
 *
 * DET_END_OF_PREVIOUS: we have not been able to add the note to the
 * glyph, so we will have to close the glyph.
 *
 * DET_END_OF_CURRENT: we have been able to add the note to the glyph,
 * but we won't be able to add more notes to the glyph, we can close
 * it. The new type is returned.

 * DET_END_OF_BOTH: we have'nt been able to add the note to the glyph,
 * and we won't be able to add notes to the new glyph. This special
 * case is quite rare, we use it for trivirga, tristropha, etc.

 * When we encouter a S_QUADRATUM (or S_QUILISMA_QUADRATUM), we build
 * a new glyph with the (temporary) shape G_PES_QUADRATUM_FIRST_PART
 * (or G_PES_QUILISMA_QUADRATUM_FIRST_PART), and we wait for the next
 * note.
 *
****************************/

static char gregorio_add_note_to_a_glyph(gregorio_glyph_type current_glyph_type,
        char current_pitch, char last_pitch, gregorio_shape shape,
        gregorio_liquescentia liquescentia,
        gregorio_note *current_glyph_first_note,
        gabc_determination * end_of_glyph)
{

    /* next glyph type is the type of the glyph that will be returned (the
     * new type of the glyph with the note added to it, or the type of the
     * new glyph with the note alone. */
    gregorio_glyph_type next_glyph_type = G_UNDETERMINED;

    *end_of_glyph = DET_NO_END;

    /* here we separate notes that would be logically in the same glyph
     * but that are too far to be so */
    if (last_pitch) {
        if (current_pitch - last_pitch > MAX_INTERVAL
                || current_pitch - last_pitch < -MAX_INTERVAL) {
            current_glyph_type = G_UNDETERMINED;
        }
    }

    switch (shape) {
    case S_PUNCTUM_CAVUM:
    case S_LINEA_PUNCTUM:
    case S_LINEA_PUNCTUM_CAVUM:
    case S_LINEA:
        next_glyph_type = G_PUNCTUM;
        *end_of_glyph = DET_END_OF_BOTH;
        break;
    case S_PUNCTUM:
        /*
         * we determine here the shape of the thing if it is made of puncta
         */
        if (current_pitch == last_pitch) {
            next_glyph_type = G_PUNCTUM;
            *end_of_glyph = DET_END_OF_PREVIOUS;
            break;
        }
        switch (current_glyph_type) {
        case G_PUNCTUM:
            if (current_pitch > last_pitch) {
                next_glyph_type = G_PODATUS;
            } else {
                next_glyph_type = G_FLEXA;
            }
            break;
        case G_PODATUS:
            if (current_pitch > last_pitch) {
                if (current_glyph_first_note->u.note.shape == S_PUNCTUM &&
                        current_glyph_first_note->u.note.liquescentia !=
                        L_INITIO_DEBILIS) {
                    next_glyph_type = G_SCANDICUS;
                    *end_of_glyph = DET_END_OF_CURRENT;
                } else {
                    next_glyph_type = G_PUNCTUM;
                    *end_of_glyph = DET_END_OF_PREVIOUS;
                }
            } else {
                next_glyph_type = G_TORCULUS;
            }
            break;
        case G_PES_QUADRATUM_FIRST_PART:
            if (current_pitch > last_pitch) {
                next_glyph_type = G_PES_QUADRATUM;
                *end_of_glyph = DET_END_OF_CURRENT;
            } else {
                next_glyph_type = G_FLEXA;
            }
            break;
        case G_PES_QUILISMA_QUADRATUM_FIRST_PART:
            if (current_pitch > last_pitch) {
                next_glyph_type = G_PES_QUADRATUM;
                *end_of_glyph = DET_END_OF_CURRENT;
            } else {
                next_glyph_type = G_PUNCTUM;
                *end_of_glyph = DET_END_OF_PREVIOUS;
            }
            break;
        case G_VIRGA_STRATA:
            if (current_pitch > last_pitch) {
                next_glyph_type = G_SALICUS;
                *end_of_glyph = DET_END_OF_CURRENT;
            } else {
                next_glyph_type = G_PUNCTUM;
                *end_of_glyph = DET_END_OF_PREVIOUS;
            }
            break;
        case G_FLEXA:
            if (current_pitch > last_pitch) {
                next_glyph_type = G_PORRECTUS;
            } else {
                *end_of_glyph = DET_END_OF_CURRENT;
                next_glyph_type = G_ANCUS;
            }
            break;
        case G_TORCULUS:
            if (current_pitch > last_pitch) {
                next_glyph_type = G_TORCULUS_RESUPINUS;
            } else if (liquescentia == L_DEMINUTUS) {
                *end_of_glyph = DET_END_OF_CURRENT;
                next_glyph_type = G_TORCULUS_LIQUESCENS;
            } else {
                *end_of_glyph = DET_END_OF_PREVIOUS;
                next_glyph_type = G_PUNCTUM;
            }
            break;
        case G_TORCULUS_RESUPINUS:
            if (current_pitch > last_pitch) {
                *end_of_glyph = DET_END_OF_PREVIOUS;
                next_glyph_type = G_PUNCTUM;
            } else {
                *end_of_glyph = DET_END_OF_CURRENT;
                next_glyph_type = G_TORCULUS_RESUPINUS_FLEXUS;
            }
            break;
        case G_PORRECTUS:
            if (current_pitch > last_pitch) {
                *end_of_glyph = DET_END_OF_PREVIOUS;
                next_glyph_type = G_PUNCTUM;
            } else {
                *end_of_glyph = DET_END_OF_CURRENT;
                next_glyph_type = G_PORRECTUS_FLEXUS;
            }
            break;
        default:
            *end_of_glyph = DET_END_OF_PREVIOUS;
            next_glyph_type = G_PUNCTUM;
            break;
        }
        break;
    case S_ORISCUS:
        *end_of_glyph = DET_END_OF_PREVIOUS;
        next_glyph_type = G_PUNCTUM;
        break;
    case S_ORISCUS_AUCTUS:
        *end_of_glyph = DET_END_OF_PREVIOUS;
        next_glyph_type = G_PUNCTUM;
        break;
    case S_ORISCUS_DEMINUTUS:
        *end_of_glyph = DET_END_OF_PREVIOUS;
        next_glyph_type = G_PUNCTUM;
        break;
    case S_QUILISMA:
        *end_of_glyph = DET_END_OF_PREVIOUS;
        next_glyph_type = G_PUNCTUM;
        break;
    case S_VIRGA:
        if (current_glyph_type == G_VIRGA && last_pitch == current_pitch) {
            next_glyph_type = G_BIVIRGA;
        } else {
            if (current_glyph_type == G_BIVIRGA && last_pitch == current_pitch) {
                next_glyph_type = G_TRIVIRGA;
            } else {
                *end_of_glyph = DET_END_OF_PREVIOUS;
                next_glyph_type = G_VIRGA;
            }
        }
        break;
    case S_VIRGA_REVERSA:
        *end_of_glyph = DET_END_OF_PREVIOUS;
        next_glyph_type = G_VIRGA_REVERSA;
        break;
    case S_BIVIRGA:
        if (current_glyph_type == G_VIRGA && last_pitch == current_pitch) {
            next_glyph_type = G_TRIVIRGA;
        } else {
            *end_of_glyph = DET_END_OF_PREVIOUS;
            next_glyph_type = G_BIVIRGA;
        }
        break;
    case S_TRIVIRGA:
        *end_of_glyph = DET_END_OF_BOTH;
        next_glyph_type = G_TRIVIRGA;
        break;
    case S_QUADRATUM:
        *end_of_glyph = DET_END_OF_PREVIOUS;
        next_glyph_type = G_PES_QUADRATUM_FIRST_PART;
        break;
    case S_QUILISMA_QUADRATUM:
        *end_of_glyph = DET_END_OF_PREVIOUS;
        next_glyph_type = G_PES_QUILISMA_QUADRATUM_FIRST_PART;
        break;
    case S_ORISCUS_SCAPUS:
        if (current_glyph_type == G_PUNCTUM && last_pitch < current_pitch) {
            next_glyph_type = G_VIRGA_STRATA;
        } else {
            *end_of_glyph = DET_END_OF_PREVIOUS;
            next_glyph_type = G_PUNCTUM;
        }
        break;
    case S_PUNCTUM_INCLINATUM:
    case S_PUNCTUM_CAVUM_INCLINATUM:
        /*
         * Warning : this part of the code is specific to the
         * declarations of the header file
         */
        if (current_glyph_type > G_PUNCTA_INCLINATA) {
            /*
             * meaning that the previous glyph is not a combination of puncta
             * inclinata, see header file for details
             */
            *end_of_glyph = DET_END_OF_PREVIOUS;
            next_glyph_type = G_PUNCTUM_INCLINATUM;
            break;
        }
        if (current_pitch == last_pitch) {
            *end_of_glyph = DET_END_OF_PREVIOUS;
            next_glyph_type = G_PUNCTUM_INCLINATUM;
            break;
        }
        switch (current_glyph_type) {
        case G_PUNCTUM_INCLINATUM:
            if (last_pitch < current_pitch) {
                next_glyph_type = G_2_PUNCTA_INCLINATA_ASCENDENS;
            } else {
                next_glyph_type = G_2_PUNCTA_INCLINATA_DESCENDENS;
            }
            break;
        case G_2_PUNCTA_INCLINATA_ASCENDENS:
            if (last_pitch < current_pitch) {
                next_glyph_type = G_3_PUNCTA_INCLINATA_ASCENDENS;
            } else {
                next_glyph_type = G_TRIGONUS;
            }
            break;
        case G_3_PUNCTA_INCLINATA_ASCENDENS:
            if (last_pitch < current_pitch) {
                next_glyph_type = G_4_PUNCTA_INCLINATA_ASCENDENS;
            } else {
                next_glyph_type = G_PUNCTA_INCLINATA;
            }
            break;
        case G_4_PUNCTA_INCLINATA_ASCENDENS:
            if (last_pitch < current_pitch) {
                next_glyph_type = G_5_PUNCTA_INCLINATA_ASCENDENS;
            } else {
                next_glyph_type = G_PUNCTA_INCLINATA;
            }
            break;
        case G_2_PUNCTA_INCLINATA_DESCENDENS:
            if (last_pitch < current_pitch) {
                next_glyph_type = G_TRIGONUS;
            } else {
                next_glyph_type = G_3_PUNCTA_INCLINATA_DESCENDENS;
            }
            break;
        case G_3_PUNCTA_INCLINATA_DESCENDENS:
            if (last_pitch < current_pitch) {
                next_glyph_type = G_PUNCTA_INCLINATA;
            } else {
                next_glyph_type = G_4_PUNCTA_INCLINATA_DESCENDENS;
            }
            break;
        case G_4_PUNCTA_INCLINATA_DESCENDENS:
            if (last_pitch < current_pitch) {
                next_glyph_type = G_PUNCTA_INCLINATA;
            } else {
                next_glyph_type = G_5_PUNCTA_INCLINATA_DESCENDENS;
            }
            break;
        default:
            next_glyph_type = G_PUNCTA_INCLINATA;
            break;
        }
        break;
    case S_STROPHA:
        if (last_pitch != current_pitch) {
            *end_of_glyph = DET_END_OF_PREVIOUS;
            next_glyph_type = G_STROPHA;
            break;
        }
        switch (current_glyph_type) {
        case G_STROPHA:
            next_glyph_type = G_DISTROPHA;
            break;
        case G_DISTROPHA:
            *end_of_glyph = DET_END_OF_CURRENT;
            next_glyph_type = G_TRISTROPHA;
            break;
        default:
            *end_of_glyph = DET_END_OF_PREVIOUS;
            next_glyph_type = G_STROPHA;
            break;
        }
        break;
    case S_DISTROPHA:
        if (last_pitch == current_pitch && current_glyph_type == G_STROPHA) {
            *end_of_glyph = DET_END_OF_CURRENT;
            next_glyph_type = G_TRISTROPHA;
        } else {
            *end_of_glyph = DET_END_OF_PREVIOUS;
            next_glyph_type = G_DISTROPHA;
        }
        break;
    case S_TRISTROPHA:
        *end_of_glyph = DET_END_OF_BOTH;
        next_glyph_type = G_TRISTROPHA;
        break;
    default:
        break;
    }
    /* end of the main switch */

    if (current_glyph_type == G_UNDETERMINED) {
        /*
         * means that this is the first glyph or that the previous glyph has
         * already been added
         */
        if (*end_of_glyph == DET_END_OF_PREVIOUS) {
            *end_of_glyph = DET_NO_END;
        } else {
            if (*end_of_glyph == DET_END_OF_BOTH) {
                *end_of_glyph = DET_END_OF_CURRENT;
            }
        }
    }

    /*
     * WARNING : Ugly section of the code, just some kind of patch for it to work
     * with fonts that can't handle large intervals.
     */
    /* here we separate notes that would be logically in the same glyph
     * but that are too far to be so, we already said to the function that
     * it was not the same glyph, there we must say that the previous
     * glyph has ended. */
    if (last_pitch) {
        if (current_pitch - last_pitch > MAX_INTERVAL
                || current_pitch - last_pitch < -MAX_INTERVAL) {
            if (*end_of_glyph == DET_END_OF_CURRENT
                    || *end_of_glyph == DET_END_OF_BOTH) {
                *end_of_glyph = DET_END_OF_BOTH;
            } else {
                *end_of_glyph = DET_END_OF_PREVIOUS;
            }
        }
    }

    return next_glyph_type;
}

/****************************
 *
 * Function called with a list of gregorio_notes as argument, this
 * list is determined from gabc notation by the function
 * gabc_det_notes_from_string.
 *
 * In this function we create a list of glyphs, by determining their
 * type according to the sequence of notes (we look at their height,
 * shape, etc.). Each glyph has a pointer gregorio_note *first_note
 * that will be filled with the good note (and notes will be cut as
 * explained in the comments on close_glyph function).
 *
 * Here is how it works :
 * current_glyph is the glyph we are currently determining, that is to
 * say a glyph whose type may change according to the note we will add
 * to it.
 * current_note is the note we are currently dealing with.
 * Let's take an example, if the list of notes is ab, the first step
 * (current_glyph: undetermined, current_note: a) will be to add a to
 * nothing, that will give a glyph "punctum" containing only one note:
 * a. In the second step (current_glyph: punctum, current_note: b) we
 * transform the glyph in "pes".
 * For special notes like a key change or something, we stop the
 * determination of the current glyph, we delete the note and we add
 * it as a glyph.
 *
 * When we determine the glyphs we can encounter the shapes
 * S_QUADRATUM and S_QUILISMA_QUADRATUM, which means that the
 * corresponding note is the first note of a pes quadratum (it can
 * have the shape quilisma for very rare forms). But these shapes must
 * not appear in the final form of the score, and we transform them
 * respectively in punctum and quilisma (and the glyph type must be
 * pes_quadratum, but it is done in gregorio_add_note_to_a_glyph).
 *
****************************/

/* this function updates current_key with the new values (with clef changes) */

gregorio_glyph *gabc_det_glyphs_from_notes(gregorio_note *current_note,
        int *current_key)
{
    /* the first note of the current glyph, to be able to close it well:
     * later we will cut the link (next_notes and previous_note) between
     * this note and the previous one */
    gregorio_note *current_glyph_first_note = current_note;

    /* the last glyph we have totally determined. It is automatically
     * updated by close_glyph(). */
    gregorio_glyph *last_glyph = NULL;

    /* type of the glyph we are currently determining */
    gregorio_glyph_type current_glyph_type = G_UNDETERMINED;
    gregorio_glyph_type next_glyph_type = G_UNDETERMINED;
    char last_pitch = USELESS_VALUE;
    /* a variable for the signs of bars and to tell if a key is flatted or
     * not */
    gregorio_note *next_note = NULL;

    /* determination of end of glyphs, see comments on
     * gregorio_add_note_to_a_glyph */
    gabc_determination end_of_glyph = DET_NO_END;

    /* a char representing the liquescentia of the current glyph */
    gregorio_liquescentia liquescentia = L_NO_LIQUESCENTIA;

    if (current_note == NULL) {
        return NULL;
    }

    gregorio_go_to_first_note(&current_note);

    while (current_note) {
        next_note = current_note->next;
        if (current_note->type != GRE_NOTE) {
            gregorio_type type = current_note->type;
            char pitch = USELESS_VALUE;
            bool flat = false;
            bool force = false;
            gregorio_sign sign = _NO_SIGN;

            if (current_glyph_type != G_UNDETERMINED) {
                close_glyph(&last_glyph, next_glyph_type,
                        &current_glyph_first_note, liquescentia,
                        current_note->previous);
                current_glyph_type = G_UNDETERMINED;
                liquescentia = L_NO_LIQUESCENTIA;
            }

            switch (type) {
            case GRE_C_KEY_CHANGE:
                pitch = current_note->u.note.pitch;
                *current_key = gregorio_calculate_new_key(C_KEY, pitch - '0');
                break;

            case GRE_F_KEY_CHANGE:
                pitch = current_note->u.note.pitch;
                *current_key = gregorio_calculate_new_key(F_KEY, pitch - '0');
                break;

            case GRE_C_KEY_CHANGE_FLATED:
                pitch = current_note->u.note.pitch;
                *current_key = gregorio_calculate_new_key(C_KEY, pitch - '0');
                type = GRE_C_KEY_CHANGE;
                flat = true;
                break;

            case GRE_F_KEY_CHANGE_FLATED:
                pitch = current_note->u.note.pitch;
                *current_key = gregorio_calculate_new_key(F_KEY, pitch - '0');
                type = GRE_F_KEY_CHANGE;
                flat = true;
                break;

            case GRE_CUSTOS:
                pitch = gabc_determine_custo_pitch(current_note->next,
                        *current_key);
                break;

            case GRE_MANUAL_CUSTOS:
                pitch = current_note->u.note.pitch;
                type = GRE_CUSTOS;
                force = true;
                break;

            case GRE_BAR:
                /* we calculate the signs of the bars */
                if (current_note->signs == _V_EPISEMA) {
                    sign = _V_EPISEMA;
                } else {
                    sign = current_note->special_sign;
                }
                if (current_note->h_episema_above) {
                    if (sign == _V_EPISEMA) {
                        sign = _V_EPISEMA_BAR_H_EPISEMA;
                    } else {
                        sign = _BAR_H_EPISEMA;
                    }
                }
                break;

            case GRE_FLAT:
            case GRE_SHARP:
            case GRE_NATURAL:
                pitch = current_note->u.note.pitch;
                break;

            default:
                /* do nothing */
                break;
            }

            if (pitch == USELESS_VALUE) {
                gregorio_add_unpitched_element_as_glyph(&last_glyph, type,
                        current_note->u.other, sign, current_note->texverb);
            } else {
                gregorio_add_pitched_element_as_glyph(&last_glyph, type, pitch,
                        flat, force, current_note->texverb);
            }
            current_glyph_first_note = current_note->next;
            current_note->texverb = NULL;
            gregorio_free_one_note(&current_note);
            last_pitch = USELESS_VALUE;
            continue;
        }

        /*
         * first we do what must be done with liquescentia
         */
        if (is_initio_debilis(current_note->u.note.liquescentia)) {
            /*
             * meaning that the note is an initio debilis, maybe more
             */
            if (current_glyph_type != G_UNDETERMINED) {
                /*
                 * if it is not the first glyph
                 */
                close_glyph(&last_glyph, current_glyph_type,
                        &current_glyph_first_note,
                        liquescentia, current_note->previous);
                current_glyph_type = G_UNDETERMINED;
            }
            liquescentia = L_INITIO_DEBILIS;
        }

        next_glyph_type =
                gregorio_add_note_to_a_glyph(current_glyph_type,
                current_note->u.note.pitch, last_pitch,
                current_note->u.note.shape,
                current_note->u.note.liquescentia,
                current_glyph_first_note, &end_of_glyph);

        /* patch to have good shapes in the special cases of pes quadratum and
         * pes quilisma quadratum. */
        switch (current_note->u.note.shape) {
        case S_QUADRATUM:
            current_note->u.note.shape = S_PUNCTUM;
            break;

        case S_QUILISMA_QUADRATUM:
            current_note->u.note.shape = S_QUILISMA;
            break;

        default:
            /* do nothing */
            break;
        }

        /* see comments on gregorio_add_note_to_a_glyph for the meaning of
         * end_of_glyph */
        switch (end_of_glyph) {
        case DET_NO_END:
            current_glyph_type = next_glyph_type;
            /*
             * we deal with liquescentia
             */
            if (is_liquescentia(current_note->u.note.liquescentia)) {
                /* special cases of oriscus auctus, treated like normal oriscus
                 * in some cases. */
                if (current_note->u.note.shape == S_ORISCUS_AUCTUS
                        && current_note->next
                        && current_note->next->type == GRE_NOTE
                        && current_note->next->u.note.pitch <
                        current_note->u.note.pitch) {
                    last_pitch = current_note->u.note.pitch;
                    current_note->u.note.shape = S_ORISCUS;
                    current_note->u.note.liquescentia = L_NO_LIQUESCENTIA;
                    current_note = next_note;
                    continue;
                }
                /* special cases of the punctum inclinatum deminutus and
                 * auctus */
                if (current_note->u.note.shape == S_PUNCTUM_INCLINATUM) {
                    if (current_note->u.note.liquescentia == L_DEMINUTUS) {
                        current_note->u.note.shape =
                                S_PUNCTUM_INCLINATUM_DEMINUTUS;
                    }
                    if (current_note->u.note.liquescentia == L_AUCTA
                            || current_note->u.note.liquescentia ==
                            L_AUCTUS_DESCENDENS
                            || current_note->u.note.liquescentia ==
                            L_AUCTUS_ASCENDENS) {
                        current_note->u.note.shape =
                                S_PUNCTUM_INCLINATUM_AUCTUS;
                    }

                    if (current_note->next
                            && current_note->next->type == GRE_NOTE
                            && current_note->next->u.note.shape ==
                            S_PUNCTUM_INCLINATUM
                            && current_note->next->u.note.liquescentia ==
                            L_DEMINUTUS) {
                        last_pitch = current_note->u.note.pitch;
                        current_note = next_note;
                        continue;
                    }
                }
                if (current_note->u.note.shape == S_PUNCTUM_CAVUM_INCLINATUM) {
                    switch (current_note->u.note.liquescentia) {
                    case L_AUCTA:
                    case L_AUCTUS_DESCENDENS:
                    case L_AUCTUS_ASCENDENS:
                    case L_AUCTUS_DESCENDENS_INITIO_DEBILIS:
                    case L_AUCTUS_ASCENDENS_INITIO_DEBILIS:
                        current_note->u.note.shape =
                                S_PUNCTUM_CAVUM_INCLINATUM_AUCTUS;
                        break;

                    default:
                        /* do nothing */
                        break;
                    }

                    if (current_note->next
                            && current_note->next->type == GRE_NOTE
                            && current_note->next->u.note.shape ==
                            S_PUNCTUM_INCLINATUM
                            && current_note->next->u.note.liquescentia ==
                            L_DEMINUTUS) {
                        last_pitch = current_note->u.note.pitch;
                        current_note = next_note;
                        continue;
                    }
                }
                liquescentia += current_note->u.note.liquescentia;
                /* once again, only works with the good values in the header
                 * file */
                current_note = close_glyph(&last_glyph, current_glyph_type,
                        &current_glyph_first_note, liquescentia, current_note);
                current_glyph_type = G_UNDETERMINED;
                liquescentia = L_NO_LIQUESCENTIA;
            }
            break;
        case DET_END_OF_PREVIOUS:
            if (current_note->previous)
            {
                /* we don't want to close previous glyph twice */
                close_glyph(&last_glyph, current_glyph_type,
                        &current_glyph_first_note, liquescentia,
                        current_note->previous);
            }
            current_glyph_type = next_glyph_type;
            liquescentia = L_NO_LIQUESCENTIA;
            last_pitch = USELESS_VALUE;
            /* we deal with liquescentia */
            if (is_liquescentia(current_note->u.note.liquescentia))
                /* not an initio debilis, because we considered it in the first
                 * part... */
            {
                /* special cases of the punctum inclinatum deminutus and
                 * auctus */
                if (current_note->u.note.shape == S_PUNCTUM_INCLINATUM) {
                    if (current_note->u.note.liquescentia == L_DEMINUTUS) {
                        current_note->u.note.shape =
                                S_PUNCTUM_INCLINATUM_DEMINUTUS;
                    }
                    if (current_note->u.note.liquescentia == L_AUCTA
                            || current_note->u.note.liquescentia ==
                            L_AUCTUS_DESCENDENS
                            || current_note->u.note.liquescentia ==
                            L_AUCTUS_ASCENDENS) {
                        current_note->u.note.shape =
                                S_PUNCTUM_INCLINATUM_AUCTUS;
                    }
                    if (current_note->next
                            && current_note->next->type == GRE_NOTE
                            && current_note->next->u.note.shape ==
                            S_PUNCTUM_INCLINATUM
                            && current_note->next->u.note.liquescentia ==
                            L_DEMINUTUS) {
                        current_note = next_note;
                        continue;
                    }
                }
                if (current_note->u.note.shape == S_PUNCTUM_CAVUM_INCLINATUM) {
                    switch (current_note->u.note.liquescentia) {
                    case L_AUCTA:
                    case L_AUCTUS_DESCENDENS:
                    case L_AUCTUS_ASCENDENS:
                    case L_AUCTUS_DESCENDENS_INITIO_DEBILIS:
                    case L_AUCTUS_ASCENDENS_INITIO_DEBILIS:
                        current_note->u.note.shape =
                                S_PUNCTUM_CAVUM_INCLINATUM_AUCTUS;
                        break;

                    default:
                        /* do nothing */
                        break;
                    }

                    if (current_note->next
                            && current_note->next->type == GRE_NOTE
                            && current_note->next->u.note.shape ==
                            S_PUNCTUM_INCLINATUM
                            && current_note->next->u.note.liquescentia ==
                            L_DEMINUTUS) {
                        current_note = next_note;
                        continue;
                    }
                }
                current_note = close_glyph(&last_glyph, current_glyph_type,
                        &current_glyph_first_note,
                        current_note->u.note.liquescentia, current_note);
                current_glyph_type = G_UNDETERMINED;
            }
            break;
        case DET_END_OF_CURRENT:
            liquescentia += current_note->u.note.liquescentia;
            /* once again, only works with the good values in the header file */
            current_note = close_glyph(&last_glyph, next_glyph_type,
                    &current_glyph_first_note, liquescentia, current_note);
            current_glyph_type = G_UNDETERMINED;
            liquescentia = L_NO_LIQUESCENTIA;
            break;
        default: /* case DET_END_OF_BOTH: */
            if (current_note->previous)
            {
                /* we don't want to close previous glyph twice */
                close_glyph(&last_glyph, current_glyph_type,
                        &current_glyph_first_note, liquescentia,
                        current_note->previous);
            }
            current_glyph_type = G_UNDETERMINED;
            liquescentia = L_NO_LIQUESCENTIA;
            current_note = close_glyph(&last_glyph, next_glyph_type,
                    &current_glyph_first_note,
                    current_note->u.note.liquescentia, current_note);
            break;
        }

        if (!next_note && current_glyph_type != G_UNDETERMINED) {
            /* we must end the determination here */
            current_note = close_glyph(&last_glyph, current_glyph_type,
                    &current_glyph_first_note, liquescentia, current_note);
        }

        last_pitch = current_note->u.note.pitch;
        current_note = next_note;
    }
    /* end of while */

    gregorio_go_to_first_glyph(&last_glyph);
    return last_glyph;
}
