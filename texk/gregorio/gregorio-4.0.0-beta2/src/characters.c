/*
 * Copyright (C) 2008-2015 The Gregorio Project (see CONTRIBUTORS.md)
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
 * @brief This file contains almost all the gregorio_character manipulation things.
 *
 * @warning The code is very spaghetti, but I don't really think I can do it another way with the chosen representation of styles, and the fact that everything must be xml-compliant and tex-compliant...
 * So this file is basically very hard to maintain. Moreover, it has not been very well coded, so it is even harder to maintain...
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#ifdef USE_KPSE
    #include <kpathsea/kpathsea.h>
#endif
#include "bool.h"
#include "struct.h"
#include "unicode.h"
#include "characters.h"
#include "messages.h"
#include "utf8strings.h"
#include "vowel/vowel.h"

#ifdef _WIN32
#ifndef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif
#endif

#define VOWEL_FILE "gregorio-vowels.dat"

#ifndef USE_KPSE
static __inline void rtrim(char *buf)
{
    char *p;
    for (p = buf + strlen(buf) - 1; p >= buf && isspace(*p); --p) {
        *p = '\0';
    }
}
#endif

static bool read_vowel_rules(char *const lang) {
    char *language = lang;
    rulefile_parse_status status = RFPS_NOT_FOUND;
    char **filenames, *filename, **p;
    const char *description;
    int tries;

#ifdef USE_KPSE
    filenames = kpse_find_file_generic(VOWEL_FILE, kpse_tex_format, true, true);
    if (!filenames) {
        if (strcmp(language, "Latin") != 0) {
            gregorio_messagef("read_patterns", VERBOSITY_WARNING, 0,
                    _("kpse_find_file_generic cannot find %s"), VOWEL_FILE);
        }
        return false;
    }
#else
    FILE *file;
    char buf[PATH_MAX];
    size_t capacity = 16, size = 0;
    
    if (!(filenames = malloc(capacity * sizeof(char *)))) {
        gregorio_messagef("read_patterns", VERBOSITY_FATAL, 0,
                _("unable to allocate memory"));
        return false;
    }

    file = popen("kpsewhich " VOWEL_FILE, "r");
    if (!file) {
        gregorio_messagef("read_patterns", VERBOSITY_WARNING, 0,
                _("unable to run kpsewhich %s: %s"), VOWEL_FILE,
                strerror(errno));
        return false;
    }
    while (!feof(file) && !ferror(file) && fgets(buf, PATH_MAX, file)) {
        rtrim(buf);
        if (strlen(buf) > 0) {
            filenames[size++] = strdup(buf);
            if (size >= capacity) {
                capacity <<= 1;
                if (!(filenames = realloc(filenames,
                                capacity * sizeof(char *)))) {
                    gregorio_messagef("read_patterns", VERBOSITY_FATAL, 0,
                            _("unable to allocate memory"));
                    return false;
                }
            }
        } else {
            gregorio_messagef("read_patterns", VERBOSITY_WARNING, 0,
                    _("kpsewhich returned bad value for %s"), VOWEL_FILE);
        }
    }
    filenames[size] = NULL;
    pclose(file);
#endif

    gregorio_vowel_tables_init();
    /* only need to try twice; if it's not resolved by then, there is an alias
     * loop */
    for (tries = 0; tries < 2; ++tries) {
        for (p = filenames; status != RFPS_FOUND && (filename = *p); ++p) {
            /* read and parse the file */
            gregorio_messagef("read_rules", VERBOSITY_INFO, 0,
                    _("Looking for %s in %s"), language, filename);
            gregorio_vowel_tables_load(filename, &language, &status);
            switch (status) {
            case RFPS_FOUND:
                description = "Found";
                break;
            case RFPS_ALIASED:
                description = "Aliased to";
                break;
            default:
                description = "Could not find";
                break;
            }
            gregorio_messagef("read_rules", VERBOSITY_INFO, 0, _("%s %s in %s"),
                    description, language, filename);
        }
        if (status != RFPS_ALIASED) {
            /* if it's not aliased, there's no reason to scan again */
            break;
        }
    }
    if (status == RFPS_ALIASED) {
        gregorio_messagef("read_rules", VERBOSITY_WARNING, 0,
                _("Unable to resolve alias for %s"), lang);
    }

    /* free the allocated memory */
    for (p = filenames; *p; ++p) {
        free(*p);
    }
    free(filenames);
    if (language != lang) {
        free(language);
    }

    return status == RFPS_FOUND;
}

void gregorio_set_centering_language(char *const language)
{
    if (!read_vowel_rules(language)) {
        if (strcmp(language, "Latin") != 0) {
            gregorio_messagef("gregorio_set_centering_language",
                    VERBOSITY_WARNING, 0, _("unable to read vowel files for "
                        "%s; defaulting to Latin rules"), language);
        }

        gregorio_vowel_tables_init();
        gregorio_vowel_table_add(DEFAULT_VOWELS);
        gregorio_prefix_table_add("i");
        gregorio_prefix_table_add("I");
        gregorio_prefix_table_add("u");
        gregorio_prefix_table_add("U");
    }
}

static __inline gregorio_character *skip_verbatim_or_special(
        gregorio_character *ch)
{
    /* skip past verbatim and special characters */
    if (ch->cos.s.type == ST_T_BEGIN && (ch->cos.s.style == ST_VERBATIM
                || ch->cos.s.style == ST_SPECIAL_CHAR)) {
        if (ch->next_character) {
            ch = ch->next_character;
        }
        while (ch->next_character && ch->is_character) {
            ch = ch->next_character;
        }
    }
    return ch;
}

static void determine_center(gregorio_character *character, int *start,
        int *end) {
    int count;
    grewchar *subject;
    gregorio_character *ch;

    *start = *end = -1;

    for (count = 0, ch = character; ch; ch = ch->next_character) {
        if (ch->is_character) {
            ++count;
        } else {
            ch = skip_verbatim_or_special(ch);
        }
    }
    if (count == 0) {
        return;
    }
    subject = (grewchar *)malloc((count + 1) * sizeof(grewchar));
    for (count = 0, ch = character; ch; ch = ch->next_character) {
        if (ch->is_character) {
            subject[count ++] = ch->cos.character;
        } else {
            ch = skip_verbatim_or_special(ch);
        }
    }
    subject[count] = (grewchar)0;

    gregorio_find_vowel_group(subject, start, end);

    free(subject);
}

/*
 * inline function that will be used for verbatim and special-characters in the 
 * 
 * next function, it calls function with a grewchar * which is the verbatim or
 * special-character. It places current_character to the character next to the
 * end of the verbatim or special_char charachters.
 */
static __inline void verb_or_sp(gregorio_character **ptr_character,
        grestyle_style style, FILE *f, void (*function) (FILE *, grewchar *))
{
    int i, j;
    grewchar *text;
    gregorio_character *current_character;
    gregorio_character *begin_character;

    i = 0;
    j = 0;

    current_character = (*ptr_character)->next_character;
    begin_character = current_character;
    while (current_character) {
        if (current_character->cos.s.type == ST_T_END
                && current_character->cos.s.style == style) {
            break;
        } else {
            if (current_character->is_character) {
                i++;
            }
            current_character = current_character->next_character;
        }
    }
    if (i == 0) {
        ptr_character = &current_character;
        return;
    }
    text = (grewchar *) malloc((i + 1) * sizeof(grewchar));
    current_character = begin_character;
    while (j < i) {
        if (current_character->is_character) {
            text[j] = current_character->cos.character;
            current_character = current_character->next_character;
            j++;
        } else {
            current_character = current_character->next_character;
        }
    }
    text[i] = 0;
    function(f, text);
    free(text);

    *ptr_character = current_character;
}

/**
 * This function is made to simplify the output modules : they just have to
 * declare some simple functions (what to do when meeting a beginning of style,
 * a character, etc.) and to call this function with pointer to these
 * functions, and that will automatically write the good ouput. This function
 * does not test at all the gregorio_character list, if it is wrong, then the
 * ouput will be wrong. It is very simple to understand, even if it is a bit
 * long.
 * 
 * @warning The difficulty comes when we have to write the first syllable text,
 * without the first letter.
 * The behaviour can have some bugs in this case if the first syllable has some
 * complex styles. It would be a bit stupid to do such a thing, but users are
 * usually very creative when it comes to inventing twisted things...
 */
void gregorio_write_text(bool skip_initial,
        gregorio_character *current_character,
        FILE *f, void (*printverb) (FILE *, grewchar *),
        void (*printchar) (FILE *, grewchar),
        void (*begin) (FILE *, grestyle_style),
        void (*end) (FILE *, grestyle_style),
        void (*printspchar) (FILE *, grewchar *))
{
    if (current_character == NULL) {
        return;
    }
    while (current_character) {
        if (current_character->is_character) {
            printchar(f, current_character->cos.character);
        } else {
            if (current_character->cos.s.type == ST_T_BEGIN) {
                switch (current_character->cos.s.style) {
                case ST_VERBATIM:
                    verb_or_sp(&current_character, ST_VERBATIM, f, printverb);
                    break;
                case ST_SPECIAL_CHAR:
                    verb_or_sp(&current_character, ST_SPECIAL_CHAR, f,
                            printspchar);
                    break;
                case ST_INITIAL:
                    if (skip_initial) {
                        while (current_character) {
                            if (!current_character->is_character
                                    && current_character->cos.s.type == ST_T_END
                                    && current_character->cos.s.style ==
                                    ST_INITIAL) {
                                break;
                            }
                            current_character =
                                    current_character->next_character;
                        }
                    } else {
                        begin(f, ST_INITIAL);
                    }
                    break;
                default:
                    begin(f, current_character->cos.s.style);
                    break;
                }
            } else { /* ST_T_END */
                end(f, current_character->cos.s.style);
            }
        }

        current_character = current_character->next_character;
    }
}

/* the default behaviour is to write only the initial, that is to say things
 * between the styles ST_INITIAL */
void gregorio_write_initial(gregorio_character *current_character,
        FILE *f, void (*printverb) (FILE *, grewchar *),
        void (*printchar) (FILE *, grewchar),
        void (*begin) (FILE *, grestyle_style),
        void (*end) (FILE *, grestyle_style),
        void (*printspchar) (FILE *, grewchar *))
{
    /* we loop until we see the beginning of the initial style */
    gregorio_go_to_first_character(&current_character);
    while (current_character) {
        if (!current_character->is_character
                && current_character->cos.s.type == ST_T_BEGIN
                && current_character->cos.s.style == ST_INITIAL) {
            current_character = current_character->next_character;
            break;
        }

        current_character = current_character->next_character;
    }
    /* then we loop until we see the end of the initial style, but we print */
    while (current_character) {
        if (current_character->is_character) {
            printchar(f, current_character->cos.character);
        } else {
            if (current_character->cos.s.type == ST_T_BEGIN) {
                switch (current_character->cos.s.style) {
                case ST_VERBATIM:
                    verb_or_sp(&current_character, ST_VERBATIM, f, printverb);
                    break;
                case ST_SPECIAL_CHAR:
                    verb_or_sp(&current_character, ST_SPECIAL_CHAR, f,
                            printspchar);
                    break;
                default:
                    begin(f, current_character->cos.s.style);
                    break;
                }
            } else { /* ST_T_END */
                if (current_character->cos.s.style == ST_INITIAL) {
                    return;
                } else {
                    end(f, current_character->cos.s.style);
                }
            }
        }
        current_character = current_character->next_character;
    }
}

/*
 * 
 * A very simple function that returns the first text of a score, or the null
 * character if there is no such thing.
 * 
 */

gregorio_character *gregorio_first_text(gregorio_score *score)
{
    gregorio_syllable *current_syllable;
    if (!score || !score->first_syllable) {
        gregorio_message(_("unable to find the first letter of the score"),
                "gregorio_first_text", VERBOSITY_ERROR, 0);
        return NULL;
    }
    current_syllable = score->first_syllable;
    while (current_syllable) {
        if (current_syllable->text) {
            return current_syllable->text;
        }
        current_syllable = current_syllable->next_syllable;
    }

    gregorio_message(_("unable to find the first letter of the score"),
            "gregorio_first_text", VERBOSITY_ERROR, 0);
    return NULL;
}

/*
 * Here starts the code of the handling of text and styles.
 * 
 * This part is not the easiest, in fact is is the most complicated. The reason 
 * is that I want to make something coherent in memory (easy to interprete),
 * and to let the user type whatever he wants. This part was originally written 
 * for gabc, that's why it's always talking about it. But it could as well be
 * used by other read plugins, to put things in order.
 * 
 * Basically all the following lines of code are made for the last function,
 * that will take a "user-written" gregorio_character list into a xml-compliant 
 * and tex-compliant list. It's more complicated than it seems...
 * 
 * Functionalities: For example if the user types tt<i>ttt<b>ttt</i>tt I want
 * it to be represented as tt<i>ttt<b>ttt</b></i><b>tt</b>. The fabulous thing
 * is that it is xml-compliant. This part also determines the middle, for
 * example pot will be interpreted as p{o}t. When I did that I also thought
 * about TeX styles that needed things like {p}{o}{t}, so when the user types
 * <i>pot</i>, it is interpreted as <i>p</i>{<i>o</i>}<i>t</i>.
 * 
 * Internal structure: To do so we have a structure, det_style, that will help
 * us : it is a stack (double chained list) of the styles that we have seen
 * until now. When we encounter a <i>, we push the i style on the stack. If we
 * encounter a </i> we suppress the i style from the stack. Let's take a more
 * complex example: if we encounter <i><b></i>, the stack will be first null,
 * then i then bi, and there we want to end i, but it is not the first style of 
 * the stack, so we close all the styles that we encounter before we encounter
 * i (remember, this is for xml-compliance), so we insert a </b> before the
 * </i>. But that's not all, we also write a <b> after the </i>, so that the b
 * style continues. There our stack is just b. For center, we just close all
 * the styles in the stack, insert a { and reopen all the styles in the stack.
 * 
 * The structure used for character, styles, etc. is described in
 * include/struct.h
 * 
 * The functionment in this file is quite simple : we add all the characters
 * that we see, even if they are incoherent, in the gregorio_character list,
 * and then we call a very complex function that will build a stack of the
 * style, determine the middle, make all xml-compliant, mow the lawn, etc.
 * 
 * This code is *really* spaghetti, but I think it's a necessary pain.
 * 
 */

/*
 * The push function pushes a style in the stack, and updates first_style to
 * this element. 
 */

static void gregorio_style_push(det_style **current_style, unsigned char style)
{
    det_style *element;
    if (!current_style) {
        return;
    }
    element = (det_style *) malloc(sizeof(det_style));
    element->style = style;
    element->previous_style = NULL;
    element->next_style = (*current_style);
    if (*current_style) {
        (*current_style)->previous_style = element;
    }
    (*current_style) = element;
}

/*
 * Pop deletes an style in the stack (the style to delete is the parameter) 
 */

static void gregorio_style_pop(det_style **first_style, det_style *element)
{
    if (!element || !first_style || !*first_style) {
        return;
    }
    if (element->previous_style) {
        element->previous_style->next_style = element->next_style;
    } else {
        if (element->next_style) {
            element->next_style->previous_style = element->previous_style;
            (*first_style) = element->next_style;
        } else {
            (*first_style) = NULL;
        }
    }
    free(element);
}

/*
 * free_styles just free the stack. You may notice that it will never be used
 * in a normal functionment. But we never know... 
 */

static void gregorio_free_styles(det_style **first_style)
{
    det_style *current_style;
    if (!first_style) {
        return;
    }
    current_style = (*first_style);
    while (current_style) {
        current_style = current_style->next_style;
        free((*first_style));
        (*first_style) = current_style;
    }
}


/*
 * 
 * This function inserts a style before current_character, updating the double
 * chained list.
 * 
 */

static void gregorio_insert_style_before(unsigned char type,
        unsigned char style, gregorio_character *current_character)
{
    gregorio_character *element =
            (gregorio_character *) malloc(sizeof(gregorio_character));
    element->is_character = 0;
    element->cos.s.type = type;
    element->cos.s.style = style;
    element->next_character = current_character;
    if (current_character->previous_character) {
        current_character->previous_character->next_character = element;
    }
    element->previous_character = current_character->previous_character;
    current_character->previous_character = element;
}

/*
 * 
 * This function puts a style after current_character, and updates
 * current_character to the gregorio_character it created. It updates the
 * double chained list. It does not touche to the det_styles list.
 * 
 */

static void gregorio_insert_style_after(unsigned char type, unsigned char style,
        gregorio_character **current_character)
{
    gregorio_character *element =
            (gregorio_character *) malloc(sizeof(gregorio_character));
    element->is_character = 0;
    element->cos.s.type = type;
    element->cos.s.style = style;
    element->next_character = (*current_character)->next_character;
    if ((*current_character)->next_character) {
        (*current_character)->next_character->previous_character = element;
    }
    element->previous_character = (*current_character);
    (*current_character)->next_character = element;
    (*current_character) = element;
}

#if 0
static void gregorio_insert_char_after(grewchar c,
        gregorio_character **current_character)
{
    gregorio_character *element =
            (gregorio_character *) malloc(sizeof(gregorio_character));
    element->is_character = 1;
    element->cos.character = c;
    element->next_character = (*current_character)->next_character;
    if ((*current_character)->next_character) {
        (*current_character)->next_character->previous_character = element;
    }
    element->previous_character = (*current_character);
    (*current_character)->next_character = element;
    (*current_character) = element;
}
#endif

/*
 * 
 * This function suppresses the current character, updates the double chained
 * list, and updates current_character to the character after, if there is one.
 * 
 */

static void gregorio_suppress_current_character(
        gregorio_character **current_character)
{
    gregorio_character *thischaracter;
    if (!current_character || !*current_character) {
        return;
    }
    thischaracter = *current_character;
    if ((*current_character)->previous_character) {
        (*current_character)->previous_character->next_character =
                (*current_character)->next_character;
    }
    if ((*current_character)->next_character) {
        (*current_character)->next_character->previous_character =
                (*current_character)->previous_character;
    }
    (*current_character) = (*current_character)->next_character;
    free(thischaracter);
}

/*
 * 
 * This function suppresses the corresponding character, updates the double
 * chained list, but does not touch to current_character.
 * 
 */

static void gregorio_suppress_this_character(gregorio_character *to_suppress)
{
    if (!to_suppress) {
        return;
    }
    if (to_suppress->previous_character) {
        to_suppress->previous_character->next_character =
                to_suppress->next_character;
    }
    if (to_suppress->next_character) {
        to_suppress->next_character->previous_character =
                to_suppress->previous_character;
    }
    free(to_suppress);
}

/*
 * 
 * Then start the inline functions for the big function. the first one is the
 * function we call when we close a style. The magic thing is that is will
 * prevent things like a<i></i>{<i>b</b>: when the user types a<i>b</i>, if the
 * middle is between a and b (...), it will interpret it as a{<i>b</i>.
 * 
 */

static __inline void close_style(gregorio_character *current_character,
        det_style *current_style)
{
    if (!current_character->previous_character->is_character
            && current_character->previous_character->cos.s.style ==
            current_style->style) {
        /*
         * we suppose that there is a previous_character, because there is a
         * current_style
         */
        gregorio_suppress_this_character(current_character->previous_character);
    } else {
        gregorio_insert_style_before(ST_T_END, current_style->style,
                current_character);
    }
}

/*
 * 
 * next the function called when we have determined that we must end the center
 * here : it closes all styles, adds a } and then reopens all styles.
 * 
 */

/* type is ST_CENTER or ST_FORCED_CENTER */
static __inline void end_center(grestyle_style type,
        gregorio_character *current_character, det_style **ptr_style)
{
    det_style *current_style;
    current_style = *ptr_style;

    while (current_style) {
        close_style(current_character, current_style);
        if (current_style->next_style) {
            current_style = current_style->next_style;
        } else {
            break;
        }
    }
    gregorio_insert_style_before(ST_T_END, type, current_character);
    while (current_style) {
        gregorio_insert_style_before(ST_T_BEGIN, current_style->style,
                current_character);
        if (current_style->previous_style) {
            current_style = current_style->previous_style;
        } else {
            break;
        }
    }

    *ptr_style = current_style;
}

/*
 * about the same, but adding a { 
 */

static __inline void begin_center(grestyle_style type,
        gregorio_character *current_character, det_style **ptr_style)
{
    det_style *current_style;
    current_style = *ptr_style;

    while (current_style) {
        close_style(current_character, current_style);
        if (current_style->next_style) {
            current_style = current_style->next_style;
        } else {
            break;
        }
    }
    gregorio_insert_style_before(ST_T_BEGIN, type, current_character);
    while (current_style) {
        gregorio_insert_style_before(ST_T_BEGIN, current_style->style,
                current_character);
        if (current_style->previous_style) {
            current_style = current_style->previous_style;
        } else {
            break;
        }
    }

    *ptr_style = current_style;
}

/*
 * 
 * the macro called when we have ended the determination of the current
 * character. It makes current_character point to the last character, not to
 * null at the end of the determination.
 * 
 */

#define end_c() if (_end_c(&current_character)) continue; else break;
static __inline bool _end_c(gregorio_character **ptr_character)
{
    if ((*ptr_character)->next_character) {
        *ptr_character = (*ptr_character)->next_character;
        /* continue */
        return true;
    } else {
        /* break */
        return false;
    }
}

/*
 * 
 * basically the same except that we suppress the current_character
 * 
 */

#define suppress_char_and_end_c() if (_suppress_char_and_end_c(&current_character)) continue; else break;
static __inline bool _suppress_char_and_end_c(
        gregorio_character **ptr_character)
{
    if ((*ptr_character)->next_character) {
        *ptr_character = (*ptr_character)->next_character;
        gregorio_suppress_this_character((*ptr_character)->previous_character);
        /* continue */
        return true;
    } else {
        if ((*ptr_character)->previous_character) {
            (*ptr_character) = (*ptr_character)->previous_character;
            gregorio_suppress_this_character((*ptr_character)->next_character);
        } else {
            gregorio_suppress_this_character(*ptr_character);
            *ptr_character = NULL;
        }
        /* break */
        return false;
    }
}

static bool gregorio_go_to_end_initial(gregorio_character **param_character)
{
    bool has_initial = false;
    gregorio_character *current_character = *param_character;
    if (!current_character) {
        return false;
    }
    gregorio_go_to_first_character(&current_character);
    /* skip past any initial */
    if (!current_character->is_character
            && current_character->cos.s.type == ST_T_BEGIN
            && current_character->cos.s.style == ST_INITIAL) {
        has_initial = true;
        while (current_character) {
            if (!current_character->is_character
                    && current_character->cos.s.type == ST_T_END
                    && current_character->cos.s.style == ST_INITIAL) {
                break;
            }
            current_character = current_character->next_character;
        }
    }
    (*param_character) = current_character;
    return has_initial;
}

/*
 * THE big function. Very long, using a lot of long macros, etc. I hope you
 * really want to understand it, 'cause it won't be easy.
 * 
 * current character is a pointer to a gregorio_character. The
 * gregorio_character double-chained list it is in will be totally reorganized
 * so that it is xml compliant, and the function will update it to the first
 * character of this brand new list.
 * 
 * center_is_determined has the values present in characters.h.
 * 
 * Another difficulty is the fact that we must consider characters in verbatim
 * and special character styles like only one block, we can't say the center is 
 * in the middle of a verbatim block.
 */

void gregorio_rebuild_characters(gregorio_character **param_character,
        gregorio_center_determination center_is_determined, bool skip_initial)
{
    /* a det_style, to walk through the list */
    det_style *current_style = NULL;
    /* the current_character */
    gregorio_character *current_character = *param_character;
    /* a char that we will use in a very particular case */
    grestyle_style this_style;
    det_style *first_style = NULL;
    /* determining the type of centering (forced or not) */
    grestyle_style center_type = ST_NO_STYLE;
    int start = -1, end = -1, index = -1; 
    /* so, here we start: we go to the first_character */
    if (gregorio_go_to_end_initial(&current_character)) {
        if (!current_character->next_character) {
            /* nothing else to rebuild, but the initial needs to be ST_CENTER */
            gregorio_insert_style_after(ST_T_END, ST_CENTER,
                    &current_character);
            gregorio_go_to_first_character(&current_character);
            gregorio_insert_style_before(ST_T_BEGIN, ST_CENTER,
                    current_character);
            gregorio_go_to_first_character(&current_character);
            (*param_character) = current_character;
            return;
        }
        if (skip_initial) {
            /* move to the character after the initial */
            current_character = current_character->next_character;
        } else {
            gregorio_go_to_first_character(&current_character);
        }
    }
    /* first we see if there is already a center determined */
    if (center_is_determined == 0) {
        center_type = ST_CENTER;
        determine_center(current_character, &start, &end);
    } else {
        center_type = ST_FORCED_CENTER;
    }
    /* we loop until there isn't any character */
    while (current_character) {
        /* the first part of the function deals with real characters (not
         * styles) */
        if (current_character->is_character) {
            ++index;
            /* the firstcase is if the user has'nt determined the middle, and
             * we have only seen vowels so far (else center_is_determined
             * would be DETERMINING_MIDDLE). The current_character is the
             * first vowel, so we start the center here. */
            if (!center_is_determined && index == start) {
                begin_center(center_type, current_character, &current_style);
                center_is_determined = CENTER_DETERMINING_MIDDLE;
                end_c();
            }
            /* the case where the user has not determined the middle and we
             * are in the middle section of the syllable, but there we
             * encounter something that is not a vowel, so the center ends
             * there. */
            if (center_is_determined == CENTER_DETERMINING_MIDDLE
                    && index == end) {
                end_center(center_type, current_character, &current_style);
                center_is_determined = CENTER_FULLY_DETERMINED;
            }
            /* in the case where it is just a normal character... we simply
             * pass. */
            end_c();
        }
        /* there starts the second part of the function that deals with the
         * styles characters */
        if (current_character->cos.s.type == ST_T_BEGIN
                && current_character->cos.s.style != ST_CENTER
                && current_character->cos.s.style != ST_FORCED_CENTER) {
            /* first, if it it the beginning of a style, which is not center.
             * We check if the style is not already in the stack. If it is the
             * case, we suppress the character and pass (with the good macro) */
            while (current_style
                    && current_style->style != current_character->cos.s.style) {
                current_style = current_style->next_style;
            }
            if (current_style) {
                current_style = first_style;
                suppress_char_and_end_c();
            }
            /* if we are determining the end of the middle and we have a
             * VERBATIM or SPECIAL_CHAR style, we end the center
             * determination */
            if ((current_character->cos.s.style == ST_VERBATIM
                            || current_character->cos.s.style ==
                            ST_SPECIAL_CHAR)
                    && center_is_determined == CENTER_DETERMINING_MIDDLE) {
                end_center(center_type, current_character, &current_style);
                center_is_determined = CENTER_FULLY_DETERMINED;
            }
            /* if it is something to add then we just push the style in the
             * stack and continue. */
            gregorio_style_push(&first_style, current_character->cos.s.style);
            current_style = first_style;
            /* Here we pass all the characters after a verbatim (or special
             * char) beginning, until we find a style (begin or end) */
            if (current_character->cos.s.style == ST_VERBATIM
                    || current_character->cos.s.style == ST_SPECIAL_CHAR) {
                if (current_character->next_character) {
                    current_character = current_character->next_character;
                }
                while (current_character->next_character
                        && current_character->is_character) {
                    current_character = current_character->next_character;
                }
            } else {
                end_c();
            }
        }
        /* if it is a beginning of a center, we call the good macro and end. */
        if (current_character->cos.s.type == ST_T_BEGIN
                && (current_character->cos.s.style == ST_CENTER
                        || current_character->cos.s.style ==
                        ST_FORCED_CENTER)) {
            if (current_character->cos.s.style == ST_CENTER) {
                center_type = ST_CENTER;
            } else {
                center_type = ST_FORCED_CENTER;
            }
            if (center_is_determined) {
                end_c();
            }
            /* center_is_determined = DETERMINING_MIDDLE; */
            /* TODO: not really sure, but shouldn't be there */
            begin_center(center_type, current_character, &current_style);
            end_c();
        }
        if (current_character->cos.s.type == ST_T_END
                && current_character->cos.s.style != ST_CENTER
                && current_character->cos.s.style != ST_FORCED_CENTER) {
            /* the case of the end of a style (the most complex). First, we
             * have to see if the style is in the stack. If there is no stack,
             * we just suppress and continue. */
            if (!current_style) {
                suppress_char_and_end_c();
            }
            /* so, we look if it is in the stack. If it is we put
             * current_style to the style just before the style corresponding
             * to the character that we are trating (still there ?) */
            if (current_style->style != current_character->cos.s.style) {
                while (current_style->next_style
                        && current_style->next_style->style !=
                        current_character->cos.s.style) {
                    current_style = current_style->next_style;
                }
                if (current_style->next_style) {
                    current_style = first_style;
                    while (current_style) {
                        /* if there are styles before in the stack, we close
                         * them */
                        gregorio_insert_style_before(ST_T_END,
                                current_style->style, current_character);
                        current_style = current_style->previous_style;
                    }
                    current_style = first_style;
                    this_style = current_character->cos.s.style;
                    /* and then we reopen them */
                    while (current_style && current_style->style != this_style) {
                        gregorio_insert_style_after(ST_T_BEGIN,
                                current_style->style, &current_character);
                        current_style = current_style->next_style;
                    }
                    /* we delete the style in the stack */
                    gregorio_style_pop(&first_style, current_style);
                    current_style = first_style;
                } else {
                    suppress_char_and_end_c();
                }
            } else {
                gregorio_style_pop(&first_style, current_style);
                current_style = first_style;
                end_c();
            }
        } else { /* ST_T_END && ST_CENTER */
            /* a quite simple case, we just call the good macro. */
            if (!center_is_determined) {
                suppress_char_and_end_c();
            }
        }
        end_c();
    }
    if (!current_character) {
        return;
    }
    /* we terminate all the styles that are still in the stack */
    while (current_style) {
        gregorio_insert_style_after(ST_T_END, current_style->style,
                &current_character);
        current_style = current_style->next_style;
    }
    /* current_character is at the end of the list now, so if we havn't closed
     * the center, we do it at the end. */
    if (center_is_determined != CENTER_FULLY_DETERMINED) {
        gregorio_insert_style_after(ST_T_END, center_type, &current_character);
    }
    /* these three lines are for the case where the user didn't tell anything
     * about the middle and there aren't any vowel in the syllable, so we
     * begin the center before the first character (you can notice that there
     * is no problem of style). */
    if (!center_is_determined) {
        if (skip_initial && gregorio_go_to_end_initial(&current_character)) {
            current_character = current_character->next_character;
        } else {
            gregorio_go_to_first_character(&current_character);
        }
        gregorio_insert_style_before(ST_T_BEGIN, ST_CENTER, current_character);
    }
    /* well.. you're quite brave if you reach this comment. */
    gregorio_go_to_first_character(&current_character);
    (*param_character) = current_character;
    gregorio_free_styles(&first_style);
}

/**
 * @brief This function will determine the behaviour of gregorio when it comes to the recognition of the initial. 
 * 
 * Basically it will take a gregorio_character list and return the same list, but with a style added : ST_INITIAL. This style will incidate the initial. The center will be placed at the second letter, unless there is a FORCED_CENTER somewhere. 
 * Finally all will be xml-compliant and tex-compliant when we call gregorio_rebuild_characters.
 *
 * If we note <> for the initial and {} for the center, here is what we want:
 *
 * @li \verbatim Po -> <P>{o} \endverbatim
 * @li \verbatim {A}b -> <>{A}b \endverbatim
 * @li \verbatim {}a -> <>{}a \endverbatim
 * @li \verbatim Glo -> <G>{l}o \endverbatim
 * @li \verbatim Gl{o} -> <G>l{o} \endverbatim
 *
 * @param param_character is a pointer to the (pointer to the) first character, it will be modified so that it points to the new first character.
 */
void gregorio_rebuild_first_syllable(gregorio_character **param_character,
        bool separate_initial)
{
    /* the current_character */
    gregorio_character *current_character = *param_character;
    gregorio_character *first_character;
    gregorio_character *start_of_special;
    /* so, here we start: we go to the first_character */
    gregorio_go_to_first_character(&current_character);
    /* first we look at the styles, to see if there is a FORCED_CENTER
     * somewhere and we also remove the CENTER styles if the syllable starts at
     * CENTER */
    if (!param_character) {
        return;
    }
    while (current_character) {
        if (!current_character->is_character) {
            if (current_character->cos.s.style == ST_FORCED_CENTER) {
                /* we can break here, as there won't be forced center plus
                 * center */
                break;
            }
            if (current_character->cos.s.style == ST_CENTER) {
                /* we have to do it in case param_character (the first
                 * character) is a ST_CENTER beginning */
                if (!current_character->previous_character
                        && current_character == *param_character) {
                    *param_character = (*param_character)->next_character;
                }
                gregorio_suppress_current_character(&current_character);
                continue;
            }
        }
        current_character = current_character->next_character;
    }
    current_character = *param_character;
    gregorio_go_to_first_character(&current_character);
    first_character = current_character;
    /* now we are going to place the two INITIAL styles (begin and end) */
    while (current_character) {
        if (!current_character->is_character
                && current_character->cos.s.style == ST_FORCED_CENTER) {
            /* we don't touch anything after a FORCED_CENTER, so we put an
             * empty INITIAL style just before */
            current_character = first_character;
            gregorio_insert_style_before(ST_T_BEGIN, ST_INITIAL,
                    current_character);
            current_character = current_character->previous_character;
            gregorio_insert_style_after(ST_T_END, ST_INITIAL,
                    &current_character);
            break;
        }
        /* this if is a hack to make gregorio consider verbatim blocks and
         * special chars like one block, not a sequence of letters */
        if (!current_character->is_character
                && current_character->cos.s.type == ST_T_BEGIN
                && (current_character->cos.s.style == ST_VERBATIM
                        || current_character->cos.s.style == ST_SPECIAL_CHAR)) {
            gregorio_insert_style_before(ST_T_BEGIN, ST_INITIAL,
                    current_character);
            start_of_special = current_character->previous_character;
            /* ... which is now the begin-initial style */

            if (current_character->next_character) {
                current_character = current_character->next_character;
            }
            while (current_character->next_character
                    && current_character->is_character) {
                current_character = current_character->next_character;
            }
            gregorio_insert_style_after(ST_T_END, ST_INITIAL,
                    &current_character);
            if (separate_initial && start_of_special->previous_character) {
                /* we need to move the initial to the front */
                start_of_special->previous_character->next_character =
                        current_character->next_character;
                current_character->next_character->previous_character =
                        start_of_special->previous_character;
                start_of_special->previous_character = NULL;
                current_character->next_character = first_character;
                first_character->previous_character = current_character;

                gregorio_message(_
                        ("Any style applied to the initial will be ignored."),
                        NULL, VERBOSITY_WARNING, 0);
            }
            break;
        }
        if (current_character->is_character) {
            if (separate_initial && current_character->previous_character) {
                /* we need to move the initial to the front */
                current_character->previous_character->next_character =
                        current_character->next_character;
                current_character->next_character->previous_character =
                        current_character->previous_character;
                current_character->previous_character = NULL;
                current_character->next_character = first_character;
                first_character->previous_character = current_character;

                gregorio_message(_
                        ("Any style applied to the initial will be ignored."),
                        NULL, VERBOSITY_WARNING, 0);
            }
            gregorio_insert_style_before(ST_T_BEGIN, ST_INITIAL,
                    current_character);
            gregorio_insert_style_after(ST_T_END, ST_INITIAL,
                    &current_character);
            break;
        }
        current_character = current_character->next_character;
    }
    /* now apply the first syllable style */
    current_character = *param_character;
    if (separate_initial) {
        if (gregorio_go_to_end_initial(&current_character)) {
            current_character = current_character->next_character;
        }
    } else {
        gregorio_go_to_first_character(&current_character);
    }
    if (current_character) {
        bool marked_syllable_iniital = false;
        gregorio_insert_style_before(ST_T_BEGIN, ST_FIRST_SYLLABLE,
                current_character);
        do {
            if (!marked_syllable_iniital && (current_character->is_character
                        || (current_character->cos.s.type == ST_T_BEGIN
                            && (current_character->cos.s.style == ST_VERBATIM
                                || current_character->cos.s.style
                                == ST_SPECIAL_CHAR
                                )
                            )
                    )) {
                marked_syllable_iniital = true;
                gregorio_insert_style_before(ST_T_BEGIN,
                        ST_FIRST_SYLLABLE_INITIAL, current_character);
                if (!current_character->is_character) {
                    /* skip past the verbatim or special character */
                    if (current_character->next_character) {
                        current_character = current_character->next_character;
                    }
                    while (current_character->next_character
                            && current_character->is_character) {
                        current_character = current_character->next_character;
                    }
                }
                gregorio_insert_style_after(ST_T_END, ST_FIRST_SYLLABLE_INITIAL,
                        &current_character);
            } else {
                if (current_character->cos.s.style == ST_CENTER
                        || current_character->cos.s.style == ST_FORCED_CENTER) {
                    gregorio_insert_style_before(ST_T_END, ST_FIRST_SYLLABLE,
                            current_character);
                    gregorio_insert_style_after(ST_T_BEGIN, ST_FIRST_SYLLABLE,
                            &current_character);
                }
            }
            if (!current_character->next_character) {
                gregorio_insert_style_after(ST_T_END, ST_FIRST_SYLLABLE,
                        &current_character);
            }
            current_character = current_character->next_character;
        } while (current_character);
    }

    current_character = *param_character;
    gregorio_go_to_first_character(&current_character);
    (*param_character) = current_character;
}
