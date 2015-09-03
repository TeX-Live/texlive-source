/*
 * Copyright (C) 2007-2015 The Gregorio Project (see CONTRIBUTORS.md)
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
#include <stdlib.h>
#include "bool.h"
#include "struct.h"
#include "unicode.h"
#include "messages.h"
#include "plugins.h"
#include "support.h"

static const char *unknown(int value) {
    static char buf[20];
    gregorio_snprintf(buf, 20, "?%d", value);
    return buf;
}

static const char *dump_translation_type_to_string(gregorio_tr_centering
                                            translation_type)
{
    switch (translation_type) {
    case TR_NORMAL:
        return "TR_NORMAL";
    case TR_WITH_CENTER_BEGINNING:
        return "TR_WITH_CENTER_BEGINNING";
    case TR_WITH_CENTER_END:
        return "TR_WITH_CENTER_END";
    }
    return unknown(translation_type);
}

static const char *dump_nlba_to_string(gregorio_nlba no_linebreak_area)
{
    switch (no_linebreak_area) {
    case NLBA_NORMAL:
        return "NLBA_NORMAL";
    case NLBA_BEGINNING:
        return "NLBA_BEGINNING";
    case NLBA_END:
        return "NLBA_END";
    }
    return unknown(no_linebreak_area);
}

static const char *dump_style_to_string(grestyle_style style)
{
    switch (style) {
    case ST_NO_STYLE:
        return "     ST_NO_STYLE";
    case ST_ITALIC:
        return "       ST_ITALIC";
    case ST_CENTER:
        return "       ST_CENTER";
    case ST_FORCED_CENTER:
        return " ST_FORCED_CENTER";
    case ST_INITIAL:
        return "      ST_INITIAL";
    case ST_BOLD:
        return "         ST_BOLD";
    case ST_TT:
        return "           ST_TT";
    case ST_UNDERLINED:
        return "   ST_UNDERLINED";
    case ST_COLORED:
        return "      ST_COLORED";
    case ST_SMALL_CAPS:
        return "   ST_SMALL_CAPS";
    case ST_SPECIAL_CHAR:
        return " ST_SPECIAL_CHAR";
    case ST_VERBATIM:
        return "     ST_VERBATIM";
    case ST_FIRST_SYLLABLE:
        return "ST_FIRST_SYLLABLE";
    case ST_FIRST_SYLLABLE_INITIAL:
        return "ST_FIRST_SYLLABLE_INITIAL";
    }
    return unknown(style);
}

static void dump_write_characters(FILE *const f,
        const gregorio_character * current_character)
{
    while (current_character) {
        fprintf(f,
                "---------------------------------------------------------------------\n");
        if (current_character->is_character) {
            fprintf(f, "     character                 ");
            gregorio_print_unichar(f, current_character->cos.character);
            fprintf(f, "\n");
        } else {
            if (current_character->cos.s.type == ST_T_BEGIN) {
                fprintf(f, "     beginning of style   %s\n",
                        dump_style_to_string(current_character->cos.s.style));
            } else {
                fprintf(f, "     end of style         %s\n",
                        dump_style_to_string(current_character->cos.s.style));
            }
        }
        current_character = current_character->next_character;
    }
}

static const char *dump_key_to_char(const int key)
{
    switch (key) {
    case -2:
        return "f1";
    case 0:
        return "f2";
    case 2:
        return "f3";
    case 4:
        return "f4";
    case 1:
        return "c1";
    case 3:
        return "c2";
    case 5:
        return "c3";
    case 7:
        return "c4";
    }
    return "no key defined";
}

static const char *dump_syllable_position(gregorio_word_position pos)
{
    switch (pos) {
    case WORD_BEGINNING:
        return "WORD_BEGINNING";
    case WORD_MIDDLE:
        return "WORD_MIDDLE";
    case WORD_END:
        return "WORD_END";
    case WORD_ONE_SYLLABLE:
        return "WORD_ONE_SYLLABLE";
    }
    return "unknown";
}

static const char *dump_type(gregorio_type type)
{
    switch (type) {
    case GRE_NOTE:
        return "GRE_NOTE";
    case GRE_GLYPH:
        return "GRE_GLYPH";
    case GRE_ELEMENT:
        return "GRE_ELEMENT";
    case GRE_FLAT:
        return "GRE_FLAT";
    case GRE_SHARP:
        return "GRE_SHARP";
    case GRE_NATURAL:
        return "GRE_NATURAL";
    case GRE_C_KEY_CHANGE:
        return "GRE_C_KEY_CHANGE";
    case GRE_F_KEY_CHANGE:
        return "GRE_F_KEY_CHANGE";
    case GRE_END_OF_LINE:
        return "GRE_END_OF_LINE";
    case GRE_END_OF_PAR:
        return "GRE_END_OF_PAR";
    case GRE_CUSTOS:
        return "GRE_CUSTOS";
    case GRE_SPACE:
        return "GRE_SPACE";
    case GRE_BAR:
        return "GRE_BAR";
    case GRE_SYLLABLE:
        return "GRE_SYLLABLE";
    case GRE_TEXVERB_GLYPH:
        return "GRE_TEXVERB_GLYPH";
    case GRE_TEXVERB_ELEMENT:
        return "GRE_TEXVERB_ELEMENT";
    case GRE_NLBA:
        return "GRE_NLBA";
    case GRE_ALT:
        return "GRE_ALT";
    case GRE_MANUAL_CUSTOS:
        return "GRE_MANUAL_CUSTOS";
    default:
        return "unknown";
    }
}

static const char *dump_bar_type(gregorio_bar element_type)
{
    switch (element_type) {
    case B_NO_BAR:
        return "B_NO_BAR";
    case B_VIRGULA:
        return "B_VIRGULA";
    case B_DIVISIO_MINIMA:
        return "B_DIVISIO_MINIMA";
    case B_DIVISIO_MINOR:
        return "B_DIVISIO_MINOR";
    case B_DIVISIO_MAIOR:
        return "B_DIVISIO_MAIOR";
    case B_DIVISIO_FINALIS:
        return "B_DIVISIO_FINALIS";
    case B_DIVISIO_MINOR_D1:
        return "B_DIVISIO_MINOR_D1";
    case B_DIVISIO_MINOR_D2:
        return "B_DIVISIO_MINOR_D2";
    case B_DIVISIO_MINOR_D3:
        return "B_DIVISIO_MINOR_D3";
    case B_DIVISIO_MINOR_D4:
        return "B_DIVISIO_MINOR_D4";
    case B_DIVISIO_MINOR_D5:
        return "B_DIVISIO_MINOR_D5";
    case B_DIVISIO_MINOR_D6:
        return "B_DIVISIO_MINOR_D6";
    }
    return "unknown";
}

static const char *dump_space_type(gregorio_space element_type)
{
    switch (element_type) {
    case SP_DEFAULT:
        return "SP_DEFAULT";
    case SP_NO_SPACE:
        return "SP_NO_SPACE";
    case SP_ZERO_WIDTH:
        return "SP_ZERO_WIDTH";
    case SP_NEUMATIC_CUT:
        return "SP_NEUMATIC_CUT";
    case SP_LARGER_SPACE:
        return "SP_LARGER_SPACE";
    case SP_GLYPH_SPACE:
        return "SP_GLYPH_SPACE";
    case SP_GLYPH_SPACE_NB:
        return "SP_GLYPH_SPACE_NB";
    case SP_LARGER_SPACE_NB:
        return "SP_LARGER_SPACE_NB";
    case SP_NEUMATIC_CUT_NB:
        return "SP_NEUMATIC_CUT_NB";
    }
    return "unknown";
}

static const char *dump_liquescentia(gregorio_liquescentia liquescentia)
{
    switch (liquescentia) {
    case L_NO_LIQUESCENTIA:
        return "L_NO_LIQUESCENTIA";
    case L_DEMINUTUS:
        return "L_DEMINUTUS";
    case L_AUCTUS_ASCENDENS:
        return "L_AUCTUS_ASCENDENS";
    case L_AUCTUS_DESCENDENS:
        return "L_AUCTUS_DESCENDENS";
    case L_AUCTA:
        return "L_AUCTA";
    case L_INITIO_DEBILIS:
        return "L_INITIO_DEBILIS";
    case L_DEMINUTUS_INITIO_DEBILIS:
        return "L_DEMINUTUS_INITIO_DEBILIS";
    case L_AUCTUS_ASCENDENS_INITIO_DEBILIS:
        return "L_AUCTUS_ASCENDENS_INITIO_DEBILIS";
    case L_AUCTUS_DESCENDENS_INITIO_DEBILIS:
        return "L_AUCTUS_DESCENDENS_INITIO_DEBILIS";
    case L_AUCTA_INITIO_DEBILIS:
        return "L_AUCTA_INITIO_DEBILIS";
    }
    return "unknown";
}

static const char *dump_glyph_type(gregorio_glyph_type glyph_type)
{
    switch (glyph_type) {
    case G_PUNCTUM_INCLINATUM:
        return "G_PUNCTUM_INCLINATUM";
    case G_2_PUNCTA_INCLINATA_DESCENDENS:
        return "G_2_PUNCTA_INCLINATA_DESCENDENS";
    case G_3_PUNCTA_INCLINATA_DESCENDENS:
        return "G_3_PUNCTA_INCLINATA_DESCENDENS";
    case G_4_PUNCTA_INCLINATA_DESCENDENS:
        return "G_4_PUNCTA_INCLINATA_DESCENDENS";
    case G_5_PUNCTA_INCLINATA_DESCENDENS:
        return "G_5_PUNCTA_INCLINATA_DESCENDENS";
    case G_2_PUNCTA_INCLINATA_ASCENDENS:
        return "G_2_PUNCTA_INCLINATA_ASCENDENS";
    case G_3_PUNCTA_INCLINATA_ASCENDENS:
        return "G_3_PUNCTA_INCLINATA_ASCENDENS";
    case G_4_PUNCTA_INCLINATA_ASCENDENS:
        return "G_4_PUNCTA_INCLINATA_ASCENDENS";
    case G_5_PUNCTA_INCLINATA_ASCENDENS:
        return "G_5_PUNCTA_INCLINATA_ASCENDENS";
    case G_TRIGONUS:
        return "G_TRIGONUS";
    case G_PUNCTA_INCLINATA:
        return "G_PUNCTA_INCLINATA";
    case G_UNDETERMINED:
        return "G_UNDETERMINED";
    case G_VIRGA:
        return "G_VIRGA";
    case G_VIRGA_REVERSA:
        return "G_VIRGA_REVERSA";
    case G_STROPHA:
        return "G_STROPHA";
    case G_STROPHA_AUCTA:
        return "G_STROPHA_AUCTA";
    case G_PUNCTUM:
        return "G_PUNCTUM";
    case G_PODATUS:
        return "G_PODATUS";
    case G_PES_QUADRATUM:
        return "G_PES_QUADRATUM";
    case G_FLEXA:
        return "G_FLEXA";
    case G_TORCULUS:
        return "G_TORCULUS";
    case G_TORCULUS_RESUPINUS:
        return "G_TORCULUS_RESUPINUS";
    case G_TORCULUS_RESUPINUS_FLEXUS:
        return "G_TORCULUS_RESUPINUS_FLEXUS";
    case G_PORRECTUS:
        return "G_PORRECTUS";
    case G_PORRECTUS_FLEXUS:
        return "G_PORRECTUS_FLEXUS";
    case G_BIVIRGA:
        return "G_BIVIRGA";
    case G_TRIVIRGA:
        return "G_TRIVIRGA";
    case G_DISTROPHA:
        return "G_DISTROPHA";
    case G_DISTROPHA_AUCTA:
        return "G_DISTROPHA_AUCTA";
    case G_TRISTROPHA:
        return "G_TRISTROPHA";
    case G_ANCUS:
        return "G_ANCUS";
    case G_TRISTROPHA_AUCTA:
        return "G_TRISTROPHA_AUCTA";
    case G_PES_QUADRATUM_FIRST_PART:
        return "G_PES_QUADRATUM_FIRST_PART";
    case G_SCANDICUS:
        return "G_SCANDICUS";
    case G_SALICUS:
        return "G_SALICUS";
    case G_VIRGA_STRATA:
        return "G_VIRGA_STRATA";
    case G_TORCULUS_LIQUESCENS:
        return "G_TORCULUS_LIQUESCENS";
    default:
        return "unknown";
    }
}

static const char *dump_shape(gregorio_shape shape)
{
    switch (shape) {
    case S_UNDETERMINED:
        return "S_UNDETERMINED";
    case S_PUNCTUM:
        return "S_PUNCTUM";
    case S_PUNCTUM_END_OF_GLYPH:
        return "S_PUNCTUM_END_OF_GLYPH";
    case S_PUNCTUM_INCLINATUM:
        return "S_PUNCTUM_INCLINATUM";
    case S_PUNCTUM_INCLINATUM_DEMINUTUS:
        return "S_PUNCTUM_INCLINATUM_DEMINUTUS";
    case S_PUNCTUM_INCLINATUM_AUCTUS:
        return "S_PUNCTUM_INCLINATUM_AUCTUS";
    case S_VIRGA:
        return "S_VIRGA";
    case S_VIRGA_REVERSA:
        return "S_VIRGA_REVERSA";
    case S_BIVIRGA:
        return "S_BIVIRGA";
    case S_TRIVIRGA:
        return "S_TRIVIRGA";
    case S_ORISCUS:
        return "S_ORISCUS";
    case S_ORISCUS_AUCTUS:
        return "S_ORISCUS_AUCTUS";
    case S_ORISCUS_DEMINUTUS:
        return "S_ORISCUS_DEMINUTUS";
    case S_ORISCUS_SCAPUS:
        return "S_ORISCUS_SCAPUS";
    case S_QUILISMA:
        return "S_QUILISMA";
    case S_STROPHA:
        return "S_STROPHA";
    case S_STROPHA_AUCTA:
        return "S_STROPHA_AUCTA";
    case S_DISTROPHA:
        return "S_DISTROPHA";
    case S_DISTROPHA_AUCTA:
        return "S_DISTROPHA_AUCTA";
    case S_TRISTROPHA:
        return "S_TRISTROPHA";
    case S_TRISTROPHA_AUCTA:
        return "S_TRISTROPHA_AUCTA";
    case S_QUADRATUM:
        return "S_QUADRATUM";
    case S_PUNCTUM_CAVUM:
        return "S_PUNCTUM_CAVUM";
    case S_LINEA_PUNCTUM:
        return "S_LINEA_PUNCTUM";
    case S_LINEA_PUNCTUM_CAVUM:
        return "S_LINEA_PUNCTUM_CAVUM";
    case S_LINEA:
        return "S_LINEA";
    case S_PUNCTUM_CAVUM_INCLINATUM:
        return "S_PUNCTUM_CAVUM_INCLINATUM";
    case S_PUNCTUM_CAVUM_INCLINATUM_AUCTUS:
        return "S_PUNCTUM_CAVUM_INCLINATUM_AUCTUS";
    default:
        return "unknown";
    }
}

static const char *dump_signs(gregorio_sign signs)
{
    switch (signs) {
    case _NO_SIGN:
        return "_NO_SIGN";
    case _PUNCTUM_MORA:
        return "_PUNCTUM_MORA";
    case _AUCTUM_DUPLEX:
        return "_AUCTUM_DUPLEX";
    case _V_EPISEMUS:
        return "_V_EPISEMUS";
    case _V_EPISEMUS_PUNCTUM_MORA:
        return "_V_EPISEMUS_PUNCTUM_MORA";
    case _V_EPISEMUS_AUCTUM_DUPLEX:
        return "_V_EPISEMUS_AUCTUM_DUPLEX";
    default:
        return "unknown";
    }
}

/* a function dumping special signs */
static const char *dump_special_sign(gregorio_sign special_sign)
{
    switch (special_sign) {
    case _ACCENTUS:
        return "_ACCENTUS";
    case _ACCENTUS_REVERSUS:
        return "_ACCENTUS_REVERSUS";
    case _CIRCULUS:
        return "_CIRCULUS";
    case _SEMI_CIRCULUS:
        return "_SEMI_CIRCULUS";
    case _SEMI_CIRCULUS_REVERSUS:
        return "_SEMI_CIRCULUS_REVERSUS";
    case _V_EPISEMUS:
        return "_V_EPISEMUS";
    case _V_EPISEMUS_BAR_H_EPISEMUS:
        return "_V_EPISEMUS_BAR_H_EPISEMUS";
    case _BAR_H_EPISEMUS:
        return "_BAR_H_EPISEMUS";
    default:
        return "unknown";
    }
}

static const char *dump_h_episemus_size(grehepisemus_size size)
{
    switch (size) {
    case H_NORMAL:
        return "H_NORMAL";
    case H_SMALL_LEFT:
        return "H_SMALL_LEFT";
    case H_SMALL_CENTRE:
        return "H_SMALL_CENTRE";
    case H_SMALL_RIGHT:
        return "H_SMALL_RIGHT";
    }
    return "unknown";
}

static const char *dump_bool(bool value) {
    return value? "true" : "false";
}

static const char *dump_vposition(gregorio_vposition vpos) {
    switch (vpos) {
    case VPOS_AUTO:
        return "VPOS_AUTO";
    case VPOS_ABOVE:
        return "VPOS_ABOVE";
    case VPOS_BELOW:
        return "VPOS_BELOW";
    }
    return "unknown";
}

static const char *dump_pitch(const char height) {
    static char buf[20];
    if (height >= LOWEST_PITCH && height <= HIGHEST_PITCH) {
        gregorio_snprintf(buf, 20, "%c", height + 'a' - LOWEST_PITCH);
    } else {
        gregorio_snprintf(buf, 20, "?%d", height);
    }
    return buf;
}

void dump_write_score(FILE *f, gregorio_score *score)
{
    gregorio_voice_info *voice_info = score->first_voice_info;
    int i;
    int annotation_num;
    gregorio_syllable *syllable;

    if (!f) {
        gregorio_message(_("call with NULL file"), "gregoriotex_write_score",
                VERBOSITY_ERROR, 0);
        return;
    }
    fprintf(f,
            "=====================================================================\n"
            " SCORE INFOS\n"
            "=====================================================================\n");
    if (score->number_of_voices) {
        fprintf(f, "   number_of_voices          %d\n",
                score->number_of_voices);
    }
    if (score->name) {
        fprintf(f, "   name                      %s\n", score->name);
    }
    if (score->gabc_copyright) {
        fprintf(f, "   gabc_copyright            %s\n", score->gabc_copyright);
    }
    if (score->score_copyright) {
        fprintf(f, "   score_copyright           %s\n", score->score_copyright);
    }
    if (score->office_part) {
        fprintf(f, "   office_part               %s\n", score->office_part);
    }
    if (score->occasion) {
        fprintf(f, "   occasion                  %s\n", score->occasion);
    }
    if (score->meter) {
        fprintf(f, "   meter                     %s\n", score->meter);
    }
    if (score->commentary) {
        fprintf(f, "   commentary                %s\n", score->commentary);
    }
    if (score->arranger) {
        fprintf(f, "   arranger                  %s\n", score->arranger);
    }
    if (score->si.author) {
        fprintf(f, "   author                    %s\n", score->si.author);
    }
    if (score->si.date) {
        fprintf(f, "   date                      %s\n", score->si.date);
    }
    if (score->si.manuscript) {
        fprintf(f, "   manuscript                %s\n", score->si.manuscript);
    }
    if (score->si.manuscript_reference) {
        fprintf(f, "   manuscript_reference      %s\n",
                score->si.manuscript_reference);
    }
    if (score->si.manuscript_storage_place) {
        fprintf(f, "   manuscript_storage_place  %s\n",
                score->si.manuscript_storage_place);
    }
    if (score->si.book) {
        fprintf(f, "   book                      %s\n", score->si.book);
    }
    if (score->si.transcriber) {
        fprintf(f, "   transcriber               %s\n", score->si.transcriber);
    }
    if (score->si.transcription_date) {
        fprintf(f, "   transcription_date        %s\n",
                score->si.transcription_date);
    }
    if (score->gregoriotex_font) {
        fprintf(f, "   gregoriotex_font          %s\n",
                score->gregoriotex_font);
    }
    if (score->mode) {
        fprintf(f, "   mode                      %d\n", score->mode);
    }
    if (score->initial_style) {
        fprintf(f, "   initial_style             %d\n", score->initial_style);
    }
    if (score->nabc_lines) {
        fprintf (f, "   nabc_lines                %d\n", (int)score->nabc_lines);
    }
    if (score->user_notes) {
        fprintf(f, "   user_notes                %s\n", score->user_notes);
    }
    fprintf(f, "\n\n"
            "=====================================================================\n"
            " VOICES INFOS\n"
            "=====================================================================\n");
    for (i = 0; i < score->number_of_voices; i++) {
        fprintf(f, "  Voice %d\n", i + 1);
        if (voice_info->initial_key) {
            fprintf(f, "   initial_key               %d (%s)\n",
                    voice_info->initial_key,
                    dump_key_to_char(voice_info->initial_key));
            if (voice_info->flatted_key) {
                fprintf(f, "   flatted_key               true\n");
            }
        }
        for (annotation_num = 0; annotation_num < MAX_ANNOTATIONS;
             ++annotation_num) {
            if (score->annotation[annotation_num]) {
                fprintf(f, "   annotation                %s\n",
                        score->annotation[annotation_num]);
            }
        }
        if (voice_info->style) {
            fprintf(f, "   style                     %s\n", voice_info->style);
        }
        if (voice_info->virgula_position) {
            fprintf(f, "   virgula_position          %s\n",
                    voice_info->virgula_position);
        }
        voice_info = voice_info->next_voice_info;
    }
    fprintf(f, "\n\n"
            "=====================================================================\n"
            " SCORE\n"
            "=====================================================================\n");
    for (syllable = score->first_syllable; syllable;
            syllable = syllable->next_syllable) {
        gregorio_element *element;
        if (syllable->type) {
            fprintf(f, "   type                      %d (%s)\n",
                    syllable->type, dump_type(syllable->type));
        }
        if (syllable->position) {
            fprintf(f, "   position                  %d (%s)\n",
                    syllable->position,
                    dump_syllable_position(syllable->position));
        }
        if (syllable->special_sign) {
            fprintf(f, "   special sign                       %s\n",
                    dump_special_sign(syllable->special_sign));
        }
        if (syllable->no_linebreak_area != NLBA_NORMAL) {
            fprintf(f, "   no line break area        %s\n",
                    dump_nlba_to_string(syllable->no_linebreak_area));
        }
        if (syllable->text) {
            if (syllable->translation) {
                fprintf(f, "\n  Text\n");
            }
            dump_write_characters(f, syllable->text);
        }
        if ((syllable->translation
             && syllable->translation_type != TR_WITH_CENTER_END)
            || syllable->translation_type == TR_WITH_CENTER_END) {
            fprintf(f, "\n  Translation type             %s",
                    dump_translation_type_to_string
                    (syllable->translation_type));
            if (syllable->translation_type == TR_WITH_CENTER_END) {
                fprintf(f, "\n");
            }
        }
        if (syllable->translation) {
            fprintf(f, "\n  Translation\n");
            dump_write_characters(f, syllable->translation);
        }
        if (syllable->abovelinestext) {
            fprintf(f, "\n  Abovelinestext\n    %s", syllable->abovelinestext);
        }
        for (element = *syllable->elements; element; element = element->next) {
            gregorio_glyph *glyph;
            fprintf(f, "---------------------------------------------------------------------\n");
            if (element->type) {
                fprintf(f, "     type                    %d (%s)\n",
                        element->type, dump_type(element->type));
            }
            switch (element->type) {
            case GRE_CUSTOS:
                if (element->u.misc.pitched.pitch) {
                    fprintf(f, "     pitch                   %s\n",
                            dump_pitch(element->u.misc.pitched.pitch));
                }
                if (element->u.misc.pitched.force_pitch) {
                    fprintf(f, "     force_pitch             true\n");
                }
                break;
            case GRE_SPACE:
                if (element->u.misc.unpitched.info.space) {
                    fprintf(f, "     space                   %d (%s)\n",
                            element->u.misc.unpitched.info.space,
                            dump_space_type(element->u.misc.unpitched.info.
                                space));
                }
                break;
            case GRE_TEXVERB_ELEMENT:
                fprintf(f, "     TeX string              \"%s\"\n",
                        element->texverb);
                break;
            case GRE_NLBA:
                fprintf(f, "     nlba                    %d (%s)\n",
                        element->u.misc.unpitched.info.nlba,
                        dump_nlba_to_string(element->u.misc.unpitched.info.
                            nlba));
                break;
            case GRE_ALT:
                fprintf(f, "     Above lines text        \"%s\"\n",
                        element->texverb);
                break;
            case GRE_BAR:
                if (element->u.misc.unpitched.info.bar) {
                    fprintf(f, "     bar                     %d (%s)\n",
                            element->u.misc.unpitched.info.bar,
                            dump_bar_type(element->u.misc.unpitched.info.bar));
                    if (element->u.misc.unpitched.special_sign) {
                        fprintf(f, "     special sign            %d (%s)\n",
                                element->u.misc.unpitched.special_sign,
                                dump_special_sign(element->u.misc.unpitched.
                                                  special_sign));
                    }
                }
                break;
            case GRE_C_KEY_CHANGE:
                if (element->u.misc.pitched.pitch) {
                    fprintf(f, "     clef                    %d (c%d)\n",
                            element->u.misc.pitched.pitch,
                            element->u.misc.pitched.pitch - '0');
                    if (element->u.misc.pitched.flatted_key) {
                        fprintf(f, "     flatted_key             true\n");
                    }
                }
                break;
            case GRE_F_KEY_CHANGE:
                if (element->u.misc.pitched.pitch) {
                    fprintf(f, "     clef                    %d (f%d)\n",
                            element->u.misc.pitched.pitch,
                            element->u.misc.pitched.pitch - '0');
                    if (element->u.misc.pitched.flatted_key) {
                        fprintf(f, "     flatted_key             true\n");
                    }
                }
                break;
            case GRE_END_OF_LINE:
                if (element->u.misc.unpitched.info.sub_type) {
                    fprintf(f, "     sub_type                %d (%s)\n",
                            element->u.misc.unpitched.info.sub_type,
                            dump_type(element->u.misc.unpitched.info.sub_type));
                }
                break;
            case GRE_ELEMENT:
                for (glyph = element->u.first_glyph; glyph;
                        glyph = glyph->next) {
                    gregorio_note *note;
                    fprintf(f, "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
                    if (glyph->type) {
                        fprintf(f, "       type                  %d (%s)\n",
                                glyph->type, dump_type(glyph->type));
                    }
                    switch (glyph->type) {
                    case GRE_TEXVERB_GLYPH:
                        fprintf(f, "       TeX string            \"%s\"\n",
                                glyph->texverb);
                        break;

                    case GRE_SPACE:
                        fprintf(f, "       space                 %d (%s)\n",
                                glyph->u.misc.unpitched.info.space,
                                dump_space_type(glyph->u.misc.unpitched.info.
                                                space));
                        break;

                    case GRE_BAR:
                        fprintf(f, "       glyph_type            %d (%s)\n",
                                glyph->u.misc.unpitched.info.bar,
                                dump_bar_type(glyph->u.misc.unpitched.info.
                                              bar));
                        if (glyph->u.misc.unpitched.special_sign) {
                            fprintf(f, "       special sign          %d (%s)\n",
                                    glyph->u.misc.unpitched.special_sign,
                                    dump_special_sign(glyph->u.misc.unpitched.
                                                      special_sign));
                        }
                        break;

                    case GRE_FLAT:
                    case GRE_NATURAL:
                    case GRE_SHARP:
                        fprintf(f, "       pitch                 %s\n",
                                dump_pitch(glyph->u.misc.pitched.pitch));
                        break;

                    case GRE_GLYPH:
                        fprintf(f, "       glyph_type            %d (%s)\n",
                                glyph->u.notes.glyph_type,
                                dump_glyph_type(glyph->u.notes.glyph_type));
                        if (glyph->u.notes.liquescentia) {
                            fprintf(f, "       liquescentia          %d (%s)\n",
                                    glyph->u.notes.liquescentia,
                                    dump_liquescentia(glyph->u.notes.
                                                      liquescentia));
                        }
                        break;

                    default:
                        fprintf(f, "       !!! UNKNOWN !!!       !!!\n");
                        break;
                    }
                    if (glyph->type == GRE_GLYPH) {
                        for (note = glyph->u.notes.first_note; note;
                                note = note->next) {
                            fprintf(f, "-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  \n");
                            if (note->type) {
                                fprintf(f, "         type                   %d (%s)\n",
                                        note->type, dump_type(note->type));
                            }
                            switch (note->type) {
                            case GRE_NOTE:
                                if (note->u.note.pitch) {
                                    fprintf(f, "         pitch                  %s\n",
                                            dump_pitch(note->u.note.pitch));
                                }
                                if (note->u.note.shape) {
                                    fprintf(f, "         shape                  %d (%s)\n",
                                            note->u.note.shape,
                                            dump_shape(note->u.note.shape));
                                }
                                if (note->u.note.liquescentia) {
                                    fprintf(f, "         liquescentia           %d (%s)\n",
                                            note->u.note.liquescentia,
                                            dump_liquescentia(note->u.note.
                                                              liquescentia));
                                }
                                break;

                            default:
                                fprintf(f, "         !!! NOT ALLOWED !!!    !!!\n");
                                break;
                            }
                            if (note->texverb) {
                                fprintf(f, "         TeX string             \"%s\"\n",
                                        note->texverb);
                            }
                            if (note->choral_sign) {
                                fprintf(f, "         Choral Sign            \"%s\"\n",
                                        note->choral_sign);
                            }
                            if (note->signs) {
                                fprintf(f, "         signs                  %d (%s)\n",
                                        note->signs, dump_signs(note->signs));
                            }
                            if (note->signs & _V_EPISEMUS && note->v_episemus_height) {
                                if (note->v_episemus_height < note->u.note.pitch) {
                                    fprintf(f, "         v episemus forced      BELOW\n");
                                }
                                else {
                                    fprintf(f, "         v episemus forced      ABOVE\n");
                                }
                            }
                            if ((note->signs == _PUNCTUM_MORA
                                        || note->signs == _V_EPISEMUS_PUNCTUM_MORA)
                                    && note->mora_vposition) {
                                fprintf(f, "         mora vposition         %s\n",
                                        dump_vposition(note->mora_vposition));
                            }
                            if (note->special_sign) {
                                fprintf(f, "         special sign           %d (%s)\n",
                                        note->special_sign,
                                        dump_special_sign(note->special_sign));
                            }
                            if (note->h_episemus_above == HEPISEMUS_AUTO
                                    && note->h_episemus_below == HEPISEMUS_AUTO) {
                                fprintf(f, "         auto hepisemus size    %d (%s)\n",
                                        note->h_episemus_above_size,
                                        dump_h_episemus_size(note->h_episemus_above_size));
                                fprintf(f, "         auto hepisemus bridge  %s\n",
                                        dump_bool(note->h_episemus_above_connect));
                            }
                            else {
                                if (note->h_episemus_above == HEPISEMUS_FORCED) {
                                    fprintf(f, "         above hepisemus size   %d (%s)\n",
                                            note->h_episemus_above_size,
                                            dump_h_episemus_size(note->h_episemus_above_size));
                                    fprintf(f, "         above hepisemus bridge %s\n",
                                            dump_bool(note->h_episemus_above_connect));
                                }
                                if (note->h_episemus_below == HEPISEMUS_FORCED) {
                                    fprintf(f, "         below hepisemus size   %d (%s)\n",
                                            note->h_episemus_below_size,
                                            dump_h_episemus_size(note->h_episemus_below_size));
                                    fprintf(f, "         below hepisemus bridge %s\n",
                                            dump_bool(note->h_episemus_below_connect));
                                }
                            }
                        }
                    }
                }
                break;

            default:
                /* do nothing */
                break;
            }
            if (element->nabc_lines) {
                fprintf(f, "     nabc_lines              %d\n",
                        (int)element->nabc_lines);
            }
            if (element->nabc_lines && element->nabc) {
                for (i = 0; i < (int)element->nabc_lines; i++) {
                    if (element->nabc[i]) {
                        fprintf(f, "     nabc_line %d             \"%s\"\n",
                                (int)(i+1), element->nabc[i]);
                    }
                }
            }
        }
        fprintf(f, "=====================================================================\n");
    }
}
