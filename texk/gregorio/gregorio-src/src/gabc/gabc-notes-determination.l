%{
/*
 * Gregorio is a program that translates gabc files to GregorioTeX
 * This file implements the note parser.
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
#include <ctype.h> /* for tolower */
#include "bool.h"
#include "struct.h"
#include "messages.h"
#include "support.h"

#include "gabc.h"

#define YY_NO_INPUT

#define YY_USER_ACTION gabc_update_location(&notes_lloc, \
        gabc_notes_determination_text, gabc_notes_determination_leng);

static gregorio_scanner_location notes_lloc;
static gregorio_note *current_note;
static char char_for_brace;
static unsigned int nbof_isolated_episema;
static char *notesmacros[10];
static char tempstr[256];
static int brace_var_counter = 0;
static int overbrace_var = 0, underbrace_var = 0;
static const char *overbrace_var_kind;
static int before_ledger_type;
static char *before_ledger_length = NULL;
static int ledger_var[2] = { 0, 0 };
static unsigned char staff_lines;
static signed char highest_pitch;
static bool legacy_oriscus_orientation;
static int slur_var[2] = { 0, 0 };
static char slur_shift[2] = { '\0', '\0' };
static gregorio_note *slur_start[] = { NULL, NULL };

typedef enum ledger_line_type {
    LL_OVER = 0,
    LL_UNDER = 1
} ledger_line_type;

static __inline char pitch_letter_to_height(const char pitch) {
    char result = pitch - 'a' + LOWEST_PITCH;
    if (pitch == 'p') {
        --result;
    }
    if (result > highest_pitch) {
        gregorio_messagef("pitch_letter_to_height", VERBOSITY_ERROR, 0,
                _("invalid pitch for %u lines: %c"), (unsigned int)staff_lines,
                pitch);
    }
    return result;
}

static gregorio_shape punctum(const char pitch)
{
    if (pitch < 'a') {
        return S_PUNCTUM_INCLINATUM;
    } else {
        return S_PUNCTUM;
    }
}

static __inline void lex_add_note(int i, gregorio_shape shape, char signs,
        char liquescentia)
{
    nbof_isolated_episema = 0;
    gregorio_add_note(&current_note, pitch_letter_to_height(
            tolower((unsigned char)gabc_notes_determination_text[i])),
            shape, signs, liquescentia, NULL, &notes_lloc);

    if (ledger_var[LL_OVER]) {
        current_note->supposed_high_ledger_line = true;
        current_note->explicit_high_ledger_line = true;
    }

    if (ledger_var[LL_UNDER]) {
        current_note->supposed_low_ledger_line = true;
        current_note->explicit_low_ledger_line = true;
    }
}

static __inline void add_bar_as_note(gregorio_bar bar)
{
    nbof_isolated_episema = 0;
    gregorio_add_bar_as_note(&current_note, bar, &notes_lloc);
}

static __inline void error(void)
{
    gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
            _("undefined macro used: m%d"),
            gabc_notes_determination_text[3] - '0');
}

static void add_h_episema(void)
{
    grehepisema_size size = H_NORMAL;
    gregorio_vposition vposition = VPOS_AUTO;
    bool disable_bridge = false;

    char *ptr = gabc_notes_determination_text;
    char current;
    /* first character is the underscore */
    while ((current = *(++ptr))) {
        switch(current) {
        case '0':
            vposition = VPOS_BELOW;
            break;
        case '1':
            vposition = VPOS_ABOVE;
            break;
        case '2':
            disable_bridge = true;
            break;
        case '3':
            size = H_SMALL_LEFT;
            break;
        case '4':
            size = H_SMALL_CENTRE;
            break;
        case '5':
            size = H_SMALL_RIGHT;
            break;
        default:
            gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
                    _("unrecognized horizontal episema modifier: %c"),
                    current);
            break;
        };
    }

    gregorio_add_h_episema(current_note, size, vposition, disable_bridge,
                &nbof_isolated_episema);
}

static void add_sign(gregorio_sign sign)
{
    gregorio_vposition vposition = VPOS_AUTO;
    switch(gabc_notes_determination_text[1]) {
    case '0':
        vposition = VPOS_BELOW;
        break;
    case '1':
        vposition = VPOS_ABOVE;
        break;
    }
    gregorio_add_sign(current_note, sign, vposition);
}

static void save_before_ledger(const char *const before_ledger)
{
    if (strcmp(before_ledger, "0") == 0) {
        before_ledger_type = 0;
        before_ledger_length = "";
    } else if (strcmp(before_ledger, "1") == 0) {
        before_ledger_type = 1;
        before_ledger_length = "";
    } else {
        before_ledger_type = 2;
        before_ledger_length = gregorio_strdup(before_ledger);
    }
}

static void add_static_ledger(const ledger_line_type type, const char *length) {
    gregorio_snprintf(tempstr, sizeof tempstr,
            "\\GreDrawAdditionalLine{%d}{%s}{%d}{%s}{0}{}",
            type, length + 1, before_ledger_type, before_ledger_length);

    if (before_ledger_type == 2) {
        free(before_ledger_length);
        before_ledger_length = NULL;
    }

    gregorio_add_texverb_as_note(&current_note, gregorio_strdup(tempstr),
            GRE_TEXVERB_GLYPH, &notes_lloc);
}

static __inline const char *ledger_type_name(const ledger_line_type type) {
    switch (type) {
    case LL_OVER:
        return "over";
    case LL_UNDER:
        return "under";
    }
    gregorio_messagef("ledger_type_name", VERBOSITY_ERROR, 0,
            _("invalid ledger type %d"), type);
    return NULL;
}

static void add_variable_ledger(const ledger_line_type type,
        const char *after_ledger)
{
    if (ledger_var[type]) {
        const char *const typename = ledger_type_name(type);
        gregorio_messagef("add_variable_ledger", VERBOSITY_ERROR, 0,
                _("variable %s-staff ledger line without termination of "
                "previous %s-staff ledger line"), typename, typename);
    } else {
        int after_ledger_type;
        const char *after_ledger_length;

        ++after_ledger;

        if (strcmp(after_ledger, "0") == 0) {
            after_ledger_type = 0;
            after_ledger_length = "";
        } else if (strcmp(after_ledger, "1") == 0) {
            after_ledger_type = 1;
            after_ledger_length = "";
        } else {
            after_ledger_type = 2;
            after_ledger_length = after_ledger;
        }

        ledger_var[type] = ++brace_var_counter;
        gregorio_snprintf(tempstr, sizeof tempstr,
                "\\GreVarBraceSavePos{%d}{0}{1}"
                "\\GreDrawAdditionalLine{%d}{\\GreVarBraceLength{%d}}"
                "{%d}{%s}{%d}{%s}",
                ledger_var[type], type, ledger_var[type], before_ledger_type,
                before_ledger_length, after_ledger_type, after_ledger_length);

        if (before_ledger_type == 2) {
            free(before_ledger_length);
            before_ledger_length = NULL;
        }

        gregorio_add_texverb_as_note(&current_note, gregorio_strdup(tempstr),
                GRE_TEXVERB_GLYPH, &notes_lloc);
    }
}

static void end_variable_ledger(const ledger_line_type type)
{
    if (!ledger_var[type]) {
        const char *const typename = ledger_type_name(type);
        gregorio_messagef("end_variable_ledger", VERBOSITY_ERROR, 0,
                _("variable %s-staff ledger line termination without variable "
                "%s-staff ledger line start"), typename, typename);
    } else {
        gregorio_snprintf(tempstr, sizeof tempstr,
                "\\GreVarBraceSavePos{%d}{0}{2}", ledger_var[type]);
        ledger_var[type] = 0;
        gregorio_add_texverb_as_note(&current_note, gregorio_strdup(tempstr),
                GRE_TEXVERB_GLYPH, &notes_lloc);
    }
}

static __inline int parse_clef_line(char line)
{
    line -= '0';
    if (line < 0 || line > staff_lines) {
        gregorio_messagef("parse_clef_line", VERBOSITY_ERROR, 0,
                _("invalid clef line for %u lines: %d"),
                (unsigned int)staff_lines, (int)line);
        return 1;
    }
    return line;
}

static __inline gregorio_bar parse_dominican_bar(char bar)
{
    bar -= '0';
    if (bar < 1 || bar > (2 * (staff_lines - 1))) {
        gregorio_messagef("parse_dominican_line", VERBOSITY_ERROR, 0,
                _("invalid dominican bar for %u lines: ;%d"),
                (unsigned int)staff_lines, (int)bar);
    }

    switch (bar) {
    case 1:
        return B_DIVISIO_MINOR_D1;
    case 2:
        return B_DIVISIO_MINOR_D2;
    case 3:
        return B_DIVISIO_MINOR_D3;
    case 4:
        return B_DIVISIO_MINOR_D4;
    case 5:
        return B_DIVISIO_MINOR_D5;
    case 6:
        return B_DIVISIO_MINOR_D6;
    case 7:
        return B_DIVISIO_MINOR_D7;
    case 8:
        return B_DIVISIO_MINOR_D8;
    }

    gregorio_messagef("check_dominican_line", VERBOSITY_ERROR, 0,
            _("invalid dominican bar: %d"), (int)bar);
    return B_NO_BAR;
}

static __inline gregorio_clef letter_to_clef(char letter)
{
    switch (letter) {
    case 'c':
        return CLEF_C;
    case 'f':
        return CLEF_F;
    }
    gregorio_messagef("letter_to_clef", VERBOSITY_ERROR, 0,
            _("invalid clef: %c"), letter);
    return CLEF_C;
}

static __inline void slur_assert(char *fn, bool test) {
    if (!test) {
        gregorio_message(_("invalid slur text"), fn, VERBOSITY_FATAL, 0);
        exit(1);
    }
}

static char *parse_slur_shift(char *shift)
{
    char *c;

    c = strchr(gabc_notes_determination_text, ':');
    slur_assert("parse_slur_shift", c != NULL);
    slur_assert("parse_slur_shift", *(++c) != '\0');
    *shift = *c;
    return c;
}

static void parse_slur(int direction)
{
    char shift, *width, *height, *end;

    if (!current_note || current_note->type != GRE_NOTE) {
        gregorio_message(
                _("cannot add a slur to something that is not a note"),
                "parse_slur", VERBOSITY_ERROR, 0);
        return;
    }

    end = parse_slur_shift(&shift);
    width = strchr(end, ';');
    slur_assert("parse_slur", width != NULL);
    height = strchr(++width, ',');
    slur_assert("parse_slur", height != NULL);
    *height = '\0';
    end = strchr(++height, ']');
    slur_assert("parse_slur", end != NULL);
    *end = '\0';

    gregorio_snprintf(tempstr, sizeof tempstr,
            "\\GreSlur{%d}{%d}{%c}{%s}{%s}{}",
            current_note->u.note.pitch + direction, direction, shift, width,
            height);
    gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
}

static void start_var_slur(int index)
{
    if (!current_note || current_note->type != GRE_NOTE) {
        gregorio_message(
                _("cannot add a slur to something that is not a note"),
                "start_var_slur", VERBOSITY_ERROR, 0);
        return;
    }

    if (slur_var[index]) {
        gregorio_message(
                _("variable slur without termination of previous slur"),
                "start_var_slur", VERBOSITY_ERROR, 0);
        return;
    }

    slur_var[index] = ++brace_var_counter;
    parse_slur_shift(slur_shift + index);
    slur_start[index] = current_note;
}

static void end_var_slur(int direction, int index)
{
    char shift;

    if (!current_note || current_note->type != GRE_NOTE) {
        gregorio_message(
                _("cannot add a slur to something that is not a note"),
                "end_var_slur", VERBOSITY_ERROR, 0);
        return;
    }

    if (!slur_var[index] || !slur_shift[index] || !slur_start[index]) {
        gregorio_message(_("variable slur end without variable slur start"),
                "end_var_slur", VERBOSITY_ERROR, 0);
        return;
    }

    parse_slur_shift(&shift);

    gregorio_snprintf(tempstr, sizeof tempstr,
            "\\GreVarBraceSavePos{%d}{%c}{1}"
            "\\GreSlur{%d}{%d}{%c}{\\GreVarBraceLength{%d}}{}{%d}",
            slur_var[index], slur_shift[index],
            slur_start[index]->u.note.pitch + direction, direction,
            slur_shift[index], slur_var[index],
            current_note->u.note.pitch + direction);
    gregorio_add_texverb_to_note(slur_start[index], gregorio_strdup(tempstr));

    gregorio_snprintf(tempstr, sizeof tempstr,
            "\\GreVarBraceSavePos{%d}{%c}{2}", slur_var[index], shift);
    gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));


    slur_var[index] = 0;
    slur_shift[index] = '\0';
    slur_start[index] = NULL;
}

%}

%option stack
%option 8bit
%option pointer
%option nounput
%option noyy_push_state
%option noyy_pop_state
%option noyy_top_state
%option full
%option noread
%option nomain
%option align
%option noyylineno
%option prefix="gabc_notes_determination_"
%option noyywrap

%x texverbnote
%x texverbglyph
%x texverbelement
%x choralsign
%x choralnabc
%x alt
%x comments
%x overbrace
%x underbrace
%x overcurlybrace
%x overcurlyaccentusbrace
%x space
%x nbspace
%x overledger overledger2
%x underledger underledger2
%x endledger

%%
<INITIAL>\% {
        BEGIN(comments);
    }
<comments>(\n|\r)+ {
        BEGIN(INITIAL);
    }
<comments>[^\n\r]* {
        /* ignored */
    }
<INITIAL>\[cs: {
        BEGIN(choralsign);
    }
<INITIAL>\[cn: {
        BEGIN(choralnabc);
    }
<INITIAL>\[ob:[01]; {
        char_for_brace = gabc_notes_determination_text[4]-'0';
        BEGIN(overbrace);
    }
<INITIAL>\[ub:[01]; {
        char_for_brace = gabc_notes_determination_text[4]-'0';
        BEGIN(underbrace);
    }
<INITIAL>\[ocb:[01]; {
        char_for_brace = gabc_notes_determination_text[5]-'0';
        BEGIN(overcurlybrace);
    }
<INITIAL>\[ocba:[01]; {
        char_for_brace = gabc_notes_determination_text[6]-'0';
        BEGIN(overcurlyaccentusbrace);
    }
<INITIAL>\[ob:[01]\{\] {
        if (overbrace_var) {
            gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
                              _("variable overbrace without termination of "
                                "previous overbrace"));
        } else {
            char_for_brace = gabc_notes_determination_text[4]-'0';
            overbrace_var = ++brace_var_counter;
            overbrace_var_kind = "ob";
            gregorio_snprintf(tempstr, sizeof tempstr,
                    "\\GreVarBraceSavePos{%d}{%d}{1}"
                    "\\GreOverBrace{\\GreVarBraceLength{%d}}{0pt}{0pt}{%d}",
                    overbrace_var, char_for_brace, overbrace_var, char_for_brace);
            gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
        }
    }
<INITIAL>\[ub:[01]\{\] {
        if (underbrace_var) {
            gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
                              _("variable underbrace without termination of "
                                "previous underbrace"));
        } else {
            char_for_brace = gabc_notes_determination_text[4]-'0';
            underbrace_var = ++brace_var_counter;
            gregorio_snprintf(tempstr, sizeof tempstr,
                    "\\GreVarBraceSavePos{%d}{%d}{1}"
                    "\\GreUnderBrace{\\GreVarBraceLength{%d}}{0pt}{0pt}{%d}",
                    underbrace_var, char_for_brace, underbrace_var, char_for_brace);
            gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
        }
    }
<INITIAL>\[ocb:[01]\{\] {
        if (overbrace_var) {
            gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
                              _("variable overbrace without termination of "
                                "previous overbrace"));
        } else {
            char_for_brace = gabc_notes_determination_text[5]-'0';
            overbrace_var = ++brace_var_counter;
            overbrace_var_kind = "ocb";
            gregorio_snprintf(tempstr, sizeof tempstr,
                    "\\GreVarBraceSavePos{%d}{%d}{1}"
                    "\\GreOverCurlyBrace{\\GreVarBraceLength{%d}}{0pt}{0pt}{%d}{0}",
                    overbrace_var, char_for_brace, overbrace_var, char_for_brace);
            gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
        }
    }
<INITIAL>\[ocba:[01]\{\] {
        if (overbrace_var) {
            gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
                              _("variable overbrace without termination of "
                                "previous overbrace"));
        } else {
            char_for_brace = gabc_notes_determination_text[6]-'0';
            overbrace_var = ++brace_var_counter;
            overbrace_var_kind = "ocba";
            gregorio_snprintf(tempstr, sizeof tempstr,
                    "\\GreVarBraceSavePos{%d}{%d}{1}"
                    "\\GreOverCurlyBrace{\\GreVarBraceLength{%d}}{0pt}{0pt}{%d}{1}",
                    overbrace_var, char_for_brace, overbrace_var, char_for_brace);
            gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
        }
    }
<INITIAL>\[ob:[01]\}\] {
        if (!overbrace_var) {
            gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
                              _("variable overbrace termination without "
                                "variable overbrace start"));
        } else if (strcmp (overbrace_var_kind, "ob")) {
            gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
                              _("variable overbrace started with %s "
                                "and terminated with ob"),
                              overbrace_var_kind);
        } else {
            char_for_brace = gabc_notes_determination_text[4]-'0';
            gregorio_snprintf(tempstr, sizeof tempstr,
                    "\\GreVarBraceSavePos{%d}{%d}{2}", overbrace_var,
                    char_for_brace);
            overbrace_var = 0;
            gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
        }
    }
<INITIAL>\[ub:[01]\}\] {
        if (!underbrace_var) {
            gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
                              _("variable underbrace termination without "
                                "variable underbrace start"));
        } else {
            char_for_brace = gabc_notes_determination_text[4]-'0';
            gregorio_snprintf(tempstr, sizeof tempstr,
                    "\\GreVarBraceSavePos{%d}{%d}{2}", underbrace_var,
                    char_for_brace);
            underbrace_var = 0;
            gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
        }
    }
<INITIAL>\[ocb:[01]\}\] {
        if (!overbrace_var) {
            gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
                              _("variable overbrace termination without "
                                "variable overbrace start"));
        } else if (strcmp (overbrace_var_kind, "ocb")) {
            gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
                              _("variable overbrace started with %s "
                                "and terminated with ocb"),
                              overbrace_var_kind);
        } else {
            char_for_brace = gabc_notes_determination_text[5]-'0';
            gregorio_snprintf(tempstr, sizeof tempstr,
                    "\\GreVarBraceSavePos{%d}{%d}{2}", overbrace_var,
                    char_for_brace);
            overbrace_var = 0;
            gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
        }
    }
<INITIAL>\[ocba:[01]\}\] {
        if (!overbrace_var) {
            gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
                              _("variable overbrace termination without "
                                "variable overbrace start"));
        } else if (strcmp (overbrace_var_kind, "ocba")) {
            gregorio_messagef("gabc_notes_determination", VERBOSITY_ERROR, 0,
                              _("variable overbrace started with %s "
                                "and terminated with ocba"),
                              overbrace_var_kind);
        } else {
            char_for_brace = gabc_notes_determination_text[6]-'0';
            gregorio_snprintf(tempstr, sizeof tempstr,
                    "\\GreVarBraceSavePos{%d}{%d}{2}", overbrace_var,
                    char_for_brace);
            overbrace_var = 0;
            gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
        }
    }
<INITIAL>\[nm[1-9]\] {
        if (notesmacros[gabc_notes_determination_text[3]-'0']) {
            gregorio_add_texverb_to_note(current_note,
            gregorio_strdup(notesmacros[gabc_notes_determination_text[3]-'0']));
        } else error();
    }
<INITIAL>\[gm[1-9]\] {
        if (notesmacros[gabc_notes_determination_text[3]-'0']) {
            gregorio_add_texverb_as_note(&current_note,
            gregorio_strdup(notesmacros[gabc_notes_determination_text[3]-'0']),
            GRE_TEXVERB_GLYPH, &notes_lloc);
        } else error();
    }
<INITIAL>\[em[1-9]\] {
        if (notesmacros[gabc_notes_determination_text[3]-'0']) {
            gregorio_add_texverb_as_note(&current_note,
            gregorio_strdup(notesmacros[gabc_notes_determination_text[3]-'0']),
            GRE_TEXVERB_ELEMENT, &notes_lloc);
        } else error();
    }
<INITIAL>\[altm[1-9]\] {
        if (notesmacros[gabc_notes_determination_text[5]-'0']) {
            gregorio_add_texverb_as_note(&current_note,
            gregorio_strdup(notesmacros[gabc_notes_determination_text[5]-'0']),
            GRE_TEXVERB_ELEMENT, &notes_lloc);
        } else error();
    }
<INITIAL>\[nv: {
        BEGIN(texverbnote);
    }
<INITIAL>\[gv: {
        BEGIN(texverbglyph);
    }
<INITIAL>\[ev: {
        BEGIN(texverbelement);
    }
<INITIAL>\[alt: {
        BEGIN(alt);
    }
<INITIAL><nlba> {
        gregorio_add_nlba_as_note(&current_note, NLBA_BEGINNING, &notes_lloc);
    }
<INITIAL><\/nlba> {
        gregorio_add_nlba_as_note(&current_note, NLBA_END, &notes_lloc);
    }
<INITIAL>@\[ {
        gregorio_start_autofuse(&current_note, &notes_lloc);
    }
<INITIAL>\] {
        gregorio_end_autofuse(&current_note, &notes_lloc);
    }
<overbrace>[^\]]+ {
        gregorio_snprintf(tempstr, sizeof tempstr,
                "\\GreOverBrace{%s}{0pt}{0pt}{%d}",
                gabc_notes_determination_text, char_for_brace);
        gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
    }
<underbrace>[^\]]+ {
        gregorio_snprintf(tempstr, sizeof tempstr,
                "\\GreUnderBrace{%s}{0pt}{0pt}{%d}",
                gabc_notes_determination_text, char_for_brace);
        gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
    }
<overcurlybrace>[^\]]+ {
        gregorio_snprintf(tempstr, sizeof tempstr,
                "\\GreOverCurlyBrace{%s}{0pt}{0pt}{%d}{0}",
                gabc_notes_determination_text, char_for_brace);
        gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
    }
<overcurlyaccentusbrace>[^\]]+ {
        gregorio_snprintf(tempstr, sizeof tempstr,
                "\\GreOverCurlyBrace{%s}{0pt}{0pt}{%d}{1}",
                gabc_notes_determination_text, char_for_brace);
        gregorio_add_texverb_to_note(current_note, gregorio_strdup(tempstr));
    }
<choralsign>[^\]]+ {
        gregorio_add_cs_to_note(&current_note,
                gregorio_strdup(gabc_notes_determination_text), false);
    }
<choralnabc>[^\]]+ {
        gregorio_add_cs_to_note(&current_note,
                gregorio_strdup(gabc_notes_determination_text), true);
    }
<texverbnote>[^\]]+ {
        gregorio_add_texverb_to_note(current_note,
                gregorio_strdup(gabc_notes_determination_text));
    }
<texverbglyph>[^\]]+ {
        gregorio_add_texverb_as_note(&current_note,
                gregorio_strdup(gabc_notes_determination_text),
                GRE_TEXVERB_GLYPH, &notes_lloc);
    }
<texverbelement>[^\]]+ {
        gregorio_add_texverb_as_note(&current_note,
                gregorio_strdup(gabc_notes_determination_text),
                GRE_TEXVERB_ELEMENT, &notes_lloc);
    }
<alt>[^\]]+ {
        gregorio_add_texverb_as_note(&current_note,
                gregorio_strdup(gabc_notes_determination_text), GRE_ALT,
                &notes_lloc);
    }
<INITIAL>\/\[ {
        BEGIN(space);
    }
<space>-?(\.[0-9]+|[0-9]+(\.[0-9]*)?)/\] {
        gregorio_add_space_as_note(&current_note, SP_AD_HOC_SPACE,
                gregorio_strdup(gabc_notes_determination_text), &notes_lloc);
    }
<INITIAL>!\/\[ {
        BEGIN(nbspace);
    }
<nbspace>-?(\.[0-9]+|[0-9]+(\.[0-9]*)?)/\] {
        gregorio_add_space_as_note(&current_note, SP_AD_HOC_SPACE_NB,
                gregorio_strdup(gabc_notes_determination_text), &notes_lloc);
    }
<INITIAL>\[oll:\}\] {
        end_variable_ledger(LL_OVER);
    }
<INITIAL>\[oll: {
        BEGIN(overledger);
    }
<overledger>[^;{]+ {
        save_before_ledger(gabc_notes_determination_text);
        BEGIN(overledger2);
    }
<overledger2>;[^\]]+ {
        add_static_ledger(LL_OVER, gabc_notes_determination_text);
        BEGIN(endledger);
    }
<overledger2>\{[^\]]+ {
        add_variable_ledger(LL_OVER, gabc_notes_determination_text);
        BEGIN(endledger);
    }
<INITIAL>\[ull:\}\] {
        end_variable_ledger(LL_UNDER);
    }
<INITIAL>\[ull: {
        BEGIN(underledger);
    }
<underledger>[^;{]+ {
        save_before_ledger(gabc_notes_determination_text);
        BEGIN(underledger2);
    }
<underledger2>;[^\]]+ {
        add_static_ledger(LL_UNDER, gabc_notes_determination_text);
        BEGIN(endledger);
    }
<underledger2>\{[^\]]+ {
        add_variable_ledger(LL_UNDER, gabc_notes_determination_text);
        BEGIN(endledger);
    }
<texverbnote,texverbglyph,texverbelement,choralsign,choralnabc,alt,overcurlyaccentusbrace,overcurlybrace,overbrace,underbrace,space,nbspace,endledger>\] {
        BEGIN(INITIAL);
    }
<INITIAL>\[oslur:[012];[^,]+,[^\]]+\] {
        parse_slur(1);
    }
<INITIAL>\[oslur:[012]\{\] {
        start_var_slur(0);
    }
<INITIAL>\[oslur:[012]\}\] {
        end_var_slur(1, 0);
    }
<INITIAL>\[uslur:[012];[^,]+,[^\]]+\] {
        parse_slur(-1);
    }
<INITIAL>\[uslur:[012]\{\] {
        start_var_slur(1);
    }
<INITIAL>\[uslur:[012]\}\] {
        end_var_slur(-1, 1);
    }
\{  {
        gregorio_add_texverb_as_note(&current_note,
                gregorio_strdup("\\hbox to 0pt{"), GRE_TEXVERB_ELEMENT,
                &notes_lloc);
    }
\}  {
        gregorio_add_texverb_as_note(&current_note,
                gregorio_strdup("\\hss%\n}%\n\\GreNoBreak\\relax "),
                GRE_TEXVERB_ELEMENT, &notes_lloc);
    }
[a-np]\+ {
        gregorio_add_manual_custos_as_note(&current_note,
                pitch_letter_to_height(gabc_notes_determination_text[0]),
                &notes_lloc);
    }
[\t\r\n]+ /* ignore ends of line and tabs */;
z0  {
        gregorio_add_custo_as_note(&current_note, &notes_lloc);
    }
z   {
        gregorio_add_end_of_line_as_note(&current_note, false, false, false,
                &notes_lloc);
    }
z\+ {
        gregorio_add_end_of_line_as_note(&current_note, false, true, true,
                &notes_lloc);
    }
z-  {
        gregorio_add_end_of_line_as_note(&current_note, false, true, false,
                &notes_lloc);
    }
Z   {
        gregorio_add_end_of_line_as_note(&current_note, true, false, false,
                &notes_lloc);
    }
Z\+ {
        gregorio_add_end_of_line_as_note(&current_note, true, true, true,
                &notes_lloc);
    }
Z-  {
        gregorio_add_end_of_line_as_note(&current_note, true, true, false,
                &notes_lloc);
    }
[cf][1-5] {
        gregorio_add_clef_as_note(&current_note,
                letter_to_clef(gabc_notes_determination_text[0]),
                parse_clef_line(gabc_notes_determination_text[1]), false,
                &notes_lloc);
    }
[cf]b[1-5] {
        gregorio_add_clef_as_note(&current_note,
                letter_to_clef(gabc_notes_determination_text[0]),
                parse_clef_line(gabc_notes_determination_text[2]), true,
                &notes_lloc);
    }
@[cf][1-5] {
        gregorio_add_secondary_clef_to_note(current_note,
                letter_to_clef(gabc_notes_determination_text[1]),
                parse_clef_line(gabc_notes_determination_text[2]), false);
    }
@[cf]b[1-5] {
        gregorio_add_secondary_clef_to_note(current_note,
                letter_to_clef(gabc_notes_determination_text[1]),
                parse_clef_line(gabc_notes_determination_text[3]), true);
    }
`   {
        add_bar_as_note(B_VIRGULA);
    }
,   {
        add_bar_as_note(B_DIVISIO_MINIMA);
    }
[,;][1-8] {
        add_bar_as_note(parse_dominican_bar(gabc_notes_determination_text[1]));
    }
;   {
        add_bar_as_note(B_DIVISIO_MINOR);
    }
:   {
        add_bar_as_note(B_DIVISIO_MAIOR);
    }
::  {
        add_bar_as_note(B_DIVISIO_FINALIS);
    }
r   {
        gregorio_change_shape(current_note, S_PUNCTUM_CAVUM,
                legacy_oriscus_orientation);
    }
R   {
        gregorio_change_shape(current_note, S_LINEA_PUNCTUM,
                legacy_oriscus_orientation);
    }
r0  {
        gregorio_change_shape(current_note, S_LINEA_PUNCTUM_CAVUM,
                legacy_oriscus_orientation);
    }
r1  {
        gregorio_add_special_sign(current_note, _ACCENTUS);
    }
r2  {
        gregorio_add_special_sign(current_note, _ACCENTUS_REVERSUS);
    }
r3  {
        gregorio_add_special_sign(current_note, _CIRCULUS);
    }
r4  {
        gregorio_add_special_sign(current_note, _SEMI_CIRCULUS);
    }
r5  {
        gregorio_add_special_sign(current_note, _SEMI_CIRCULUS_REVERSUS);
    }
x   {
        gregorio_change_shape(current_note, S_FLAT, legacy_oriscus_orientation);
    }
#   {
        gregorio_change_shape(current_note, S_SHARP,
                legacy_oriscus_orientation);
    }
y   {
        gregorio_change_shape(current_note, S_NATURAL,
                legacy_oriscus_orientation);
    }
!?\/0 {
        gregorio_add_space_as_note(&current_note, SP_HALF_SPACE, NULL,
                &notes_lloc);
    }
\/  {
        gregorio_add_space_as_note(&current_note, SP_NEUMATIC_CUT, NULL,
                &notes_lloc);
    }
\//\/\[ {
        gregorio_add_space_as_note(&current_note, SP_NEUMATIC_CUT, NULL,
                &notes_lloc);
    }
\/\/ {
        gregorio_add_space_as_note(&current_note, SP_LARGER_SPACE, NULL,
                &notes_lloc);
    }
\   {
        gregorio_add_space_as_note(&current_note, SP_GLYPH_SPACE, NULL,
                &notes_lloc);
    }
!\/ {
        gregorio_add_space_as_note(&current_note, SP_NEUMATIC_CUT_NB, NULL,
                &notes_lloc);
    }
!\//\/\[ {
        gregorio_add_space_as_note(&current_note, SP_NEUMATIC_CUT_NB, NULL,
                &notes_lloc);
    }
!\/\/ {
        gregorio_add_space_as_note(&current_note, SP_LARGER_SPACE_NB, NULL,
                &notes_lloc);
    }
!\  {
        gregorio_add_space_as_note(&current_note, SP_GLYPH_SPACE_NB, NULL,
                &notes_lloc);
    }
!/[^\/ ] {
        gregorio_add_space_as_note(&current_note, SP_ZERO_WIDTH, NULL,
                &notes_lloc);
    }
=   {
        gregorio_change_shape(current_note, S_LINEA,
                legacy_oriscus_orientation);
    }
[a-npA-NP]vv {
        lex_add_note(0, S_BIVIRGA, _NO_SIGN, L_NO_LIQUESCENTIA);
    }
[a-npA-NP]vvv {
        lex_add_note(0, S_TRIVIRGA, _NO_SIGN, L_NO_LIQUESCENTIA);
    }
[a-npA-NP]VV {
        lex_add_note(0, S_BIVIRGA, _NO_SIGN, L_NO_LIQUESCENTIA);
    }
[a-npA-NP]VVV {
        lex_add_note(0, S_TRIVIRGA, _NO_SIGN, L_NO_LIQUESCENTIA);
    }
[a-npA-NP]ss {
        lex_add_note(0, S_DISTROPHA, _NO_SIGN, L_NO_LIQUESCENTIA);
    }
[a-npA-NP]ss(\<|\>) {
        lex_add_note(0, S_DISTROPHA, _NO_SIGN, L_AUCTUS_ASCENDENS);
    }
[a-npA-NP]sss {
        lex_add_note(0, S_TRISTROPHA, _NO_SIGN, L_NO_LIQUESCENTIA);
    }
[a-npA-NP]sss(\<|\>) {
        lex_add_note(0, S_TRISTROPHA, _NO_SIGN, L_AUCTUS_ASCENDENS);
    }
[a-npA-NP] {
        lex_add_note(0, punctum(gabc_notes_determination_text[0]), _NO_SIGN,
                L_NO_LIQUESCENTIA);
    }
-[a-npA-NP] {
        lex_add_note(1, punctum(gabc_notes_determination_text[1]), _NO_SIGN,
                L_INITIO_DEBILIS);
    }
@[a-npA-NP] {
        lex_add_note(1, punctum(gabc_notes_determination_text[1]), _NO_SIGN,
                L_FUSED);
    }
\'[01]? {
        add_sign(_V_EPISEMA);
    }
_[0-5]* {
        add_h_episema();
    }
\.[01]? {
        add_sign(_PUNCTUM_MORA);
    }
~   {
        gregorio_add_tail_liquescentia(current_note, L_DEMINUTUS,
                legacy_oriscus_orientation);
    }
>   {
        gregorio_add_tail_liquescentia(current_note, L_AUCTUS_DESCENDENS,
                legacy_oriscus_orientation);
    }
\<  {
        gregorio_add_tail_liquescentia(current_note, L_AUCTUS_ASCENDENS,
                legacy_oriscus_orientation);
    }
q   {
        gregorio_change_shape(current_note, S_QUADRATUM,
                legacy_oriscus_orientation);
    }
o   {
        gregorio_change_shape(current_note, legacy_oriscus_orientation
                ? S_ORISCUS_ASCENDENS : S_ORISCUS_UNDETERMINED,
                legacy_oriscus_orientation);
    }
O   {
        gregorio_change_shape(current_note, S_ORISCUS_SCAPUS,
                legacy_oriscus_orientation);
    }
w   {
        gregorio_change_shape(current_note, S_QUILISMA,
                legacy_oriscus_orientation);
    }
W   {
        gregorio_change_shape(current_note, S_QUILISMA_QUADRATUM,
                legacy_oriscus_orientation);
    }
v   {
        gregorio_change_shape(current_note, S_VIRGA,
                legacy_oriscus_orientation);
    }
V   {
        gregorio_change_shape(current_note, S_VIRGA_REVERSA,
                legacy_oriscus_orientation);
    }
s   {
        gregorio_change_shape(current_note, S_STROPHA,
                legacy_oriscus_orientation);
    }
\[hl:1\] {
        current_note->supposed_high_ledger_line = true;
        current_note->explicit_high_ledger_line = true;
    }
\[hl:0\] {
        current_note->supposed_high_ledger_line = false;
        current_note->explicit_high_ledger_line = true;
    }
\[ll:1\] {
        current_note->supposed_low_ledger_line = true;
        current_note->explicit_low_ledger_line = true;
    }
\[ll:0\] {
        current_note->supposed_low_ledger_line = false;
        current_note->explicit_low_ledger_line = true;
    }
.   {
        gregorio_messagef("det_notes_from_string", VERBOSITY_ERROR, 0,
                _("unrecognized character: \"%c\""),
                gabc_notes_determination_text[0]);
    }

%%

gregorio_note *gabc_det_notes_from_string(char *str, char *newmacros[10],
        gregorio_scanner_location *loc, const gregorio_score *const score)
{
    int i;
    YY_BUFFER_STATE buf;

    notes_lloc.first_line = loc->first_line;
    notes_lloc.first_column = loc->first_column;
    notes_lloc.first_offset = loc->first_offset;
    /* yes... I do mean to set values from loc->first_* */
    notes_lloc.last_line = loc->first_line;
    notes_lloc.last_column = loc->first_column;
    notes_lloc.last_offset = loc->first_offset;

    staff_lines = score->staff_lines;
    highest_pitch = score->highest_pitch;
    legacy_oriscus_orientation = score->legacy_oriscus_orientation;

    /* a small optimization could uccur here: we could do it only once at the
     * beginning of the score, not at each syllable */
    for (i = 0; i < 10; i++) {
        notesmacros[i] = newmacros[i];
    }
    nbof_isolated_episema = 0;
    current_note = NULL;
    buf = yy_scan_string(str);
    yylex();
    yy_flush_buffer(buf);
    yy_delete_buffer(buf);
    gregorio_go_to_first_note(&current_note);
    return current_note;
}
