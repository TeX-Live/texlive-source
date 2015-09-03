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
static int current_key = DEFAULT_KEY;
static bool got_language = false;
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

static void gabc_fix_custos(gregorio_score *score_to_check)
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
    current_key = score_to_check->first_voice_info->initial_key;
    current_syllable = score_to_check->first_syllable;
    while (current_syllable) {
        current_element = (current_syllable->elements)[0];
        while (current_element) {
            if (current_element->type == GRE_CUSTOS) {
                custo_element = current_element;
                pitch = custo_element->u.misc.pitched.pitch;
                /* we look for the key */
                while (current_element) {
                    switch (current_element->type) {
                    case GRE_C_KEY_CHANGE:
                    case GRE_C_KEY_CHANGE_FLATED:
                        pitch = gregorio_determine_next_pitch(current_syllable,
                                current_element, NULL);
                        newkey = gregorio_calculate_new_key(C_KEY,
                                current_element->u.misc.pitched.pitch - '0');
                        pitch_difference = (char) newkey - (char) current_key;
                        pitch -= pitch_difference;
                        current_key = newkey;
                        break;
                    case GRE_F_KEY_CHANGE:
                    case GRE_F_KEY_CHANGE_FLATED:
                        pitch = gregorio_determine_next_pitch(current_syllable,
                                current_element, NULL);
                        newkey = gregorio_calculate_new_key(F_KEY,
                                current_element->u.misc.pitched.pitch - '0');
                        pitch_difference = (char) newkey - (char) current_key;
                        pitch -= pitch_difference;
                        current_key = newkey;
                        break;
                    default:
                        break;
                    }
                    if (!custo_element->u.misc.pitched.force_pitch) {
                        while (pitch < LOWEST_PITCH) {
                            pitch += 7;
                        }
                        while (pitch > HIGHEST_PITCH) {
                            pitch -= 7;
                        }
                        custo_element->u.misc.pitched.pitch = pitch;
                    }
                    assert(custo_element->u.misc.pitched.pitch >= LOWEST_PITCH 
                            && custo_element->u.misc.pitched.pitch
                            <= HIGHEST_PITCH);
                    current_element = current_element->next;
                }
            }
            if (current_element) {
                if (current_element->type == GRE_C_KEY_CHANGE
                    || current_element->type == GRE_C_KEY_CHANGE_FLATED) {
                    current_key = gregorio_calculate_new_key(C_KEY,
                            current_element->u.misc.pitched.pitch - '0');
                }
                if (current_element->type == GRE_F_KEY_CHANGE
                    || current_element->type == GRE_F_KEY_CHANGE_FLATED) {
                    current_key = gregorio_calculate_new_key(F_KEY,
                            current_element->u.misc.pitched.pitch - '0');
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
    center_is_determined = 0;
    for (i = 0; i < 10; i++) {
        macros[i] = NULL;
    }
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
    return (voice_info->initial_key != 5 || voice_info->style
            || voice_info->virgula_position);
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
    elements = (gregorio_element **) malloc(number_of_voices *
            sizeof(gregorio_element *));
    for (i = 0; i < number_of_voices; i++) {
        elements[i] = NULL;
    }

    if (!got_language) {
        static char lang_Latin[] = "Latin";
        gregorio_set_centering_language(lang_Latin);
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

static void rebuild_characters(gregorio_character **param_character,
        gregorio_center_determination center_is_determined)
{
    bool has_initial = score->initial_style != NO_INITIAL;
    /* we rebuild the first syllable text if it is the first syllable, or if
     * it is the second when the first has no text.
     * it is a patch for cases like (c4) Al(ab)le(ab) */
    if ((!score->first_syllable && has_initial && current_character)
            || (current_syllable && !current_syllable->previous_syllable
            && !current_syllable->text && current_character)) {
        gregorio_rebuild_first_syllable(&current_character, has_initial);
    }

    gregorio_rebuild_characters(param_character, center_is_determined,
            has_initial);
}

/*
 * Function to close a syllable and update the position. 
 */

static void close_syllable(YYLTYPE *loc)
{
    int i;
    gregorio_add_syllable(&current_syllable, number_of_voices, elements,
            first_text_character, first_translation_character, position,
            abovelinestext, translation_type, no_linebreak_area, euouae, loc);
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
    rebuild_characters(&current_character, center_is_determined);
    first_text_character = current_character;
    /* the middle letters of the translation have no sense */
    center_is_determined = CENTER_FULLY_DETERMINED;
    current_character = NULL;
    translation_type = asked_translation_type;
}

static void end_translation(void)
{
    rebuild_characters(&current_character, center_is_determined);
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
 * the function called when centering_scheme is seen in gabc 
 */
static void set_centering_scheme(char *sc)
{
    gregorio_message("\"centering-scheme\" header is deprecated. Please use "
            "\\setlyriccentering in TeX instead.", "set_centering_scheme",
            VERBOSITY_DEPRECATION, 0);
    if (strncmp((const char *) sc, "latine", 6) == 0) {
        score->centering = SCHEME_VOWEL;
        return;
    }
    if (strncmp((const char *) sc, "english", 6) == 0) {
        score->centering = SCHEME_SYLLABLE;
        return;
    }
    gregorio_message("centering-scheme unknown value: must be \"latine\" "
            "or \"english\"", "set_centering_scheme", VERBOSITY_WARNING, 0);
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
    gregorio_fix_initial_keys(score, DEFAULT_KEY);
    gabc_fix_custos(score);
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
                    &current_key, macros, &loc);
            current_element = elements[voice];
        } else {
            new_elements = gabc_det_elements_from_string(notes,
                    &current_key, macros, &loc);
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
            current_element->nabc = (char **) calloc (nabc_lines,
                    sizeof (char *));
        }
        current_element->nabc[nabc_state-1] = strdup(notes);
        current_element->nabc_lines = nabc_state;
    }
}

#line 702 "gabc/gabc-score-determination-y.c" /* yacc.c:339  */

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
    ATTRIBUTE = 258,
    COLON = 259,
    SEMICOLON = 260,
    OFFICE_PART = 261,
    ANNOTATION = 262,
    AUTHOR = 263,
    DATE = 264,
    MANUSCRIPT = 265,
    MANUSCRIPT_REFERENCE = 266,
    MANUSCRIPT_STORAGE_PLACE = 267,
    TRANSCRIBER = 268,
    TRANSCRIPTION_DATE = 269,
    BOOK = 270,
    STYLE = 271,
    VIRGULA_POSITION = 272,
    INITIAL_STYLE = 273,
    MODE = 274,
    GREGORIOTEX_FONT = 275,
    GENERATED_BY = 276,
    NAME = 277,
    OPENING_BRACKET = 278,
    NOTES = 279,
    VOICE_CUT = 280,
    CLOSING_BRACKET = 281,
    NUMBER_OF_VOICES = 282,
    VOICE_CHANGE = 283,
    END_OF_DEFINITIONS = 284,
    SPACE = 285,
    CHARACTERS = 286,
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
    CLOSING_BRACKET_WITH_SPACE = 306,
    TRANSLATION_BEGINNING = 307,
    TRANSLATION_END = 308,
    GABC_COPYRIGHT = 309,
    SCORE_COPYRIGHT = 310,
    OCCASION = 311,
    METER = 312,
    COMMENTARY = 313,
    ARRANGER = 314,
    GABC_VERSION = 315,
    USER_NOTES = 316,
    DEF_MACRO = 317,
    ALT_BEGIN = 318,
    ALT_END = 319,
    CENTERING_SCHEME = 320,
    TRANSLATION_CENTER_END = 321,
    BNLBA = 322,
    ENLBA = 323,
    EUOUAE_B = 324,
    EUOUAE_E = 325,
    NABC_CUT = 326,
    NABC_LINES = 327,
    LANGUAGE = 328
  };
#endif
/* Tokens.  */
#define ATTRIBUTE 258
#define COLON 259
#define SEMICOLON 260
#define OFFICE_PART 261
#define ANNOTATION 262
#define AUTHOR 263
#define DATE 264
#define MANUSCRIPT 265
#define MANUSCRIPT_REFERENCE 266
#define MANUSCRIPT_STORAGE_PLACE 267
#define TRANSCRIBER 268
#define TRANSCRIPTION_DATE 269
#define BOOK 270
#define STYLE 271
#define VIRGULA_POSITION 272
#define INITIAL_STYLE 273
#define MODE 274
#define GREGORIOTEX_FONT 275
#define GENERATED_BY 276
#define NAME 277
#define OPENING_BRACKET 278
#define NOTES 279
#define VOICE_CUT 280
#define CLOSING_BRACKET 281
#define NUMBER_OF_VOICES 282
#define VOICE_CHANGE 283
#define END_OF_DEFINITIONS 284
#define SPACE 285
#define CHARACTERS 286
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
#define CLOSING_BRACKET_WITH_SPACE 306
#define TRANSLATION_BEGINNING 307
#define TRANSLATION_END 308
#define GABC_COPYRIGHT 309
#define SCORE_COPYRIGHT 310
#define OCCASION 311
#define METER 312
#define COMMENTARY 313
#define ARRANGER 314
#define GABC_VERSION 315
#define USER_NOTES 316
#define DEF_MACRO 317
#define ALT_BEGIN 318
#define ALT_END 319
#define CENTERING_SCHEME 320
#define TRANSLATION_CENTER_END 321
#define BNLBA 322
#define ENLBA 323
#define EUOUAE_B 324
#define EUOUAE_E 325
#define NABC_CUT 326
#define NABC_LINES 327
#define LANGUAGE 328

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

#line 913 "gabc/gabc-score-determination-y.c" /* yacc.c:358  */

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
#define YYLAST   194

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  118
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  162

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   328

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
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   654,   654,   658,   663,   664,   668,   680,   686,   697,
     703,   712,   719,   726,   736,   743,   750,   757,   764,   771,
     784,   794,   805,   814,   825,   832,   839,   846,   854,   862,
     869,   877,   885,   896,   909,   915,   921,   925,   931,   932,
     933,   934,   935,   936,   937,   938,   939,   940,   941,   942,
     943,   944,   945,   946,   947,   948,   949,   950,   951,   952,
     953,   954,   955,   956,   957,   958,   959,   960,   961,   966,
     967,   971,   993,  1016,  1029,  1042,  1047,  1056,  1059,  1062,
    1065,  1068,  1071,  1074,  1077,  1080,  1089,  1092,  1095,  1098,
    1101,  1104,  1107,  1110,  1113,  1122,  1125,  1131,  1134,  1140,
    1141,  1144,  1145,  1146,  1147,  1150,  1151,  1155,  1161,  1164,
    1170,  1176,  1181,  1187,  1190,  1196,  1197,  1200,  1201
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ATTRIBUTE", "COLON", "SEMICOLON",
  "OFFICE_PART", "ANNOTATION", "AUTHOR", "DATE", "MANUSCRIPT",
  "MANUSCRIPT_REFERENCE", "MANUSCRIPT_STORAGE_PLACE", "TRANSCRIBER",
  "TRANSCRIPTION_DATE", "BOOK", "STYLE", "VIRGULA_POSITION",
  "INITIAL_STYLE", "MODE", "GREGORIOTEX_FONT", "GENERATED_BY", "NAME",
  "OPENING_BRACKET", "NOTES", "VOICE_CUT", "CLOSING_BRACKET",
  "NUMBER_OF_VOICES", "VOICE_CHANGE", "END_OF_DEFINITIONS", "SPACE",
  "CHARACTERS", "I_BEGINNING", "I_END", "TT_BEGINNING", "TT_END",
  "UL_BEGINNING", "UL_END", "C_BEGINNING", "C_END", "B_BEGINNING", "B_END",
  "SC_BEGINNING", "SC_END", "SP_BEGINNING", "SP_END", "VERB_BEGINNING",
  "VERB", "VERB_END", "CENTER_BEGINNING", "CENTER_END",
  "CLOSING_BRACKET_WITH_SPACE", "TRANSLATION_BEGINNING", "TRANSLATION_END",
  "GABC_COPYRIGHT", "SCORE_COPYRIGHT", "OCCASION", "METER", "COMMENTARY",
  "ARRANGER", "GABC_VERSION", "USER_NOTES", "DEF_MACRO", "ALT_BEGIN",
  "ALT_END", "CENTERING_SCHEME", "TRANSLATION_CENTER_END", "BNLBA",
  "ENLBA", "EUOUAE_B", "EUOUAE_E", "NABC_CUT", "NABC_LINES", "LANGUAGE",
  "$accept", "score", "all_definitions", "definitions",
  "number_of_voices_definition", "macro_definition", "name_definition",
  "centering_scheme_definition", "language_definition",
  "gabc_copyright_definition", "score_copyright_definition",
  "gregoriotex_font_definition", "office_part_definition",
  "occasion_definition", "meter_definition", "commentary_definition",
  "arranger_definition", "gabc_version_definition", "mode_definition",
  "nabc_lines_definition", "initial_style_definition",
  "annotation_definition", "author_definition", "date_definition",
  "manuscript_definition", "manuscript_reference_definition",
  "manuscript_storage_place_definition", "book_definition",
  "transcriber_definition", "transcription_date_definition",
  "style_definition", "virgula_position_definition",
  "generated_by_definition", "user_notes_definition", "attribute",
  "definition", "notes", "note", "style_beginning", "style_end", "euouae",
  "linebreak_area", "character", "text", "translation_beginning",
  "translation", "above_line_text", "syllable_with_notes",
  "notes_without_word", "syllable", "syllables", YY_NULLPTR
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
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328
};
# endif

#define YYPACT_NINF -83

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-83)))

#define YYTABLE_NINF -3

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -83,    20,   -83,    -6,   -83,    17,    23,    23,    23,    23,
      23,    23,    23,    23,    23,    23,    23,    23,    23,    23,
      23,    23,    23,    23,   -83,   -83,    23,    23,    23,    23,
      23,    23,    23,    23,    23,    23,    23,    23,   -83,   -83,
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
     -83,   -83,    53,   -83,     2,   -83,   -83,   -83,    21,   -83,
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,     6,
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
      -2,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
       8,   -83,    93,   -83,    28,   -83,    -7,   -83,   -83,   -83,
       6,   -30,   -83,   -83,     6,   -83,   -83,   -83,   -83,   -83,
     -83,     6
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,   117,     0,     1,   105,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    68,     3,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    38,    40,
      39,    66,    67,    41,    42,    64,    56,    57,    58,    59,
      60,    61,    63,    52,    62,    55,    54,    53,    50,    49,
      48,    51,    47,    46,    45,    44,    43,    65,     5,    69,
     107,   109,     0,   105,     0,   115,   116,   118,     0,    14,
      23,    24,    25,    26,    27,    28,    30,    31,    29,    32,
      33,    22,    20,    13,    34,     8,     6,    11,    12,    15,
      16,    17,    18,    19,    35,     7,     9,    21,    10,   113,
      69,   100,    77,    86,    78,    87,    79,    88,    80,    89,
      81,    90,    82,    91,    84,    93,    83,    92,    85,    94,
       0,    97,    98,    95,    96,   101,   102,   104,   103,   106,
       0,    99,     0,    69,     0,    37,     0,    75,    76,    70,
     111,     0,    69,   108,   114,    36,    73,    71,    72,    74,
     110,   112
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
     -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,   -83,
     -83,   -83,   -83,   -83,   157,   -83,   -82,   -83,   -83,   -83,
     -83,   -83,   -83,   -38,   -83,   -36,   -83,   -83,   -83,   -83,
     -83
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     3,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    79,    68,   109,   149,   135,   136,
     137,   138,   139,    72,    73,    74,   141,    75,    76,    77,
       5
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    -2,   156,   157,
       4,    23,    24,    25,   144,   143,   145,    78,   150,   151,
     146,   152,   147,   155,   160,   142,   140,     0,     0,     0,
      69,     0,     0,     0,   158,     0,     0,     0,    26,    27,
      28,    29,    30,    31,    32,    33,    34,   148,     0,    35,
       0,   154,     0,     0,   159,     0,    36,    37,     0,    70,
     161,     0,     0,     0,     0,     0,   110,     0,     0,     0,
       0,     0,     0,    71,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
       0,   127,   128,   129,     0,    70,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   130,     0,     0,    71,
     131,   132,   133,   134,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
       0,   127,   128,   129,     0,     0,   153,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   130,     0,     0,     0,
     131,   132,   133,   134,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,     0,     0,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108
};

static const yytype_int16 yycheck[] =
{
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,     0,    25,    26,
       0,    27,    28,    29,     3,    23,     5,     4,   110,    31,
      24,    23,    26,     5,    64,    73,    72,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    51,    -1,    -1,    -1,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    51,    -1,    65,
      -1,   143,    -1,    -1,    71,    -1,    72,    73,    -1,    52,
     152,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,
      -1,    -1,    -1,    66,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      -1,    48,    49,    50,    -1,    52,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    66,
      67,    68,    69,    70,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      -1,    48,    49,    50,    -1,    -1,    53,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,
      67,    68,    69,    70,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    -1,    -1,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    75,    76,    77,     0,   124,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    27,    28,    29,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    65,    72,    73,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   109,    23,
      52,    66,   117,   118,   119,   121,   122,   123,     4,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   110,
      23,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    48,    49,    50,
      63,    67,    68,    69,    70,   112,   113,   114,   115,   116,
     119,   120,   117,    23,     3,     5,    24,    26,    51,   111,
     110,    31,    23,    53,   110,     5,    25,    26,    51,    71,
      64,   110
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    74,    75,    76,    77,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   108,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   110,
     110,   111,   111,   111,   111,   111,   111,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   114,   114,   115,   115,   116,
     116,   116,   116,   116,   116,   117,   117,   118,   119,   119,
     120,   121,   121,   122,   122,   123,   123,   124,   124
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     0,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     3,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       2,     2,     2,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     2,     1,     3,     1,
       3,     3,     4,     2,     3,     1,     1,     0,     2
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
#line 628 "gabc/gabc-score-determination.y" /* yacc.c:1429  */
{
    yylloc.first_line = 1;
    yylloc.first_column = 0;
    yylloc.first_offset = 0;
    yylloc.last_line = 1;
    yylloc.last_column = 0;
    yylloc.last_offset = 0;
}

#line 2072 "gabc/gabc-score-determination-y.c" /* yacc.c:1429  */
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
#line 658 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        end_definitions();
    }
#line 2263 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 6:
#line 668 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        number_of_voices=atoi((yyvsp[0]).text);
        if (number_of_voices > MAX_NUMBER_OF_VOICES) {
            gregorio_messagef("det_score", VERBOSITY_WARNING, 0,
                    _("can't define %d voices, maximum is %d"),
                    number_of_voices, MAX_NUMBER_OF_VOICES);
        }
        gregorio_set_score_number_of_voices (score, number_of_voices);
    }
#line 2277 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 7:
#line 680 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        macros[(yyvsp[-1]).character - '0'] = (yyvsp[0]).text;
    }
#line 2285 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 8:
#line 686 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if ((yyvsp[0]).text==NULL) {
            gregorio_message("name can't be empty","det_score",
                    VERBOSITY_WARNING, 0);
        }
        check_multiple("name", score->name != NULL);
        gregorio_set_score_name (score, (yyvsp[0]).text);
    }
#line 2298 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 9:
#line 697 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        set_centering_scheme((yyvsp[0]).text);
    }
#line 2306 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 10:
#line 703 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("language", got_language);
        gregorio_set_centering_language((yyvsp[0]).text);
        free((yyvsp[0]).text);
        got_language = true;
    }
#line 2317 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 11:
#line 712 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("gabc-copyright", score->gabc_copyright != NULL);
        gregorio_set_score_gabc_copyright (score, (yyvsp[0]).text);
    }
#line 2326 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 12:
#line 719 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("score_copyright", score->score_copyright != NULL);
        gregorio_set_score_score_copyright (score, (yyvsp[0]).text);
    }
#line 2335 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 13:
#line 726 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_message("\"gregoriotex-font\" header is deprecated. "
        "Please use \\gresetgregoriofont in TeX instead.",
        "set_gregoriotex_font", VERBOSITY_DEPRECATION, 0);
        check_multiple("GregorioTeX font", score->gregoriotex_font != NULL);
        score->gregoriotex_font=(yyvsp[0]).text;
    }
#line 2347 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 14:
#line 736 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("office part", score->office_part != NULL);
        gregorio_set_score_office_part (score, (yyvsp[0]).text);
    }
#line 2356 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 15:
#line 743 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("occasion", score->occasion != NULL);
        gregorio_set_score_occasion (score, (yyvsp[0]).text);
    }
#line 2365 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 16:
#line 750 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("meter", score->meter != NULL);
        gregorio_set_score_meter (score, (yyvsp[0]).text);
    }
#line 2374 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 17:
#line 757 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("commentary", score->commentary != NULL);
        gregorio_set_score_commentary (score, (yyvsp[0]).text);
    }
#line 2383 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 18:
#line 764 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("arranger", score->arranger != NULL);
        gregorio_set_score_arranger (score, (yyvsp[0]).text);
    }
#line 2392 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 19:
#line 771 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        /* So far this handling of the version is rudimentary.  When
         * we start supporting multiple input versions, it will become
         * more complex.  For the moment, just issue a warning. */
        if (strcmp ((yyvsp[0]).text, GABC_CURRENT_VERSION) != 0) {
            gregorio_message(_("gabc-version is not the current one "
                    GABC_CURRENT_VERSION " ; there may be problems"),
                    "det_score", VERBOSITY_WARNING, 0);
        }
    }
#line 2407 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 20:
#line 784 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("mode", score->mode != 0);
        if ((yyvsp[0]).text) {
            score->mode=atoi((yyvsp[0]).text);
            free((yyvsp[0]).text);
        }
    }
#line 2419 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 21:
#line 794 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("nabc lines", score->nabc_lines != 0);
        if ((yyvsp[0]).text) {
            nabc_lines=atoi((yyvsp[0]).text);
            score->nabc_lines=nabc_lines;
            free((yyvsp[0]).text);
        }
    }
#line 2432 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 22:
#line 805 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if ((yyvsp[0]).text) {
            score->initial_style=atoi((yyvsp[0]).text);
            free((yyvsp[0]).text);
        }
    }
#line 2443 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 23:
#line 814 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if (score->annotation [MAX_ANNOTATIONS - 1]) {
            gregorio_messagef("det_score", VERBOSITY_WARNING, 0,
                    _("too many definitions of annotation found, only the "
                    "first %d will be taken"), MAX_ANNOTATIONS);
        }
        gregorio_set_score_annotation (score, (yyvsp[0]).text);
    }
#line 2456 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 24:
#line 825 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("author", score->si.author != NULL);
        gregorio_set_score_author (score, (yyvsp[0]).text);
    }
#line 2465 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 25:
#line 832 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("date", score->si.date != NULL);
        gregorio_set_score_date (score, (yyvsp[0]).text);
    }
#line 2474 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 26:
#line 839 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("manuscript", score->si.manuscript != NULL);
        gregorio_set_score_manuscript (score, (yyvsp[0]).text);
    }
#line 2483 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 27:
#line 846 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("manuscript-reference",
                score->si.manuscript_reference != NULL);
        gregorio_set_score_manuscript_reference (score, (yyvsp[0]).text);
    }
#line 2493 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 28:
#line 854 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("manuscript-storage-place",
                score->si.manuscript_storage_place != NULL);
        gregorio_set_score_manuscript_storage_place (score, (yyvsp[0]).text);
    }
#line 2503 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 29:
#line 862 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("book", score->si.book != NULL);
        gregorio_set_score_book (score, (yyvsp[0]).text);
    }
#line 2512 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 30:
#line 869 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("transcriber", score->si.transcriber != NULL);
        gregorio_set_score_transcriber (score, (yyvsp[0]).text);
        /* free($2.text); */
    }
#line 2522 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 31:
#line 877 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        check_multiple("transcription date",
                score->si.transcription_date != NULL);
        gregorio_set_score_transcription_date (score, (yyvsp[0]).text);
    }
#line 2532 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 32:
#line 885 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if (current_voice_info->style) {
            gregorio_messagef("det_score", VERBOSITY_WARNING, 0,
                    _("several definitions of style found for voice %d, only "
                    "the last will be taken into consideration"), voice);
        }
        gregorio_set_voice_style (current_voice_info, (yyvsp[0]).text);
    }
#line 2545 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 33:
#line 896 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if (current_voice_info->virgula_position) {
            gregorio_messagef("det_score", VERBOSITY_WARNING, 0,
                    _("several definitions of virgula position found for "
                    "voice %d, only the last will be taken into consideration"),
                    voice);
        }
        gregorio_set_voice_virgula_position (current_voice_info, (yyvsp[0]).text);
    }
#line 2559 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 34:
#line 909 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        /* set_voice_generated_by (current_voice_info, $2.text); */
    }
#line 2567 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 35:
#line 915 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_set_score_user_notes (score, (yyvsp[0]).text);
    }
#line 2575 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 36:
#line 921 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        (yyval).text = (yyvsp[-1]).text;
    }
#line 2583 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 37:
#line 925 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        (yyval).text = NULL;
    }
#line 2591 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 68:
#line 961 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        next_voice_info();
    }
#line 2599 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 71:
#line 971 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
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
#line 2626 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 72:
#line 993 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
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
#line 2654 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 73:
#line 1016 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
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
#line 2672 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 74:
#line 1029 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
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
#line 2690 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 75:
#line 1042 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        elements[voice]=NULL;
        voice=0;
        nabc_state=0;
    }
#line 2700 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 76:
#line 1047 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        elements[voice]=NULL;
        voice=0;
        nabc_state=0;
        update_position_with_space();
    }
#line 2711 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 77:
#line 1056 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_ITALIC);
    }
#line 2719 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 78:
#line 1059 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_TT);
    }
#line 2727 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 79:
#line 1062 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_UNDERLINED);
    }
#line 2735 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 80:
#line 1065 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_COLORED);
    }
#line 2743 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 81:
#line 1068 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_BOLD);
    }
#line 2751 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 82:
#line 1071 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_SMALL_CAPS);
    }
#line 2759 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 83:
#line 1074 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_VERBATIM);
    }
#line 2767 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 84:
#line 1077 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_style(ST_SPECIAL_CHAR);
    }
#line 2775 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 85:
#line 1080 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if (!center_is_determined) {
            gregorio_gabc_add_style(ST_FORCED_CENTER);
            center_is_determined=CENTER_HALF_DETERMINED;
        }
    }
#line 2786 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 86:
#line 1089 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_ITALIC);
    }
#line 2794 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 87:
#line 1092 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_TT);
    }
#line 2802 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 88:
#line 1095 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_UNDERLINED);
    }
#line 2810 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 89:
#line 1098 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_COLORED);
    }
#line 2818 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 90:
#line 1101 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_BOLD);
    }
#line 2826 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 91:
#line 1104 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_SMALL_CAPS);
    }
#line 2834 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 92:
#line 1107 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_VERBATIM);
    }
#line 2842 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 93:
#line 1110 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_end_style(ST_SPECIAL_CHAR);
    }
#line 2850 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 94:
#line 1113 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        if (center_is_determined==CENTER_HALF_DETERMINED) {
            gregorio_gabc_end_style(ST_FORCED_CENTER);
            center_is_determined=CENTER_FULLY_DETERMINED;
        }
    }
#line 2861 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 95:
#line 1122 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        euouae = EUOUAE_BEGINNING;
    }
#line 2869 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 96:
#line 1125 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        euouae = EUOUAE_END;
    }
#line 2877 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 97:
#line 1131 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        no_linebreak_area = NLBA_BEGINNING;
    }
#line 2885 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 98:
#line 1134 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        no_linebreak_area = NLBA_END;
    }
#line 2893 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 100:
#line 1141 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        gregorio_gabc_add_text((yyvsp[0]).text);
    }
#line 2901 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 107:
#line 1155 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        start_translation(TR_NORMAL);
    }
#line 2909 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 108:
#line 1161 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        end_translation();
    }
#line 2917 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 109:
#line 1164 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        start_translation(TR_WITH_CENTER_END);
    }
#line 2925 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 110:
#line 1170 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        abovelinestext = (yyvsp[-1]).text;
    }
#line 2933 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 111:
#line 1176 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        rebuild_characters (&current_character, center_is_determined);
        first_text_character = current_character;
        close_syllable(&(yylsp[-2]));
    }
#line 2943 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 112:
#line 1181 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        close_syllable(&(yylsp[-3]));
    }
#line 2951 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 113:
#line 1187 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        close_syllable(NULL);
    }
#line 2959 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;

  case 114:
#line 1190 "gabc/gabc-score-determination.y" /* yacc.c:1646  */
    {
        close_syllable(NULL);
    }
#line 2967 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
    break;


#line 2971 "gabc/gabc-score-determination-y.c" /* yacc.c:1646  */
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
