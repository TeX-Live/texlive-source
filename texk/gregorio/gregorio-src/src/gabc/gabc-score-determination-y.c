/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         gabc_score_determination_parse
#define yylex           gabc_score_determination_lex
#define yyerror         gabc_score_determination_error
#define yydebug         gabc_score_determination_debug
#define yynerrs         gabc_score_determination_nerrs

#define yylval          gabc_score_determination_lval
#define yychar          gabc_score_determination_char
#define yylloc          gabc_score_determination_lloc

/* Copy the first part of user declarations.  */
#line 1 "gabc/gabc-score-determination.y" /* yacc.c:339  */

/*
 * Gregorio is a program that translates gabc files to GregorioTeX
 * This file implements the score parser.
 *
 * Gregorio score determination in gabc input.
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

/*
 * 
 * This file is certainly not the most easy to understand, it is a bison file.
 * See the bison manual on gnu.org for further details.
 * 
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bool.h"
#include "struct.h"
#include "unicode.h"
#include "messages.h"
#include "characters.h"
#include "support.h"
#include "sha1.h"
#include "plugins.h"
#include "gabc.h"

#define YYLLOC_DEFAULT(Current, Rhs, N) \
    if (N) { \
        (Current).first_line   = YYRHSLOC(Rhs, 1).first_line; \
        (Current).first_column = YYRHSLOC (Rhs, 1).first_column; \
        (Current).first_offset = YYRHSLOC (Rhs, 1).first_offset; \
        (Current).last_line    = YYRHSLOC (Rhs, N).last_line; \
        (Current).last_column  = YYRHSLOC (Rhs, N).last_column; \
        (Current).last_offset  = YYRHSLOC (Rhs, N).last_offset; \
    } else { \
        (Current).first_line   = (Current).last_line   = YYRHSLOC (Rhs, 0).last_line; \
        (Current).first_column = (Current).last_column = YYRHSLOC (Rhs, 0).last_column; \
        (Current).first_offset = (Current).last_offset = YYRHSLOC (Rhs, 0).last_offset; \
    }

#include "gabc-score-determination.h"
#include "gabc-score-determination-l.h"

/* uncomment it if you want to have an interactive shell to understand the
 * details on how bison works for a certain input */
/* int gabc_score_determination_debug=1; */

/*
 * 
 * We will need some variables and functions through the entire file, we
 * declare them there:
 * 
 */

/* the score that we will determine and return */
static gregorio_score *score;
/* an array of elements that we will use for each syllable */
static gregorio_element **elements;
gregorio_element *current_element;
/* a table containing the macros to use in gabc file */
static char *macros[10];
/* forward declaration of the flex/bison process function */
static int gabc_score_determination_parse(void);
/* other variables that we will have to use */
static gregorio_character *current_character;
static gregorio_character *first_text_character;
static gregorio_character *first_translation_character;
static gregorio_tr_centering translation_type;
static gregorio_nlba no_linebreak_area;
static gregorio_euouae euouae;
static gregorio_voice_info *current_voice_info;
static int number_of_voices;
static int voice;
/* see comments on text to understand this */
static gregorio_center_determination center_is_determined;
/* current_key is... the current key... updated by each notes determination
 * (for key changes) */
static int current_key;
static bool got_language = false;
static bool got_staff_lines = false;
static bool started_first_word = false;
static struct sha1_ctx digester;

static __inline void check_multiple(const char *name, bool exists) {
    if (exists) {
        gregorio_messagef("det_score", VERBOSITY_WARNING, 0,
                _("several %s definitions found, only the last will be taken "
                "into consideration"), name);
    }
}

static void gabc_score_determination_error(const char *error_str)
{
    gregorio_message(error_str, (const char *) "gabc_score_determination_parse",
            VERBOSITY_ERROR, 0);
}

static void fix_custos(gregorio_score *score_to_check)
{
    gregorio_syllable *current_syllable;
    gregorio_element *current_element;
    gregorio_element *custo_element;
    char pitch = 0;
    char pitch_difference = 0;
    int newkey;
    int current_key;
    if (!score_to_check || !score_to_check->first_syllable
            || !score_to_check->first_voice_info) {
        return;
    }
    current_key = gregorio_calculate_new_key(
            score_to_check->first_voice_info->initial_clef);
    current_syllable = score_to_check->first_syllable;
    while (current_syllable) {
        current_element = (current_syllable->elements)[0];
        while (current_element) {
            if (current_element->type == GRE_CUSTOS) {
                custo_element = current_element;
                pitch = custo_element->u.misc.pitched.pitch;
                /* we look for the key */
                while (current_element) {
                    if (current_element->type == GRE_CLEF) {
                        pitch = gregorio_determine_next_pitch( current_syllable,
                                current_element, NULL);
                        newkey = gregorio_calculate_new_key(
                                current_element->u.misc.clef);
                        pitch_difference = (char) newkey - (char) current_key;
                        pitch -= pitch_difference;
                        current_key = newkey;
                    }
                    if (!custo_element->u.misc.pitched.force_pitch) {
                        while (pitch < LOWEST_PITCH) {
                            pitch += 7;
                        }
                        while (pitch > score_to_check->highest_pitch) {
                            pitch -= 7;
                        }
                        custo_element->u.misc.pitched.pitch = pitch;
                    }
                    assert(custo_element->u.misc.pitched.pitch >= LOWEST_PITCH 
                            && custo_element->u.misc.pitched.pitch
                            <= score_to_check->highest_pitch);
                    current_element = current_element->next;
                }
            }
            if (current_element) {
                if (current_element->type == GRE_CLEF) {
                    current_key = gregorio_calculate_new_key(
                            current_element->u.misc.clef);
                }
                current_element = current_element->next;
            }
        }
        current_syllable = current_syllable->next_syllable;
    }
}

/*
 * A function that checks to score integrity. For now it is... quite
 * ridiculous... but it might be improved in the future. 
 */

static int check_score_integrity(gregorio_score *score_to_check)
{
    if (!score_to_check) {
        return 0;
    }
    return 1;
}

/*
 * Another function to be improved: this one checks the validity of the voice_infos.
 */

static int check_infos_integrity(gregorio_score *score_to_check)
{
    if (!score_to_check->name) {
        gregorio_message(_("no name specified, put `name:...;' at the "
                "beginning of the file, can be dangerous with some output "
                "formats"), "det_score", VERBOSITY_WARNING, 0);
    }
    return 1;
}

/*
 * The function that will initialize the variables. 
 */

static void initialize_variables(void)
{
    int i;
    /* build a brand new empty score */
    score = gregorio_new_score();
    /* initialization of the first voice info to an empty voice info */
    current_voice_info = NULL;
    gregorio_add_voice_info(&current_voice_info);
    score->first_voice_info = current_voice_info;
    /* other initializations */
    number_of_voices = 0;
    voice = 1;
    current_character = NULL;
    first_translation_character = NULL;
    first_text_character = NULL;
    translation_type = TR_NORMAL;
    no_linebreak_area = NLBA_NORMAL;
    euouae = EUOUAE_NORMAL;
    center_is_determined = CENTER_NOT_DETERMINED;
    current_key = gregorio_calculate_new_key(gregorio_default_clef);
    for (i = 0; i < 10; i++) {
        macros[i] = NULL;
    }
    got_language = false;
    got_staff_lines = false;
    started_first_word = false;
}

/*
 * function that frees the variables that need it, for when we have finished to 
 * determine the score 
 */

static void free_variables(void)
{
    int i;
    free(elements);
    for (i = 0; i < 10; i++) {
        free(macros[i]);
    }
}

/* see whether a voice_info is empty */
static int voice_info_is_not_empty(const gregorio_voice_info *voice_info)
{
    return (voice_info->initial_clef.line);
}

/*
 * a function called when we see "--\n" that end the infos for a certain voice 
 */
static void next_voice_info(void)
{
    /* we must do this test in the case where there would be a "--" before
     * first_declarations */
    if (voice_info_is_not_empty(current_voice_info)) {
        gregorio_add_voice_info(&current_voice_info);
        voice++;
    }
}

/*
 * Function that frees the voice_infos for voices > final_count. Useful if
 * there are too many voice_infos 
 */

static void reajust_voice_infos(gregorio_voice_info *voice_info,
        int final_count)
{
    int i = 1;
    while (voice_info && i <= final_count) {
        voice_info = voice_info->next_voice_info;
    }
    gregorio_free_voice_infos(voice_info);
}

/*
 * Function called when we have reached the end of the definitions, it tries to 
 * make the voice_infos coherent. 
 */
static void end_definitions(void)
{
    int i;

    if (!check_infos_integrity(score)) {
        gregorio_message(_("can't determine valid infos on the score"),
                "det_score", VERBOSITY_ERROR, 0);
    }
    if (!number_of_voices) {
        if (voice > MAX_NUMBER_OF_VOICES) {
            voice = MAX_NUMBER_OF_VOICES;
            reajust_voice_infos(score->first_voice_info, number_of_voices);
        }
        number_of_voices = voice;
        score->number_of_voices = voice;
    } else {
        if (number_of_voices > voice) {
            gregorio_messagef("det_score", VERBOSITY_WARNING, 0,
                    ngt_("not enough voice infos found: %d found, %d waited, "
                    "%d assumed", "not enough voice infos found: %d found, %d "
                    "waited, %d assumed", voice), voice,
                    number_of_voices, voice);
            score->number_of_voices = voice;
            number_of_voices = voice;
        } else {
            if (number_of_voices < voice) {
                gregorio_messagef("det_score", VERBOSITY_WARNING, 0,
                        ngt_("too many voice infos found: %d found, %d "
                        "waited, %d assumed", "not enough voice infos found: "
                        "%d found, %d waited, %d assumed",
                        number_of_voices), voice, number_of_voices,
                        number_of_voices);
            }
        }
    }
    /* voice is now voice-1, so that it can be the index of elements */
    voice = 0;
    elements = (gregorio_element **) gregorio_malloc(number_of_voices *
            sizeof(gregorio_element *));
    for (i = 0; i < number_of_voices; i++) {
        elements[i] = NULL;
    }

    if (!got_language) {
        static char latin[] = "Latin";
        gregorio_set_centering_language(latin);
    }
}

/*
 * Here starts the code for the determinations of the notes. The notes are not
 * precisely determined here, we separate the text describing the notes of each 
 * voice, and we call determine_elements_from_string to really determine them. 
 */
static char position = WORD_BEGINNING;
static gregorio_syllable *current_syllable = NULL;
static char *abovelinestext = NULL;

/*
 * Function called when we see a ")", it completes the gregorio_element array
 * of the syllable with NULL pointers. Usefull in the cases where for example
 * you have two voices, but a voice that is silent on a certain syllable. 
 */
static void complete_with_nulls(int last_voice)
{
    int i;
    for (i = last_voice + 1; i < number_of_voices; i++) {
        elements[i] = NULL;
    }
}

/*
 * Function called each time we find a space, it updates the current position. 
 */
static void update_position_with_space(void)
{
    if (position == WORD_MIDDLE) {
        position = WORD_END;
    }
    if (position == WORD_BEGINNING) {
        position = WORD_ONE_SYLLABLE;
    }
}

/*
 * When we encounter a translation center ending, we call this function that
 * sets translation_type = TR_WITH_CENTER_BEGINNING on previous syllable with
 * translation 
 */
static void gregorio_set_translation_center_beginning(
        gregorio_syllable *current_syllable)
{
    gregorio_syllable *syllable = current_syllable->previous_syllable;
    while (syllable) {
        if (syllable->translation_type == TR_WITH_CENTER_END) {
            gregorio_message("encountering translation centering end but "
                    "cannot find translation centering beginning...",
                    "set_translation_center_beginning", VERBOSITY_ERROR, 0);
            current_syllable->translation_type = TR_NORMAL;
            return;
        }
        if (syllable->translation) {
            syllable->translation_type = TR_WITH_CENTER_BEGINNING;
            return;
        }
        syllable = syllable->previous_syllable;
    }
    /* we didn't find any beginning... */
    gregorio_message("encountering translation centering end but cannot find "
            "translation centering beginning...",
            "set_translation_center_beginning", VERBOSITY_ERROR, 0);
    current_syllable->translation_type = TR_NORMAL;
}

static void rebuild_characters(void)
{
    /* we rebuild the first syllable text if it is the first syllable, or if
     * it is the second when the first has no text.
     * it is a patch for cases like (c4) Al(ab)le(ab) */
    if ((!score->first_syllable && current_character)
            || (current_syllable && !current_syllable->previous_syllable
            && !current_syllable->text && current_character)) {
        /* leave the first syllable text untouched at this time */
        gregorio_go_to_first_character_c(&current_character);

        started_first_word = true;
    } else {
        gregorio_rebuild_characters(&current_character, center_is_determined,
                false);

        if (started_first_word) {
            gregorio_set_first_word(&current_character);
        }
    }
}

/*
 * Function to close a syllable and update the position. 
 */

static void close_syllable(YYLTYPE *loc)
{
    int i;
    gregorio_add_syllable(&current_syllable, number_of_voices, elements,
            first_text_character, first_translation_character, position,
            abovelinestext, translation_type, no_linebreak_area, euouae, loc,
            started_first_word);
    if (!score->first_syllable) {
        /* we rebuild the first syllable if we have to */
        score->first_syllable = current_syllable;
    }
    if (translation_type == TR_WITH_CENTER_END) {
        gregorio_set_translation_center_beginning(current_syllable);
    }
    /* we update the position */
    if (position == WORD_BEGINNING) {
        position = WORD_MIDDLE;
    }
    if (position == WORD_ONE_SYLLABLE || position == WORD_END) {
        position = WORD_BEGINNING;

        if (started_first_word) {
            started_first_word = false;
        }
    }
    center_is_determined = CENTER_NOT_DETERMINED;
    current_character = NULL;
    first_text_character = NULL;
    first_translation_character = NULL;
    translation_type = TR_NORMAL;
    no_linebreak_area = NLBA_NORMAL;
    euouae = EUOUAE_NORMAL;
    abovelinestext = NULL;
    for (i = 0; i < number_of_voices; i++) {
        elements[i] = NULL;
    }
    current_element = NULL;
}

/* a function called when we see a [, basically, all characters are added to
 * the translation pointer instead of the text pointer */
static void start_translation(unsigned char asked_translation_type)
{
    rebuild_characters();
    first_text_character = current_character;
    /* the middle letters of the translation have no sense */
    center_is_determined = CENTER_FULLY_DETERMINED;
    current_character = NULL;
    translation_type = asked_translation_type;
}

static void end_translation(void)
{
    rebuild_characters();
    first_translation_character = current_character;
}

/*
 * gregorio_gabc_add_text is the function called when lex returns a char *. In
 * this function we convert it into grewchar, and then we add the corresponding 
 * gregorio_characters in the list of gregorio_characters. 
 */

static void gregorio_gabc_add_text(char *mbcharacters)
{
    if (current_character) {
        current_character->next_character = gregorio_build_char_list_from_buf(
                mbcharacters);
        current_character->next_character->previous_character =
                current_character;
    } else {
        current_character = gregorio_build_char_list_from_buf(mbcharacters);
    }
    while (current_character && current_character->next_character) {
        current_character = current_character->next_character;
    }
    free(mbcharacters);
}

/*
 * 
 * The two functions called when lex returns a style, we simply add it. All the 
 * complex things will be done by the function after...
 * 
 */

static void gregorio_gabc_add_style(unsigned char style)
{
    gregorio_begin_style(&current_character, style);
}

static void gregorio_gabc_end_style(unsigned char style)
{
    gregorio_end_style(&current_character, style);
}

void gabc_digest(const void *const buf, const size_t size)
{
    sha1_process_bytes(buf, size, &digester);
}

static void determine_oriscus_orientation(gregorio_score *score) {
    gregorio_syllable *syllable;
    gregorio_element *element;
    gregorio_glyph *glyph;
    gregorio_note *note;
    gregorio_note *oriscus = NULL;

    for (syllable = score->first_syllable; syllable;
            syllable = syllable->next_syllable) {
        for (element = syllable->elements[0]; element;
                element = element->next) {
            if (element->type == GRE_ELEMENT) {
                for (glyph = element->u.first_glyph; glyph;
                        glyph = glyph->next) {
                    if (glyph->type == GRE_GLYPH) {
                        for (note = glyph->u.notes.first_note; note;
                                note = note->next) {
                            if (note->type == GRE_NOTE) {
                                if (oriscus) {
                                    if (note->u.note.pitch
                                            < oriscus->u.note.pitch) {
                                        switch(oriscus->u.note.shape) {
                                        case S_ORISCUS_UNDETERMINED:
                                            oriscus->u.note.shape =
                                                    S_ORISCUS_DESCENDENS;
                                            break;
                                        case S_ORISCUS_CAVUM_UNDETERMINED:
                                            oriscus->u.note.shape =
                                                    S_ORISCUS_CAVUM_DESCENDENS;
                                            break;
                                        default:
                                            gregorio_message(_("bad shape"),
                                                    "determine_oriscus_orientation",
                                                    VERBOSITY_ERROR, 0);
                                            break;
                                        }
                                    } else { /* ascending or the same */
                                        switch(oriscus->u.note.shape) {
                                        case S_ORISCUS_UNDETERMINED:
                                            oriscus->u.note.shape =
                                                    S_ORISCUS_ASCENDENS;
                                            break;
                                        case S_ORISCUS_CAVUM_UNDETERMINED:
                                            oriscus->u.note.shape =
                                                    S_ORISCUS_CAVUM_ASCENDENS;
                                            break;
                                        default:
                                            gregorio_message(_("bad shape"),
                                                    "determine_oriscus_orientation",
                                                    VERBOSITY_ERROR, 0);
                                            break;
                                        }
                                    }
                                    oriscus = NULL;
                                }

                                switch (note->u.note.shape) {
                                case S_ORISCUS_UNDETERMINED:
                                case S_ORISCUS_CAVUM_UNDETERMINED:
                                    oriscus = note;
                                    break;

                                default:
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (oriscus) {
        /* oriscus at the end of the score */
        switch(oriscus->u.note.shape) {
        case S_ORISCUS_UNDETERMINED:
            oriscus->u.note.shape = S_ORISCUS_DESCENDENS;
            break;
        case S_ORISCUS_CAVUM_UNDETERMINED:
            oriscus->u.note.shape = S_ORISCUS_CAVUM_DESCENDENS;
            break;
        default:
            gregorio_message(_("bad shape"), "determine_oriscus_orientation",
                    VERBOSITY_ERROR, 0);
            break;
        }
    }
}

/*
 * The "main" function. It is the function that is called when we have to read
 * a gabc file. It takes a file descriptor, that is to say a file that is
 * aleady open. It returns a valid gregorio_score 
 */

gregorio_score *gabc_read_score(FILE *f_in)
{
    /* compute the SHA-1 digest while parsing, for I/O efficiency */
    sha1_init_ctx(&digester);
    /* digest GREGORIO_VERSION to get a different value when the version
    changes */
    sha1_process_bytes(GREGORIO_VERSION, strlen(GREGORIO_VERSION), &digester);
    /* the input file that flex will parse */
    gabc_score_determination_in = f_in;
    if (!f_in) {
        gregorio_message(_("can't read stream from argument, returning NULL "
                "pointer"), "det_score", VERBOSITY_ERROR, 0);
        return NULL;
    }
    initialize_variables();
    /* the flex/bison main call, it will build the score (that we have
     * initialized) */
    gabc_score_determination_parse();
    if (!score->legacy_oriscus_orientation) {
        determine_oriscus_orientation(score);
    }
    gregorio_fix_initial_keys(score, gregorio_default_clef);
    fix_custos(score);
    free_variables();
    /* the we check the validity and integrity of the score we have built. */
    if (!check_score_integrity(score)) {
        gregorio_free_score(score);
        score = NULL;
        gregorio_message(_("unable to determine a valid score from file"),
                "det_score", VERBOSITY_FATAL, 0);
    }
    sha1_finish_ctx(&digester, score->digest);
    return score;
}

unsigned char nabc_state = 0;
size_t nabc_lines = 0;

static void gabc_y_add_notes(char *notes, YYLTYPE loc) {
    gregorio_element *new_elements;
    gregorio_element *last_element;
    if (nabc_state == 0) {
        if (!elements[voice]) {
            elements[voice] = gabc_det_elements_from_string(notes,
                    &current_key, macros, &loc, score);
            current_element = elements[voice];
        } else {
            new_elements = gabc_det_elements_from_string(notes,
                    &current_key, macros, &loc, score);
            last_element = elements[voice];
            while(last_element->next) {
                last_element = last_element->next;
            }
            last_element->next = new_elements;
            new_elements->previous = last_element;
            current_element = new_elements;
        }
    } else {
        if (!elements[voice]) {
            gregorio_add_element(&elements[voice], NULL);
            current_element = elements[voice];
        }
        if (!current_element) {
            gregorio_message(_("current_element is null, this shouldn't "
                    "happen!"), "gabc_y_add_notes", VERBOSITY_FATAL, 0);
        }
        if (!current_element->nabc) {
            current_element->nabc = (char **) gregorio_calloc (nabc_lines,
                    sizeof (char *));
        }
        current_element->nabc[nabc_state-1] = gregorio_strdup(notes);
        current_element->nabc_lines = nabc_state;
    }
}

#line 775 "gabc/gabc-score-determination-y.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "gabc-score-determination-y.h".  */
#ifndef YY_GABC_SCORE_DETERMINATION_GABC_GABC_SCORE_DETERMINATION_Y_H_INCLUDED
# define YY_GABC_SCORE_DETERMINATION_GABC_GABC_SCORE_DETERMINATION_Y_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int gabc_score_determination_debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NAME = 258,
    AUTHOR = 259,
    GABC_COPYRIGHT = 260,
    SCORE_COPYRIGHT = 261,
    NUMBER_OF_VOICES = 262,
    LANGUAGE = 263,
    STAFF_LINES = 264,
    ORISCUS_ORIENTATION = 265,
    DEF_MACRO = 266,
    OTHER_HEADER = 267,
    ANNOTATION = 268,
    MODE = 269,
    MODE_MODIFIER = 270,
    MODE_DIFFERENTIA = 271,
    INITIAL_STYLE = 272,
    VOICE_CUT = 273,
    VOICE_CHANGE = 274,
    END_OF_DEFINITIONS = 275,
    END_OF_FILE = 276,
    COLON = 277,
    SEMICOLON = 278,
    SPACE = 279,
    CHARACTERS = 280,
    NOTES = 281,
    HYPHEN = 282,
    ATTRIBUTE = 283,
    OPENING_BRACKET = 284,
    CLOSING_BRACKET = 285,
    CLOSING_BRACKET_WITH_SPACE = 286,
    I_BEGINNING = 287,
    I_END = 288,
    TT_BEGINNING = 289,
    TT_END = 290,
    UL_BEGINNING = 291,
    UL_END = 292,
    C_BEGINNING = 293,
    C_END = 294,
    B_BEGINNING = 295,
    B_END = 296,
    SC_BEGINNING = 297,
    SC_END = 298,
    SP_BEGINNING = 299,
    SP_END = 300,
    VERB_BEGINNING = 301,
    VERB = 302,
    VERB_END = 303,
    CENTER_BEGINNING = 304,
    CENTER_END = 305,
    TRANSLATION_BEGINNING = 306,
    TRANSLATION_END = 307,
    TRANSLATION_CENTER_END = 308,
    ALT_BEGIN = 309,
    ALT_END = 310,
    BNLBA = 311,
    ENLBA = 312,
    EUOUAE_B = 313,
    EUOUAE_E = 314,
    NABC_CUT = 315,
    NABC_LINES = 316
  };
#endif
/* Tokens.  */
#define NAME 258
#define AUTHOR 259
#define GABC_COPYRIGHT 260
#define SCORE_COPYRIGHT 261
#define NUMBER_OF_VOICES 262
#define LANGUAGE 263
#define STAFF_LINES 264
#define ORISCUS_ORIENTATION 265
#define DEF_MACRO 266
#define OTHER_HEADER 267
#define ANNOTATION 268
#define MODE 269
#define MODE_MODIFIER 270
#define MODE_DIFFERENTIA 271
#define INITIAL_STYLE 272
#define VOICE_CUT 273
#define VOICE_CHANGE 274
#define END_OF_DEFINITIONS 275
#define END_OF_FILE 276
#define COLON 277
#define SEMICOLON 278
#define SPACE 279
#define CHARACTERS 280
#define NOTES 281
#define HYPHEN 282
#define ATTRIBUTE 283
#define OPENING_BRACKET 284
#define CLOSING_BRACKET 285
#define CLOSING_BRACKET_WITH_SPACE 286
#define I_BEGINNING 287
#define I_END 288
#define TT_BEGINNING 289
#define TT_END 290
#define UL_BEGINNING 291
#define UL_END 292
#define C_BEGINNING 293
#define C_END 294
#define B_BEGINNING 295
#define B_END 296
#define SC_BEGINNING 297
#define SC_END 298
#define SP_BEGINNING 299
#define SP_END 300
#define VERB_BEGINNING 301
#define VERB 302
#define VERB_END 303
#define CENTER_BEGINNING 304
#define CENTER_END 305
#define TRANSLATION_BEGINNING 306
#define TRANSLATION_END 307
#define TRANSLATION_CENTER_END 308
#define ALT_BEGIN 309
#define ALT_END 310
#define BNLBA 311
#define ENLBA 312
#define EUOUAE_B 313
#define EUOUAE_E 314
#define NABC_CUT 315
#define NABC_LINES 316

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE gabc_score_determination_lval;
extern YYLTYPE gabc_score_determination_lloc;
int gabc_score_determination_parse (void);

#endif /* !YY_GABC_SCORE_DETERMINATION_GABC_GABC_SCORE_DETERMINATION_Y_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 962 "gabc/gabc-score-determination-y.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   175

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  82
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  117

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   316

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   736,   736,   740,   745,   746,   750,   754,   760,   770,
     775,   784,   790,   795,   800,   805,   810,   815,   823,   831,
     841,   850,   855,   859,   862,   867,   868,   872,   894,   917,
     930,   943,   948,   957,   958,   959,   963,   966,   969,   972,
     975,   978,   981,   984,   987,   996,   999,  1002,  1005,  1008,
    1011,  1014,  1017,  1020,  1029,  1032,  1038,  1041,  1047,  1048,
    1051,  1052,  1053,  1054,  1058,  1061,  1066,  1067,  1068,  1072,
    1078,  1081,  1087,  1093,  1098,  1106,  1109,  1118,  1121,  1127,
    1128,  1131,  1132
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NAME", "AUTHOR", "GABC_COPYRIGHT",
  "SCORE_COPYRIGHT", "NUMBER_OF_VOICES", "LANGUAGE", "STAFF_LINES",
  "ORISCUS_ORIENTATION", "DEF_MACRO", "OTHER_HEADER", "ANNOTATION", "MODE",
  "MODE_MODIFIER", "MODE_DIFFERENTIA", "INITIAL_STYLE", "VOICE_CUT",
  "VOICE_CHANGE", "END_OF_DEFINITIONS", "END_OF_FILE", "COLON",
  "SEMICOLON", "SPACE", "CHARACTERS", "NOTES", "HYPHEN", "ATTRIBUTE",
  "OPENING_BRACKET", "CLOSING_BRACKET", "CLOSING_BRACKET_WITH_SPACE",
  "I_BEGINNING", "I_END", "TT_BEGINNING", "TT_END", "UL_BEGINNING",
  "UL_END", "C_BEGINNING", "C_END", "B_BEGINNING", "B_END", "SC_BEGINNING",
  "SC_END", "SP_BEGINNING", "SP_END", "VERB_BEGINNING", "VERB", "VERB_END",
  "CENTER_BEGINNING", "CENTER_END", "TRANSLATION_BEGINNING",
  "TRANSLATION_END", "TRANSLATION_CENTER_END", "ALT_BEGIN", "ALT_END",
  "BNLBA", "ENLBA", "EUOUAE_B", "EUOUAE_E", "NABC_CUT", "NABC_LINES",
  "$accept", "score", "all_definitions", "definitions", "attribute",
  "definition", "notes", "note", "closing_bracket_with_space",
  "style_beginning", "style_end", "euouae", "linebreak_area", "character",
  "text_hyphen", "text", "translation_beginning", "translation",
  "above_line_text", "syllable_with_notes", "notes_without_word",
  "syllable", "syllables", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316
};
# endif

#define YYPACT_NINF -55

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-55)))

#define YYTABLE_NINF -3

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -55,    26,   -55,    -1,   -55,     1,     5,     5,     5,     5,
       5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
       5,   -55,   -55,     5,   -55,   -55,   -55,   -55,    36,   -55,
       2,   -55,   -55,   -55,     0,   -55,   -55,   -55,   -55,   -55,
     -55,   -55,   -55,   -55,   -55,   -55,   -55,   -55,   -55,   -55,
     -55,    -6,   -55,    -8,   -55,   -55,   -55,   -55,   -55,   -55,
     -55,   -55,   -55,   -55,   -55,   -55,   -55,   -55,   -55,   -55,
     -55,   -55,   -55,     7,   -55,   -55,   -55,   -55,   -55,   -55,
     -55,   -55,   -55,    99,     8,   -55,    71,   -55,   -55,     6,
       4,    15,    17,   -55,   -55,   -55,    10,    -6,   -15,   -55,
     -55,   -55,   -55,   -55,    -6,   -55,   -55,    15,   -55,   -55,
     -55,   -55,    -6,   -55,   -55,    -6,    -6
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,    81,     0,     1,    66,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    24,     3,     0,     5,    25,    69,    71,     0,    66,
       0,    79,    80,    82,     0,    10,    21,    12,    13,     8,
      11,    17,    22,     9,    23,    20,    14,    15,    16,    19,
      18,    77,    59,    64,    25,    36,    45,    37,    46,    38,
      47,    39,    48,    40,    49,    41,    50,    43,    52,    42,
      51,    44,    53,     0,    56,    57,    54,    55,    60,    61,
      63,    62,    67,     0,     0,    58,     0,    25,     7,     0,
       0,    31,    33,    26,    32,    25,     0,    73,     0,    65,
      68,    25,    64,    70,    78,     6,    29,    27,    30,    28,
      35,    34,    74,    25,    72,    75,    76
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -55,   -55,   -55,   -55,   152,   -55,   -54,   -55,   -46,   -55,
     -55,   -55,   -55,   -37,   -55,    19,   -55,   -11,   -55,   -55,
     -55,   -55,   -55
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,     3,    35,    24,    51,    93,    94,    78,
      79,    80,    81,    82,    83,    28,    29,    30,    85,    31,
      32,    33,     5
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      97,    -2,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    84,    21,    22,
      90,    95,   106,    88,    91,    92,     4,    34,    89,   105,
      25,    87,    98,   104,   107,    92,   110,   101,   111,   113,
     114,   112,    96,    26,   109,    27,   100,   115,    86,     0,
       0,     0,    26,     0,    27,     0,     0,     0,     0,   116,
      23,    52,     0,    53,   108,    54,     0,     0,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,     0,    70,    71,    72,    26,     0,    27,
      73,     0,    74,    75,    76,    77,    52,     0,   102,     0,
       0,     0,     0,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,     0,    70,
      71,    72,     0,   103,    52,    73,    99,    74,    75,    76,
      77,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,     0,    70,    71,    72,
       0,     0,     0,    73,     0,    74,    75,    76,    77,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,     0,     0,    50
};

static const yytype_int8 yycheck[] =
{
      54,     0,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    28,    19,    20,
      26,    29,    18,    23,    30,    31,     0,    22,    28,    23,
      29,    29,    25,    87,    30,    31,    21,    29,    21,    29,
      55,    95,    53,    51,    90,    53,    83,   101,    29,    -1,
      -1,    -1,    51,    -1,    53,    -1,    -1,    -1,    -1,   113,
      61,    25,    -1,    27,    60,    29,    -1,    -1,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    -1,    48,    49,    50,    51,    -1,    53,
      54,    -1,    56,    57,    58,    59,    25,    -1,    27,    -1,
      -1,    -1,    -1,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    -1,    48,
      49,    50,    -1,    52,    25,    54,    27,    56,    57,    58,
      59,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    -1,    48,    49,    50,
      -1,    -1,    -1,    54,    -1,    56,    57,    58,    59,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    -1,    -1,    23
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    63,    64,    65,     0,    84,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    19,    20,    61,    67,    29,    51,    53,    77,    78,
      79,    81,    82,    83,    22,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    68,    25,    27,    29,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      48,    49,    50,    54,    56,    57,    58,    59,    71,    72,
      73,    74,    75,    76,    79,    80,    77,    29,    23,    28,
      26,    30,    31,    69,    70,    29,    79,    68,    25,    27,
      75,    29,    27,    52,    68,    23,    18,    30,    60,    70,
      21,    21,    68,    29,    55,    68,    68
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    62,    63,    64,    65,    65,    66,    66,    67,    67,
      67,    67,    67,    67,    67,    67,    67,    67,    67,    67,
      67,    67,    67,    67,    67,    68,    68,    69,    69,    69,
      69,    69,    69,    70,    70,    70,    71,    71,    71,    71,
      71,    71,    71,    71,    71,    72,    72,    72,    72,    72,
      72,    72,    72,    72,    73,    73,    74,    74,    75,    75,
      75,    75,    75,    75,    76,    76,    77,    77,    77,    78,
      79,    79,    80,    81,    81,    81,    81,    82,    82,    83,
      83,    84,    84
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     0,     2,     3,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     0,     2,     2,     2,     2,
       2,     1,     1,     1,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     0,     2,     3,     1,
       3,     1,     3,     3,     4,     4,     5,     2,     3,     1,
       1,     0,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

/* User initialization code.  */
#line 701 "gabc/gabc-score-determination.y" /* yacc.c:1429  */
{
    yylloc.first_line = 1;
    yylloc.first_column = 0;
    yylloc.first_offset = 0;
    yylloc.last_line = 1;
    yylloc.last_column = 0;
    yylloc.last_offset = 0;
}

#line 2070 "gabc/gabc-score-determination-y.c" /* yacc.c:1429  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 740 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        end_definitions();
    }
#line 2261 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 6:
#line 750 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        (yyval).text = (yyvsp[-1]).text;
    }
#line 2269 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 7:
#line 754 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        (yyval).text = NULL;
    }
#line 2277 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 8:
#line 760 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
        number_of_voices=atoi((yyvsp[0]).text);
        if (number_of_voices > MAX_NUMBER_OF_VOICES) {
            gregorio_messagef("det_score", VERBOSITY_WARNING, 0,
                    _("can't define %d voices, maximum is %d"),
                    number_of_voices, MAX_NUMBER_OF_VOICES);
        }
        score->number_of_voices = number_of_voices;
    }
#line 2292 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 9:
#line 770 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        /* these definitions are not passed through */
        free(macros[(yyvsp[-1]).character - '0']);
        macros[(yyvsp[-1]).character - '0'] = (yyvsp[0]).text;
    }
#line 2302 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 10:
#line 775 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if ((yyvsp[0]).text == NULL) {
            gregorio_message("name can't be empty","det_score",
                    VERBOSITY_WARNING, 0);
        }
        check_multiple("name", score->name != NULL);
        gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
        score->name = (yyvsp[0]).text;
    }
#line 2316 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 11:
#line 784 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("language", got_language);
        gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
        gregorio_set_centering_language((yyvsp[0]).text);
        got_language = true;
    }
#line 2327 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 12:
#line 790 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("gabc-copyright", score->gabc_copyright != NULL);
        gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
        score->gabc_copyright = (yyvsp[0]).text;
    }
#line 2337 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 13:
#line 795 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("score_copyright", score->score_copyright != NULL);
        gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
        score->score_copyright = (yyvsp[0]).text;
    }
#line 2347 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 14:
#line 800 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("mode", score->mode != 0);
        gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
        score->mode = (yyvsp[0]).text;
    }
#line 2357 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 15:
#line 805 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("mode-modifier", score->mode_modifier != NULL);
        gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
        score->mode_modifier = (yyvsp[0]).text;
    }
#line 2367 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 16:
#line 810 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("mode-differentia", score->mode_differentia != NULL);
        gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
        score->mode_differentia = (yyvsp[0]).text;
    }
#line 2377 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 17:
#line 815 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("staff-lines", got_staff_lines);
        if ((yyvsp[0]).text) {
            gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
            gregorio_set_score_staff_lines(score, atoi((yyvsp[0]).text));
            got_staff_lines = true;
        }
    }
#line 2390 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 18:
#line 823 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("nabc lines", score->nabc_lines != 0);
        if ((yyvsp[0]).text) {
            gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
            nabc_lines=atoi((yyvsp[0]).text);
            score->nabc_lines=nabc_lines;
        }
    }
#line 2403 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 19:
#line 831 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if ((yyvsp[0]).text) {
            /* DEPRECATED by 4.1 */
            gregorio_message("\"initial-style\" header is deprecated. Please "
            "use \\gresetinitiallines in TeX instead.",
            "gabc_score_determination_parse", VERBOSITY_DEPRECATION, 0);
            score->initial_style = atoi((yyvsp[0]).text);
            free((yyvsp[0]).text);
        }
    }
#line 2418 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 20:
#line 841 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if (score->annotation [MAX_ANNOTATIONS - 1]) {
            gregorio_messagef("det_score", VERBOSITY_WARNING, 0,
                    _("too many definitions of annotation found, only the "
                    "first %d will be taken"), MAX_ANNOTATIONS);
        }
        gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
        gregorio_set_score_annotation(score, (yyvsp[0]).text);
    }
#line 2432 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 21:
#line 850 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("author", score->author != NULL);
        gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
        score->author = (yyvsp[0]).text;
    }
#line 2442 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 22:
#line 855 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
        score->legacy_oriscus_orientation = (strcmp((yyvsp[0]).text, "legacy") == 0);
    }
#line 2451 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 23:
#line 859 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_add_score_header(score, (yyvsp[-1]).text, (yyvsp[0]).text);
    }
#line 2459 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 24:
#line 862 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        next_voice_info();
    }
#line 2467 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 27:
#line 872 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if (voice<number_of_voices) {
            gabc_y_add_notes((yyvsp[-1]).text, (yylsp[-1]));
            free((yyvsp[-1]).text);
        }
        else {
            gregorio_messagef("det_score", VERBOSITY_ERROR, 0,
                    ngt_("too many voices in note : %d found, %d expected",
                    "too many voices in note : %d found, %d expected",
                    number_of_voices), voice+1, number_of_voices);
        }
        if (voice<number_of_voices-1) {
            gregorio_messagef("det_score", VERBOSITY_INFO, 0,
                    ngt_("not enough voices in note : %d found, %d expected, "
                    "completing with empty neume", "not enough voices in note "
                    ": %d found, %d expected, completing with empty neume",
                    voice+1), voice+1, number_of_voices);
            complete_with_nulls(voice);
        }
        voice=0;
        nabc_state=0;
    }
#line 2494 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 28:
#line 894 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if (voice<number_of_voices) {
            gabc_y_add_notes((yyvsp[-1]).text, (yylsp[-1]));
            free((yyvsp[-1]).text);
        }
        else {
            gregorio_messagef("det_score", VERBOSITY_ERROR, 0,
                    ngt_("too many voices in note : %d found, %d expected",
                    "too many voices in note : %d found, %d expected",
                    number_of_voices), voice+1, number_of_voices);
        }
        if (voice<number_of_voices-1) {
            gregorio_messagef("det_score", VERBOSITY_INFO, 0,
                    ngt_("not enough voices in note : %d found, %d expected, "
                    "completing with empty neume", "not enough voices in note "
                    ": %d found, %d expected, completing with empty neume",
                    voice+1), voice+1, number_of_voices);
            complete_with_nulls(voice);
        }
        voice=0;
        nabc_state=0;
        update_position_with_space();
    }
#line 2522 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 29:
#line 917 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if (voice<number_of_voices) {
            gabc_y_add_notes((yyvsp[-1]).text, (yylsp[-1]));
            free((yyvsp[-1]).text);
            voice++;
        }
        else {
            gregorio_messagef("det_score", VERBOSITY_ERROR, 0,
                    ngt_("too many voices in note : %d found, %d expected",
                    "too many voices in note : %d found, %d expected",
                    number_of_voices), voice+1, number_of_voices);
        }
    }
#line 2540 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 30:
#line 930 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if (!nabc_lines) {
            gregorio_message(_("You used character \"|\" in gabc without "
                               "setting \"nabc-lines\" parameter. Please "
                               "set it in your gabc header."),
                             "det_score", VERBOSITY_FATAL, 0);
        }
        if (voice<number_of_voices) {
            gabc_y_add_notes((yyvsp[-1]).text, (yylsp[-1]));
            free((yyvsp[-1]).text);
            nabc_state = (nabc_state + 1) % (nabc_lines+1);
        }
    }
#line 2558 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 31:
#line 943 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        elements[voice]=NULL;
        voice=0;
        nabc_state=0;
    }
#line 2568 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 32:
#line 948 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        elements[voice]=NULL;
        voice=0;
        nabc_state=0;
        update_position_with_space();
    }
#line 2579 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 36:
#line 963 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_ITALIC);
    }
#line 2587 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 37:
#line 966 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_TT);
    }
#line 2595 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 38:
#line 969 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_UNDERLINED);
    }
#line 2603 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 39:
#line 972 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_COLORED);
    }
#line 2611 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 40:
#line 975 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_BOLD);
    }
#line 2619 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 41:
#line 978 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_SMALL_CAPS);
    }
#line 2627 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 42:
#line 981 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_VERBATIM);
    }
#line 2635 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 43:
#line 984 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_SPECIAL_CHAR);
    }
#line 2643 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 44:
#line 987 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if (!center_is_determined) {
            gregorio_gabc_add_style(ST_FORCED_CENTER);
            center_is_determined=CENTER_HALF_DETERMINED;
        }
    }
#line 2654 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 45:
#line 996 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_ITALIC);
    }
#line 2662 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 46:
#line 999 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_TT);
    }
#line 2670 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 47:
#line 1002 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_UNDERLINED);
    }
#line 2678 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 48:
#line 1005 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_COLORED);
    }
#line 2686 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 49:
#line 1008 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_BOLD);
    }
#line 2694 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 50:
#line 1011 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_SMALL_CAPS);
    }
#line 2702 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 51:
#line 1014 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_VERBATIM);
    }
#line 2710 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 52:
#line 1017 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_SPECIAL_CHAR);
    }
#line 2718 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 53:
#line 1020 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if (center_is_determined==CENTER_HALF_DETERMINED) {
            gregorio_gabc_end_style(ST_FORCED_CENTER);
            center_is_determined=CENTER_FULLY_DETERMINED;
        }
    }
#line 2729 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 54:
#line 1029 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        euouae = EUOUAE_BEGINNING;
    }
#line 2737 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 55:
#line 1032 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        euouae = EUOUAE_END;
    }
#line 2745 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 56:
#line 1038 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        no_linebreak_area = NLBA_BEGINNING;
    }
#line 2753 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 57:
#line 1041 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        no_linebreak_area = NLBA_END;
    }
#line 2761 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 59:
#line 1048 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_text((yyvsp[0]).text);
    }
#line 2769 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 64:
#line 1058 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_text(gregorio_strdup("-"));
    }
#line 2777 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 65:
#line 1061 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_text(gregorio_strdup("-"));
    }
#line 2785 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 69:
#line 1072 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        start_translation(TR_NORMAL);
    }
#line 2793 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 70:
#line 1078 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        end_translation();
    }
#line 2801 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 71:
#line 1081 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        start_translation(TR_WITH_CENTER_END);
    }
#line 2809 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 72:
#line 1087 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        abovelinestext = (yyvsp[-1]).text;
    }
#line 2817 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 73:
#line 1093 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        rebuild_characters();
        first_text_character = current_character;
        close_syllable(&(yylsp[-2]));
    }
#line 2827 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 74:
#line 1098 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_VERBATIM);
        gregorio_gabc_add_text(gregorio_strdup("\\GreForceHyphen"));
        gregorio_gabc_end_style(ST_VERBATIM);
        rebuild_characters();
        first_text_character = current_character;
        close_syllable(&(yylsp[-3]));
    }
#line 2840 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 75:
#line 1106 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        close_syllable(&(yylsp[-3]));
    }
#line 2848 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 76:
#line 1109 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_VERBATIM);
        gregorio_gabc_add_text(gregorio_strdup("\\GreForceHyphen"));
        gregorio_gabc_end_style(ST_VERBATIM);
        close_syllable(&(yylsp[-4]));
    }
#line 2859 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 77:
#line 1118 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        close_syllable(NULL);
    }
#line 2867 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 78:
#line 1121 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        close_syllable(NULL);
    }
#line 2875 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;


#line 2879 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
