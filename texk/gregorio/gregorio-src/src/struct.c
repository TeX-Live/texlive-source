/*
 * Gregorio is a program that translates gabc files to GregorioTeX
 * This file implements the Gregorio data structures.
 *
 * Copyright (C) 2006-2015 The Gregorio Project (see CONTRIBUTORS.md)
 *
 * This file is part of Gregorio.
 * 
 * Gregorio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gregorio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gregorio.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief This file contains a set of function to manipulate the gregorio
 * structure. 
 *
 * It starts by simple add/delete functions for almost all
 * structs, and ends with more complex functions for manipulating
 * horizontal episema, keys, etc.
 *
 * The first functions are not commented, but they always act like
 * this : we give them a pointer to the pointer to the current element
 * (by element I mean a struct which can be gregorio_note,
 * gregorio_element, etc.), they add an element and the update the
 * pointer to the element so that it points to the new element (may
 * seem a bit strange).
 *
 * All the delete functions are recursive and free all memory.
 *
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "struct.h"
#include "unicode.h"
#include "messages.h"
#include "support.h"
#include "characters.h"

static gregorio_note *create_and_link_note(gregorio_note **current_note,
        const gregorio_scanner_location *const loc)
{
    gregorio_note *note = gregorio_calloc(1, sizeof(gregorio_note));
    note->previous = *current_note;
    note->next = NULL;
    if (*current_note) {
        (*current_note)->next = note;
    }
    *current_note = note;
    note->src_line = loc->first_line;
    note->src_column = loc->first_column;
    note->src_offset = loc->first_offset;

    return note;
}

void gregorio_position_h_episema_above(gregorio_note *note, signed char height,
        bool connect)
{
    assert(note && (note->type == GRE_NOTE || note->type == GRE_BAR));
    note->h_episema_above = height;
    note->h_episema_above_connect = connect;
}

static void set_h_episema_above(gregorio_note *note, signed char height,
        grehepisema_size size, bool connect)
{
    gregorio_position_h_episema_above(note, height, connect);
    note->h_episema_above_size = size;
}

void gregorio_position_h_episema_below(gregorio_note *note, signed char height,
        bool connect)
{
    assert(note && (note->type == GRE_NOTE || note->type == GRE_BAR));
    note->h_episema_below = height;
    note->h_episema_below_connect = connect;
}

static void set_h_episema_below(gregorio_note *note, signed char height,
        grehepisema_size size, bool connect)
{
    gregorio_position_h_episema_below(note, height, connect);
    note->h_episema_below_size = size;
}

void gregorio_add_note(gregorio_note **current_note, signed char pitch,
        gregorio_shape shape, gregorio_sign signs,
        gregorio_liquescentia liquescentia, gregorio_note *prototype,
        const gregorio_scanner_location *const loc)
{
    gregorio_note *element = create_and_link_note(current_note, loc);
    if (element) {
        element->type = GRE_NOTE;
        element->u.note.pitch = pitch;
        element->u.note.shape = shape;
        element->signs = signs;
        element->special_sign = _NO_SIGN;
        element->u.note.liquescentia = liquescentia;
        if (prototype) {
            set_h_episema_above(element, prototype->h_episema_above,
                    prototype->h_episema_above_size,
                    prototype->h_episema_above_connect);
            set_h_episema_below(element, prototype->h_episema_below,
                    prototype->h_episema_below_size,
                    prototype->h_episema_below_connect);
        }
        element->texverb = NULL;
        element->choral_sign = NULL;
    }
}

static void add_pitched_item_as_note(gregorio_note **current_note,
        gregorio_type type, signed char pitch,
        const gregorio_scanner_location *const loc)
{
    gregorio_note *element = create_and_link_note(current_note, loc);
    if (element) {
        element->type = type;
        element->u.note.pitch = pitch;
    }
}

void gregorio_add_end_of_line_as_note(gregorio_note **current_note,
        gregorio_type sub_type, const gregorio_scanner_location *const loc)
{
    gregorio_note *element = create_and_link_note(current_note, loc);
    if (element) {
        element->type = GRE_END_OF_LINE;
        element->u.other.sub_type = sub_type;
    }
}

void gregorio_add_custo_as_note(gregorio_note **current_note,
        const gregorio_scanner_location *const loc)
{
    gregorio_note *element = create_and_link_note(current_note, loc);
    if (element) {
        element->type = GRE_CUSTOS;
    }
}

void gregorio_add_manual_custos_as_note(gregorio_note **current_note,
        signed char pitch, const gregorio_scanner_location *const loc)
{
    add_pitched_item_as_note(current_note, GRE_MANUAL_CUSTOS, pitch, loc);
}

void gregorio_add_clef_change_as_note(gregorio_note **current_note,
        gregorio_type type, signed char clef_line,
        const gregorio_scanner_location *const loc)
{
    assert(type == GRE_C_KEY_CHANGE || type == GRE_F_KEY_CHANGE
           || type == GRE_C_KEY_CHANGE_FLATED
           || type == GRE_F_KEY_CHANGE_FLATED);
    add_pitched_item_as_note(current_note, type, clef_line, loc);
}

void gregorio_add_bar_as_note(gregorio_note **current_note, gregorio_bar bar,
        const gregorio_scanner_location *const loc)
{
    gregorio_note *element = create_and_link_note(current_note, loc);
    if (element) {
        element->type = GRE_BAR;
        element->u.other.bar = bar;
    }
}

void gregorio_add_alteration_as_note(gregorio_note **current_note,
        gregorio_type type, signed char pitch,
        const gregorio_scanner_location *const loc)
{
    assert(type == GRE_FLAT || type == GRE_SHARP || type == GRE_NATURAL);
    add_pitched_item_as_note(current_note, type, pitch, loc);
}

void gregorio_add_space_as_note(gregorio_note **current_note,
        gregorio_space space, const gregorio_scanner_location *const loc)
{
    gregorio_note *element = create_and_link_note(current_note, loc);
    if (element) {
        element->type = GRE_SPACE;
        element->u.other.space = space;
    }
}

void gregorio_add_texverb_as_note(gregorio_note **current_note, char *str,
        gregorio_type type, const gregorio_scanner_location *const loc)
{
    gregorio_note *element;
    if (str == NULL) {
        return;
    }
    element = create_and_link_note(current_note, loc);
    assert(type == GRE_TEXVERB_GLYPH || type == GRE_TEXVERB_ELEMENT
           || type == GRE_ALT);
    if (element) {
        element->type = type;
        element->texverb = str;
    }
}

void gregorio_add_nlba_as_note(gregorio_note **current_note, gregorio_nlba type,
        const gregorio_scanner_location *const loc)
{
    gregorio_note *element = create_and_link_note(current_note, loc);
    if (element) {
        element->type = GRE_NLBA;
        element->u.other.nlba = type;
    }
}

void gregorio_add_texverb_to_note(gregorio_note **current_note, char *str)
{
    size_t len;
    char *res;
    if (str == NULL) {
        return;
    }
    if (*current_note) {
        if ((*current_note)->texverb) {
            len = strlen((*current_note)->texverb) + strlen(str) + 1;
            res = gregorio_malloc(len * sizeof(char));
            strcpy(res, (*current_note)->texverb);
            strcat(res, str);
            free((*current_note)->texverb);
            free(str);
            (*current_note)->texverb = res;
        } else {
            (*current_note)->texverb = str;
        }
    }
}

void gregorio_add_cs_to_note(gregorio_note *const*const current_note,
        char *const str, const bool nabc)
{
    if (*current_note) {
        (*current_note)->choral_sign = str;
        (*current_note)->choral_sign_is_nabc = nabc;
    }
}

void gregorio_add_special_sign(gregorio_note *note, gregorio_sign sign)
{
    if (!note) {
        /* error */
        return;
    }
    note->special_sign = sign;
}

static void fix_punctum_cavum_inclinatum_liquescentia(gregorio_note *const note)
{
    switch (note->u.note.liquescentia) {
    case L_AUCTUS_ASCENDENS:
    case L_AUCTUS_DESCENDENS:
    case L_AUCTA:
    case L_AUCTUS_ASCENDENS_INITIO_DEBILIS:
    case L_AUCTUS_DESCENDENS_INITIO_DEBILIS:
        note->u.note.liquescentia = L_AUCTA;
        break;
    default:
        note->u.note.liquescentia = L_NO_LIQUESCENTIA;
        break;
    }
}

void gregorio_change_shape(gregorio_note *note, gregorio_shape shape)
{
    if (!note || note->type != GRE_NOTE) {
        gregorio_message(_("trying to change the shape of something that is "
                           "not a note"), "change_shape", VERBOSITY_ERROR, 0);
        return;
    }
    switch (note->u.note.shape) {
    case S_PUNCTUM_INCLINATUM:
        if (shape == S_PUNCTUM_CAVUM) {
            note->u.note.shape = S_PUNCTUM_CAVUM_INCLINATUM;
            fix_punctum_cavum_inclinatum_liquescentia(note);
            break;
        }
        /* else fall through */

    default:
        note->u.note.shape = shape;
        switch (shape) {
        case S_STROPHA:
        case S_DISTROPHA:
        case S_TRISTROPHA:
            switch (note->u.note.liquescentia) {
            case L_AUCTUS_ASCENDENS:
            case L_AUCTUS_DESCENDENS:
                note->u.note.liquescentia = L_AUCTA;
                break;
            case L_AUCTUS_ASCENDENS_INITIO_DEBILIS:
            case L_AUCTUS_DESCENDENS_INITIO_DEBILIS:
                note->u.note.liquescentia = L_AUCTA_INITIO_DEBILIS;
                break;
            default:
                break;
            }
            break;

        case S_ORISCUS:
            switch (note->u.note.liquescentia) {
            case L_AUCTUS_ASCENDENS:
            case L_AUCTUS_DESCENDENS:
            case L_AUCTUS_ASCENDENS_INITIO_DEBILIS:
            case L_AUCTUS_DESCENDENS_INITIO_DEBILIS:
                note->u.note.shape = S_ORISCUS_AUCTUS;
                break;
            case L_DEMINUTUS:
            case L_DEMINUTUS_INITIO_DEBILIS:
                note->u.note.shape = S_ORISCUS_DEMINUTUS;
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }
        break;
    }
}

void gregorio_add_liquescentia(gregorio_note *note, gregorio_liquescentia liq)
{
    if (!note || note->type != GRE_NOTE) {
        gregorio_message(_("trying to make a liquescence on something that "
                    "is not a note"), "add_liquescentia", VERBOSITY_ERROR, 0);
        return;
    }
    if (is_initio_debilis(liq)) {
        switch (liq) {
        case L_DEMINUTUS:
            note->u.note.liquescentia = L_DEMINUTUS_INITIO_DEBILIS;
            break;
        case L_AUCTUS_ASCENDENS:
            note->u.note.liquescentia = L_AUCTUS_ASCENDENS_INITIO_DEBILIS;
            break;
        case L_AUCTUS_DESCENDENS:
            note->u.note.liquescentia = L_AUCTUS_DESCENDENS_INITIO_DEBILIS;
            break;
        case L_AUCTA:
            note->u.note.liquescentia = L_AUCTA_INITIO_DEBILIS;
            break;
        default:
            /* do nothing */
            break;
        }
    } else {
        note->u.note.liquescentia = liq;
    }
    switch (note->u.note.shape) {
    case S_STROPHA:
    case S_DISTROPHA:
    case S_TRISTROPHA:
        switch (note->u.note.liquescentia) {
        case L_AUCTUS_ASCENDENS:
        case L_AUCTUS_DESCENDENS:
            note->u.note.liquescentia = L_AUCTA;
            break;
        case L_AUCTUS_ASCENDENS_INITIO_DEBILIS:
        case L_AUCTUS_DESCENDENS_INITIO_DEBILIS:
            note->u.note.liquescentia = L_AUCTA_INITIO_DEBILIS;
            break;
        default:
            break;
        }
        break;

    case S_ORISCUS:
        switch (note->u.note.liquescentia) {
        case L_AUCTUS_ASCENDENS:
        case L_AUCTUS_DESCENDENS:
        case L_AUCTUS_ASCENDENS_INITIO_DEBILIS:
        case L_AUCTUS_DESCENDENS_INITIO_DEBILIS:
            note->u.note.shape = S_ORISCUS_AUCTUS;
            break;
        case L_DEMINUTUS:
        case L_DEMINUTUS_INITIO_DEBILIS:
            note->u.note.shape = S_ORISCUS_DEMINUTUS;
            break;
        default:
            break;
        }
        break;

    case S_PUNCTUM_CAVUM_INCLINATUM:
        fix_punctum_cavum_inclinatum_liquescentia(note);
        break;

    default:
        break;
    }
}

static void apply_auto_h_episema(gregorio_note *const note,
        const grehepisema_size size, const bool disable_bridge)
{
    if (note->h_episema_above == HEPISEMA_NONE
            && note->h_episema_below == HEPISEMA_NONE) {
        /* if both are unset, set both to auto */
        set_h_episema_above(note, HEPISEMA_AUTO, size, !disable_bridge);
        set_h_episema_below(note, HEPISEMA_AUTO, size, !disable_bridge);
    } else if (note->h_episema_above == HEPISEMA_AUTO
            && note->h_episema_below == HEPISEMA_AUTO) {
        /* if both are auto, then force both */
        /* the upper episema keeps its settings */
        note->h_episema_above = HEPISEMA_FORCED;

        set_h_episema_below(note, HEPISEMA_FORCED, size, !disable_bridge);
    } else {
        /* force whichever is not already forced */
        if (note->h_episema_above != HEPISEMA_FORCED) {
            set_h_episema_above(note, HEPISEMA_FORCED, size, !disable_bridge);
        }
        if (note->h_episema_below != HEPISEMA_FORCED) {
            set_h_episema_below(note, HEPISEMA_FORCED, size, !disable_bridge);
        }
    }
}

/**********************************
 *
 * Activate_isolated_h_episema is used when we see an "isolated"
 * horizontal episema: when we type ab__ lex see a then b then _ then _, so
 * we must put the _ on the a (kind of backward process), and say the
 * the episema on the b is a multi episema. Here n is the length of
 * the isolated episema we found (can be up to 4).
 *
 *********************************/
static void gregorio_activate_isolated_h_episema(gregorio_note *note,
        const grehepisema_size size, const bool disable_bridge, int n)
{
    if (!note) {
        gregorio_message(ngt_("isolated horizontal episema at the beginning "
                    "of a note sequence, ignored",
                    "isolated horizontal episema at the beginning of a note "
                    "sequence, ignored", n), "activate_h_isolated_episema",
                VERBOSITY_WARNING, 0);
        return;
    }
    if (note->type != GRE_NOTE) {
        gregorio_message(ngt_("isolated horizontal episema after something "
                    "that is not a note, ignored",
                    "isolated horizontal episema after something that is not "
                    "a note, ignored", n), "activate_h_isolated_episema",
                VERBOSITY_WARNING, 0);
        return;
    }
    for (; n > 0; --n) {
        note = note->previous;
        if (!note || note->type != GRE_NOTE) {
            gregorio_message(_("found more horizontal episema than notes "
                        "able to be under"), "activate_h_isolated_episema",
                    VERBOSITY_WARNING, 0);
            return;
        }
    }
    apply_auto_h_episema(note, size, disable_bridge);
}

void gregorio_add_h_episema(gregorio_note *note,
        grehepisema_size size, gregorio_vposition vposition,
        bool disable_bridge, unsigned int *nbof_isolated_episema)
{
    if (!note || (note->type != GRE_NOTE && note->type != GRE_BAR)) {
        gregorio_message(_("trying to add a horizontal episema on something "
                    "that is not a note"), "add_h_episema",
                VERBOSITY_ERROR, 0);
        return;
    }
    if (!nbof_isolated_episema) {
        gregorio_message(_("NULL argument nbof_isolated_episema"),
                "add_h_episema", VERBOSITY_FATAL, 0);
        return;
    }
    if (vposition && *nbof_isolated_episema) {
        gregorio_message(_("trying to add a forced horizontal episema on a "
                    "note which already has an automatic horizontal "
                    "episema"), "add_h_episema", VERBOSITY_ERROR, 0);
        return;
    }

    if (vposition || !*nbof_isolated_episema) {
        switch (vposition) {
        case VPOS_ABOVE:
            set_h_episema_above(note, HEPISEMA_FORCED, size, !disable_bridge);
            break;

        case VPOS_BELOW:
            set_h_episema_below(note, HEPISEMA_FORCED, size, !disable_bridge);
            break;

        default: /* VPOS_AUTO */
            apply_auto_h_episema(note, size, disable_bridge);
            *nbof_isolated_episema = 1;
            break;
        }
    } else {
        gregorio_activate_isolated_h_episema(note, size, disable_bridge,
                (*nbof_isolated_episema)++);
    }
}

void gregorio_add_sign(gregorio_note *note, gregorio_sign sign,
        gregorio_vposition vposition)
{
    if (!note) {
        /* error */
        return;
    }
    switch (sign) {
    case _PUNCTUM_MORA:
        switch (note->signs) {
        case _NO_SIGN:
            note->signs = _PUNCTUM_MORA;
            break;
        case _V_EPISEMA:
            note->signs = _V_EPISEMA_PUNCTUM_MORA;
            break;
        case _PUNCTUM_MORA:
            note->signs = _AUCTUM_DUPLEX;
            break;
        case _V_EPISEMA_PUNCTUM_MORA:
            note->signs = _V_EPISEMA_AUCTUM_DUPLEX;
            break;
        default:
            break;
        }

        note->mora_vposition = vposition;
        break;

    case _V_EPISEMA:
        switch (note->signs) {
        case _NO_SIGN:
            note->signs = _V_EPISEMA;
            break;
        case _PUNCTUM_MORA:
            note->signs = _V_EPISEMA_PUNCTUM_MORA;
            break;
        case _AUCTUM_DUPLEX:
            note->signs = _V_EPISEMA_AUCTUM_DUPLEX;
            break;
        default:
            break;
        }

        if (note->type == GRE_NOTE && vposition) {
            note->v_episema_height = note->u.note.pitch + vposition;
        }
        break;

    default:
        break;
    }
}

void gregorio_go_to_first_note(gregorio_note **note)
{
    gregorio_note *tmp;
    if (!*note) {
        return;
    }
    tmp = *note;
    while (tmp->previous) {
        tmp = tmp->previous;
    }
    *note = tmp;
}

static __inline void free_one_note(gregorio_note *note)
{
    free(note->texverb);
    free(note->choral_sign);
    free(note);
}

void gregorio_free_one_note(gregorio_note **note)
{
    gregorio_note *next = NULL;
    if (!note || !*note) {
        return;
    }
    if ((*note)->next) {
        (*note)->next->previous = NULL;
        next = (*note)->next;
    }
    if ((*note)->previous) {
        (*note)->previous->next = NULL;
    }
    free_one_note(*note);
    *note = next;
}

static void gregorio_free_notes(gregorio_note **note)
{
    gregorio_note *tmp;
    while (*note) {
        tmp = (*note)->next;
        free_one_note(*note);
        *note = tmp;
    }
}

static gregorio_glyph *create_and_link_glyph(gregorio_glyph **current_glyph)
{
    gregorio_glyph *glyph = gregorio_calloc(1, sizeof(gregorio_glyph));
    glyph->previous = *current_glyph;
    glyph->next = NULL;
    if (*current_glyph) {
        (*current_glyph)->next = glyph;
    }
    *current_glyph = glyph;

    return glyph;
}

void gregorio_add_glyph(gregorio_glyph **current_glyph,
        gregorio_glyph_type type, gregorio_note *first_note,
        gregorio_liquescentia liquescentia)
{
    gregorio_glyph *next_glyph = create_and_link_glyph(current_glyph);
    if (next_glyph) {
        next_glyph->type = GRE_GLYPH;
        next_glyph->u.notes.glyph_type = type;
        next_glyph->u.notes.liquescentia = liquescentia;
        next_glyph->u.notes.first_note = first_note;
    }
}

void gregorio_add_pitched_element_as_glyph(gregorio_glyph **current_glyph,
        gregorio_type type, signed char pitch, bool flatted_key,
        bool force_pitch, char *texverb)
{
    gregorio_glyph *next_glyph = create_and_link_glyph(current_glyph);
    assert(type == GRE_C_KEY_CHANGE || type == GRE_F_KEY_CHANGE
           || type == GRE_C_KEY_CHANGE_FLATED || type == GRE_F_KEY_CHANGE_FLATED
           || type == GRE_CUSTOS || type == GRE_FLAT || type == GRE_NATURAL
           || type == GRE_SHARP);
    if (next_glyph) {
        next_glyph->type = type;
        next_glyph->u.misc.pitched.pitch = pitch;
        next_glyph->u.misc.pitched.flatted_key = flatted_key;
        next_glyph->u.misc.pitched.force_pitch = force_pitch;
        next_glyph->texverb = texverb;
    }
}

void gregorio_add_unpitched_element_as_glyph(gregorio_glyph **current_glyph,
        gregorio_type type, gregorio_extra_info info, gregorio_sign sign,
        char *texverb)
{
    gregorio_glyph *next_glyph = create_and_link_glyph(current_glyph);
    assert(type != GRE_NOTE && type != GRE_GLYPH && type != GRE_ELEMENT
           && type != GRE_C_KEY_CHANGE && type != GRE_F_KEY_CHANGE
           && type != GRE_C_KEY_CHANGE_FLATED && type != GRE_F_KEY_CHANGE_FLATED
           && type != GRE_CUSTOS && type != GRE_FLAT && type != GRE_NATURAL
           && type != GRE_SHARP);
    if (next_glyph) {
        next_glyph->type = type;
        next_glyph->u.misc.unpitched.info = info;
        next_glyph->u.misc.unpitched.special_sign = sign;
        next_glyph->texverb = texverb;
    }
}

void gregorio_go_to_first_glyph(gregorio_glyph **glyph)
{
    gregorio_glyph *tmp;
    if (!*glyph) {
        return;
    }
    tmp = *glyph;
    while (tmp->previous) {
        tmp = tmp->previous;
    }
    *glyph = tmp;
}

static __inline void free_one_glyph(gregorio_glyph *glyph)
{
    free(glyph->texverb);
    if (glyph->type == GRE_GLYPH) {
        gregorio_free_notes(&glyph->u.notes.first_note);
    }
    free(glyph);
}

void gregorio_free_one_glyph(gregorio_glyph **glyph)
{
    gregorio_glyph *next = NULL;
    if (!glyph || !*glyph) {
        return;
    }
    if ((*glyph)->next) {
        (*glyph)->next->previous = NULL;
        next = (*glyph)->next;
    }
    if ((*glyph)->previous) {
        (*glyph)->previous->next = NULL;
    }
    free_one_glyph(*glyph);
    *glyph = next;
}

static void gregorio_free_glyphs(gregorio_glyph **glyph)
{
    gregorio_glyph *next_glyph;
    if (!glyph || !*glyph) {
        return;
    }
    while (*glyph) {
        next_glyph = (*glyph)->next;
        free_one_glyph(*glyph);
        *glyph = next_glyph;
    }
}

static gregorio_element *create_and_link_element(gregorio_element
                                                 **current_element)
{
    gregorio_element *element = gregorio_calloc(1, sizeof(gregorio_element));
    element->previous = *current_element;
    element->next = NULL;
    if (*current_element) {
        (*current_element)->next = element;
    }
    *current_element = element;

    return element;
}

void gregorio_add_element(gregorio_element **current_element,
        gregorio_glyph *first_glyph)
{
    gregorio_element *next = create_and_link_element(current_element);
    if (next) {
        next->type = GRE_ELEMENT;
        next->u.first_glyph = first_glyph;
    }
}

void gregorio_add_misc_element(gregorio_element **current_element,
        gregorio_type type, gregorio_misc_element_info info, char *texverb)
{
    gregorio_element *special = create_and_link_element(current_element);
    if (special) {
        special->type = type;
        special->u.misc = info;
        special->texverb = texverb;
    }
}

static __inline void free_one_element(gregorio_element *element)
{
    size_t i;
    free(element->texverb);
    for (i = 0; i < element->nabc_lines; i++) {
        free(element->nabc[i]);
    }
    if (element->type == GRE_ELEMENT) {
        gregorio_free_glyphs(&element->u.first_glyph);
    }
    free(element);
}

static void gregorio_free_one_element(gregorio_element **element)
{
    gregorio_element *next = NULL;
    if (!element || !*element) {
        return;
    }
    if ((*element)->next) {
        (*element)->next->previous = NULL;
        next = (*element)->next;
    }
    if ((*element)->previous) {
        (*element)->previous->next = NULL;
    }
    free_one_element(*element);
    *element = next;
}

static void gregorio_free_elements(gregorio_element **element)
{
    gregorio_element *next;
    if (!element || !*element) {
        return;
    }
    while (*element) {
        next = (*element)->next;
        free_one_element(*element);
        *element = next;
    }
}

void gregorio_add_character(gregorio_character **current_character,
        grewchar wcharacter)
{
    gregorio_character *element =
        (gregorio_character *) gregorio_calloc(1, sizeof(gregorio_character));
    element->is_character = 1;
    element->cos.character = wcharacter;
    element->next_character = NULL;
    element->previous_character = *current_character;
    if (*current_character) {
        (*current_character)->next_character = element;
    }
    *current_character = element;
}

static void gregorio_free_one_character(gregorio_character *current_character)
{
    free(current_character);
}

static void gregorio_free_characters(gregorio_character *current_character)
{
    gregorio_character *next_character;
    if (!current_character) {
        return;
    }
    while (current_character) {
        next_character = current_character->next_character;
        gregorio_free_one_character(current_character);
        current_character = next_character;
    }
}

void gregorio_go_to_first_character(gregorio_character **character)
{
    gregorio_character *tmp;
    if (!character || !*character) {
        return;
    }
    tmp = *character;
    while (tmp->previous_character) {
        tmp = tmp->previous_character;
    }
    *character = tmp;
}

void gregorio_begin_style(gregorio_character **current_character,
        grestyle_style style)
{
    gregorio_character *element =
        (gregorio_character *) gregorio_calloc(1, sizeof(gregorio_character));
    element->is_character = 0;
    element->cos.s.type = ST_T_BEGIN;
    element->cos.s.style = style;
    element->previous_character = *current_character;
    element->next_character = NULL;
    if (*current_character) {
        (*current_character)->next_character = element;
    }
    *current_character = element;
}

void gregorio_end_style(gregorio_character **current_character,
        grestyle_style style)
{
    gregorio_character *element =
        (gregorio_character *) gregorio_calloc(1, sizeof(gregorio_character));
    element->is_character = 0;
    element->cos.s.type = ST_T_END;
    element->cos.s.style = style;
    element->next_character = NULL;
    element->previous_character = *current_character;
    if (*current_character) {
        (*current_character)->next_character = element;
    }
    *current_character = element;
}

void gregorio_add_syllable(gregorio_syllable **current_syllable,
        int number_of_voices, gregorio_element *elements[],
        gregorio_character *first_character,
        gregorio_character *first_translation_character,
        gregorio_word_position position, char *abovelinestext,
        gregorio_tr_centering translation_type, gregorio_nlba no_linebreak_area,
        gregorio_euouae euouae, const gregorio_scanner_location *const loc,
        const bool first_word)
{
    gregorio_syllable *next;
    gregorio_element **tab;
    int i;
    if (number_of_voices > MAX_NUMBER_OF_VOICES) {
        gregorio_message(_("too many voices"), "add_syllable", VERBOSITY_FATAL,
                0);
        return;
    }
    next = gregorio_calloc(1, sizeof(gregorio_syllable));
    next->type = GRE_SYLLABLE;
    next->special_sign = _NO_SIGN;
    next->position = position;
    next->no_linebreak_area = no_linebreak_area;
    next->euouae = euouae;
    next->text = first_character;
    next->translation = first_translation_character;
    next->translation_type = translation_type;
    next->abovelinestext = abovelinestext;
    next->first_word = first_word;
    if (loc) {
        next->src_line = loc->first_line;
        next->src_column = loc->first_column;
        next->src_offset = loc->first_offset;
    }
    next->next_syllable = NULL;
    next->previous_syllable = *current_syllable;
    tab = (gregorio_element **) gregorio_malloc(number_of_voices *
            sizeof(gregorio_element *));
    if (elements) {
        for (i = 0; i < number_of_voices; i++) {
            tab[i] = elements[i];
        }
    } else {
        for (i = 0; i < number_of_voices; i++) {
            tab[i] = NULL;
        }
    }
    next->elements = tab;
    if (*current_syllable) {
        (*current_syllable)->next_syllable = next;
    }
    *current_syllable = next;
}

static void gregorio_free_one_syllable(gregorio_syllable **syllable,
        int number_of_voices)
{
    int i;
    gregorio_syllable *next;
    if (!syllable || !*syllable) {
        gregorio_message(_("function called with NULL argument"),
                "free_one_syllable", VERBOSITY_WARNING, 0);
        return;
    }
    for (i = 0; i < number_of_voices; i++) {
        gregorio_free_elements((struct gregorio_element **)
                               &((*syllable)->elements[i]));
    }
    if ((*syllable)->text) {
        gregorio_free_characters((*syllable)->text);
    }
    if ((*syllable)->translation) {
        gregorio_free_characters((*syllable)->translation);
    }
    free((*syllable)->abovelinestext);
    next = (*syllable)->next_syllable;
    free((*syllable)->elements);
    free(*syllable);
    *syllable = next;
}

static void gregorio_free_syllables(gregorio_syllable **syllable,
        int number_of_voices)
{
    if (!syllable || !*syllable) {
        gregorio_message(_("function called with NULL argument"),
                "free_syllables", VERBOSITY_WARNING, 0);
        return;
    }
    while (*syllable) {
        gregorio_free_one_syllable(syllable, number_of_voices);
    }
}

static void gregorio_source_info_init(source_info *si)
{
    si->author = NULL;
    si->date = NULL;
    si->manuscript = NULL;
    si->manuscript_reference = NULL;
    si->manuscript_storage_place = NULL;
    si->transcriber = NULL;
    si->transcription_date = NULL;
    si->book = NULL;
}

gregorio_score *gregorio_new_score(void)
{
    int annotation_num;
    gregorio_score *new_score = gregorio_calloc(1, sizeof(gregorio_score));
    new_score->first_syllable = NULL;
    new_score->number_of_voices = 1;
    new_score->name = NULL;
    new_score->gabc_copyright = NULL;
    new_score->score_copyright = NULL;
    new_score->initial_style = NORMAL_INITIAL;
    new_score->office_part = NULL;
    new_score->occasion = NULL;
    new_score->meter = NULL;
    new_score->commentary = NULL;
    new_score->arranger = NULL;
    gregorio_source_info_init(&new_score->si);
    new_score->first_voice_info = NULL;
    new_score->mode = 0;
    new_score->gregoriotex_font = NULL;
    new_score->user_notes = NULL;
    for (annotation_num = 0; annotation_num < MAX_ANNOTATIONS; ++annotation_num) {
        new_score->annotation[annotation_num] = NULL;
    }
    return new_score;
}

static void gregorio_free_source_info(source_info *si)
{
    free(si->date);
    free(si->author);
    free(si->manuscript);
    free(si->manuscript_reference);
    free(si->manuscript_storage_place);
    free(si->transcriber);
    free(si->transcription_date);
    free(si->book);
}

static void gregorio_free_score_infos(gregorio_score *score)
{
    int annotation_num;
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_free_score_infos", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->name);
    free(score->gabc_copyright);
    free(score->score_copyright);
    free(score->office_part);
    free(score->occasion);
    free(score->meter);
    free(score->commentary);
    free(score->arranger);
    free(score->user_notes);
    free(score->gregoriotex_font);
    for (annotation_num = 0; annotation_num < MAX_ANNOTATIONS; ++annotation_num) {
        free(score->annotation[annotation_num]);
    }
    gregorio_free_source_info(&score->si);
    if (score->first_voice_info) {
        gregorio_free_voice_infos(score->first_voice_info);
    }
}

void gregorio_free_score(gregorio_score *score)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "free_one_syllable", VERBOSITY_WARNING, 0);
        return;
    }
    gregorio_free_syllables(&(score->first_syllable), score->number_of_voices);
    gregorio_free_score_infos(score);
    free(score);
}

void gregorio_set_score_name(gregorio_score *score, char *name)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_name", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->name);
    score->name = name;
}

void gregorio_set_score_gabc_copyright(gregorio_score *score,
        char *gabc_copyright)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_gabc_copyright", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->gabc_copyright);
    score->gabc_copyright = gabc_copyright;
}

void gregorio_set_score_score_copyright(gregorio_score *score,
        char *score_copyright)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_score_copyright", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->score_copyright);
    score->score_copyright = score_copyright;
}

void gregorio_set_score_office_part(gregorio_score *score, char *office_part)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_office_part", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->office_part);
    score->office_part = office_part;
}

void gregorio_set_score_occasion(gregorio_score *score, char *occasion)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_occasion", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->occasion);
    score->occasion = occasion;
}

void gregorio_set_score_meter(gregorio_score *score, char *meter)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_meter", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->meter);
    score->meter = meter;
}

void gregorio_set_score_commentary(gregorio_score *score, char *commentary)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_commentary", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->commentary);
    score->commentary = commentary;
}

void gregorio_set_score_arranger(gregorio_score *score, char *arranger)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_arranger", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->arranger);
    score->arranger = arranger;
}

void gregorio_set_score_number_of_voices(gregorio_score *score,
        int number_of_voices)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_number_of_voices", VERBOSITY_WARNING, 0);
        return;
    }
    score->number_of_voices = number_of_voices;
}

void gregorio_set_score_user_notes(gregorio_score *score, char *user_notes)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_user_notes", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->user_notes);
    score->user_notes = user_notes;
}

void gregorio_add_voice_info(gregorio_voice_info **current_voice_info)
{
    gregorio_voice_info *next = gregorio_calloc(1, sizeof(gregorio_voice_info));
    next->initial_key = NO_KEY;
    next->flatted_key = false;
    next->style = NULL;
    next->virgula_position = NULL;
    next->next_voice_info = NULL;
    if (*current_voice_info) {
        (*current_voice_info)->next_voice_info = next;
    }
    *current_voice_info = next;
}

void gregorio_free_voice_infos(gregorio_voice_info *voice_info)
{
    gregorio_voice_info *next;
    if (!voice_info) {
        gregorio_message(_("function called with NULL argument"),
                "free_voice_info", VERBOSITY_WARNING, 0);
        return;
    }
    while (voice_info) {
        free(voice_info->style);
        free(voice_info->virgula_position);
        next = voice_info->next_voice_info;
        free(voice_info);
        voice_info = next;
    }
}

/*
 * a set of quite useless function 
 */

void gregorio_set_score_annotation(gregorio_score *score, char *annotation)
{
    int annotation_num;
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_annotation", VERBOSITY_WARNING, 0);
        return;
    }
    /* save the annotation in the first spare place. */
    for (annotation_num = 0; annotation_num < MAX_ANNOTATIONS; ++annotation_num) {
        if (score->annotation[annotation_num] == NULL) {
            score->annotation[annotation_num] = annotation;
            break;
        }
    }
    if (annotation_num >= MAX_ANNOTATIONS) {
        free(annotation);
        gregorio_message(_("too many annotations"),
                "gregorio_set_annotation", VERBOSITY_WARNING, 0);
    }
}

void gregorio_set_score_author(gregorio_score *score, char *author)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_author", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->si.author);
    score->si.author = author;
}

void gregorio_set_score_date(gregorio_score *score, char *date)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_date", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->si.date);
    score->si.date = date;
}

void gregorio_set_score_manuscript(gregorio_score *score, char *manuscript)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_manuscript", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->si.manuscript);
    score->si.manuscript = manuscript;
}

void gregorio_set_score_manuscript_reference(gregorio_score *score,
        char *manuscript_reference)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_reference", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->si.manuscript_reference);
    score->si.manuscript_reference = manuscript_reference;
}

void gregorio_set_score_manuscript_storage_place(gregorio_score *score,
        char *manuscript_storage_place)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_manuscript_storage_place",
                VERBOSITY_WARNING, 0);
        return;
    }
    free(score->si.manuscript_storage_place);
    score->si.manuscript_storage_place = manuscript_storage_place;
}

void gregorio_set_score_book(gregorio_score *score, char *book)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_book", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->si.book);
    score->si.book = book;
}

void gregorio_set_score_transcriber(gregorio_score *score, char *transcriber)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_transcriber", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->si.transcriber);
    score->si.transcriber = transcriber;
}

void gregorio_set_score_transcription_date(gregorio_score *score,
        char *transcription_date)
{
    if (!score) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_score_transcription_date", VERBOSITY_WARNING, 0);
        return;
    }
    free(score->si.transcription_date);
    score->si.transcription_date = transcription_date;
}

void gregorio_set_voice_style(gregorio_voice_info *voice_info, char *style)
{
    if (!voice_info) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_voice_style", VERBOSITY_WARNING, 0);
        return;
    }
    free(voice_info->style);
    voice_info->style = style;
}

void gregorio_set_voice_virgula_position(gregorio_voice_info *voice_info,
        char *virgula_position)
{
    if (!voice_info) {
        gregorio_message(_("function called with NULL argument"),
                "gregorio_set_voice_virgula_position", VERBOSITY_WARNING, 0);
        return;
    }
    free(voice_info->virgula_position);
    voice_info->virgula_position = virgula_position;
}

/**********************************
 *
 * A function to build an integer from a key, very useful to represent
 * it in the structure.
 *
 *The representation is :
 *
 * * 1 for a C key on the first (bottom) line
 * * 3 for a C key on the second line
 * * 5 for a C key on the third line (default key)
 * * 7 for a C key on the fourth line
 *
 * * -2 for a F key on the first line
 * * 0 for a F key on the second line
 * * 2 for a F key on the third line
 * * 4 for a F key on the fourth line
 *
 *********************************/

int gregorio_calculate_new_key(char step, int line)
{
    switch (step) {
    case C_KEY:
        return (2 * line) - 1;
        break;
    case F_KEY:
        return (2 * line) - 4;
        break;
    default:
        gregorio_message(_("can't calculate key"),
                "gregorio_calculate_new_key", VERBOSITY_ERROR, 0);
        return NO_KEY;
    }
}

/**********************************
 *
 * The reverse function of the preceeding : give step (c or f) and
 * line (1-4) from an integer representing the key.
 *
 *********************************/

void gregorio_det_step_and_line_from_key(int key, char *step, int *line)
{
    switch (key) {
    case -2:
        *step = 'f';
        *line = 1;
        break;
    case 0:
        *step = 'f';
        *line = 2;
        break;
    case 2:
        *step = 'f';
        *line = 3;
        break;
    case 4:
        *step = 'f';
        *line = 4;
        break;
    case 1:
        *step = 'c';
        *line = 1;
        break;
    case 3:
        *step = 'c';
        *line = 2;
        break;
    case 5:
        *step = 'c';
        *line = 3;
        break;
    case 7:
        *step = 'c';
        *line = 4;
        break;
    default:
        *step = '?';
        *line = 0;
        gregorio_message(_("can't determine step and line of the key"),
                "gregorio_det_step_and_line_from_key", VERBOSITY_ERROR, 0);
        return;
    }
}

static signed char gregorio_syllable_first_note(gregorio_syllable *syllable)
{
    gregorio_element *element;
    gregorio_glyph *glyph;
    if (!syllable) {
        gregorio_message(_("called with a NULL argument"),
                "gregorio_syllable_first_note", VERBOSITY_ERROR, 0);
    }
    element = syllable->elements[0];
    while (element) {
        if (element->type == GRE_CUSTOS) {
            return element->u.misc.pitched.pitch;
        }
        if (element->type == GRE_ELEMENT && element->u.first_glyph) {
            glyph = element->u.first_glyph;
            while (glyph) {
                if (glyph->type == GRE_GLYPH && glyph->u.notes.first_note) {
                    assert(glyph->u.notes.first_note->type == GRE_NOTE);
                    return glyph->u.notes.first_note->u.note.pitch;
                }
                glyph = glyph->next;
            }
        }
        element = element->next;
    }
    return 0;
}

signed char gregorio_determine_next_pitch(gregorio_syllable *syllable,
        gregorio_element *element, gregorio_glyph *glyph)
{
    signed char temp;
    if (!element || !syllable) {
        gregorio_message(_("called with a NULL argument"),
                "gregorio_determine_next_pitch", VERBOSITY_ERROR, 0);
        return DUMMY_PITCH;
    }
    /* we first explore the next glyphs to find a note, if there is one */
    if (glyph) {
        glyph = glyph->next;
        while (glyph) {
            if (glyph->type == GRE_GLYPH && glyph->u.notes.first_note) {
                assert(glyph->u.notes.first_note->type == GRE_NOTE);
                return glyph->u.notes.first_note->u.note.pitch;
            }
            glyph = glyph->next;
        }
    }
    /* then we do the same with the elements */
    element = element->next;
    while (element) {
        if (element->type == GRE_CUSTOS) {
            return element->u.misc.pitched.pitch;
        }
        if (element->type == GRE_ELEMENT && element->u.first_glyph) {
            glyph = element->u.first_glyph;
            while (glyph) {
                if (glyph->type == GRE_GLYPH && glyph->u.notes.first_note) {
                    assert(glyph->u.notes.first_note->type == GRE_NOTE);
                    return glyph->u.notes.first_note->u.note.pitch;
                }
                glyph = glyph->next;
            }
        }
        element = element->next;
    }

    /* then we do the same with the syllables */
    syllable = syllable->next_syllable;
    while (syllable) {
        /* we call another function that will return the pitch of the first
         * note if syllable has a note, and 0 else */
        temp = gregorio_syllable_first_note(syllable);
        if (temp) {
            return temp;
        }
        syllable = syllable->next_syllable;
    }
    /* here it means that there is no next note, so we return a stupid value,
     * but it won' t be used */
    return DUMMY_PITCH;
}

/**********************************
 *
 * A function that may be useful (used in xml-write) : we have a
 * tabular of alterations (we must remember all alterations on all
 * notes all the time, they are reinitialized when a bar is found),
 * and we assign all of them to NO_ALTERATION.
 *
 *This function works in fact with a tabular of tabular, one per
 *voice, for polyphony.
 *
 *********************************/

void gregorio_reinitialize_alterations(char alterations[][13],
        int number_of_voices)
{
    int i;
    int j;
    for (j = 0; j < number_of_voices; j++) {
        for (i = 0; i < 13; i++) {
            alterations[j][i] = NO_ALTERATION;
        }
    }
}

/**********************************
 *
 * The corresponding function for monophony.
 *
 *********************************/

void gregorio_reinitialize_one_voice_alterations(char alterations[13])
{
    int i;
    for (i = 0; i < 13; i++) {
        alterations[i] = NO_ALTERATION;
    }
}

/**********************************
 *
 * A function called after the entire score is determined : we check
 * if the first element is a key change, if it is the case we delete
 * it and we update the score->voice-info->initial_key. Works in
 * polyphony.
 *
 *********************************/

void gregorio_fix_initial_keys(gregorio_score *score, int default_key)
{
    int clef = 0;
    gregorio_element *element;
    gregorio_voice_info *voice_info;
    int i;
    char to_delete = 1;

    if (!score || !score->first_syllable || !score->first_voice_info) {
        gregorio_message(_("score is not available"),
                "gregorio_fix_initial_keys", VERBOSITY_WARNING, 0);
        return;
    }
    voice_info = score->first_voice_info;
    for (i = 0; i < score->number_of_voices; i++) {
        element = score->first_syllable->elements[i];
        if (!element) {
            continue;
        }
        if (element->type == GRE_C_KEY_CHANGE) {
            clef =
                gregorio_calculate_new_key(C_KEY,
                                           element->u.misc.pitched.pitch - '0');
            voice_info->initial_key = clef;
            voice_info->flatted_key = element->u.misc.pitched.flatted_key;
            gregorio_free_one_element(&(score->first_syllable->elements[i]));
            gregorio_messagef("gregorio_fix_initial_keys", VERBOSITY_INFO, 0,
                    _("in voice %d the first element is a key definition, "
                    "considered as initial key"), i + 1);
        } else if (element->type == GRE_F_KEY_CHANGE) {
            clef =
                gregorio_calculate_new_key(F_KEY,
                                           element->u.misc.pitched.pitch - '0');
            voice_info->initial_key = clef;
            voice_info->flatted_key = element->u.misc.pitched.flatted_key;
            gregorio_free_one_element(&(score->first_syllable->elements[i]));
            gregorio_messagef("gregorio_fix_initial_keys", VERBOSITY_INFO, 0,
                    _("in voice %d the first element is a key definition, "
                    "considered as initial key"), i + 1);
        }
        voice_info = voice_info->next_voice_info;
    }

    /* then we suppress syllables that contain nothing anymore : case of (c2)
     * at beginning of files */

    for (i = 0; i < score->number_of_voices; i++) {
        if (score->first_syllable->elements[i]) {
            to_delete = 0;
            break;
        }
    }

    if (to_delete) {
        gregorio_free_one_syllable(&(score->first_syllable),
                                   score->number_of_voices);
    }
    /* finally we initialize voice infos that have no initial key to default
     * key */

    voice_info = score->first_voice_info;

    for (i = 0; i < score->number_of_voices; i++) {
        if (voice_info->initial_key == NO_KEY) {
            voice_info->initial_key = default_key;
            gregorio_messagef("gregorio_fix_initial_keys", VERBOSITY_INFO, 0,
                    _("no initial key definition in voice %d, default key "
                    "definition applied"), i + 1);
        }
        voice_info = voice_info->next_voice_info;
    }
}

/**********************************
 *
 * A small function to determine if an element list contains only
 * special elements (bar, key-change, etc.), useful because the
 * representation (in xml for example) may vary according to it.
 *
 *********************************/

bool gregorio_is_only_special(gregorio_element *element)
{
    if (!element) {
        return 0;
    }
    while (element) {
        if (element->type == GRE_ELEMENT) {
            return 0;
        }
        element = element->next;
    }
    return 1;
}
