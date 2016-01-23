%{
/*
 * Gregorio is a program that translates gabc files to GregorioTeX
 * This file implements the score lexer.
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

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "messages.h"
#include "bool.h"
#include "support.h"

#include "gabc.h"
#include "gabc-score-determination.h"
#include "gabc-score-determination-y.h"

static unsigned char style_stack = 0;
static bool eof_found = false;

#define YY_NO_INPUT

#define YY_INPUT(buf,result,max_size) \
    if ( YY_CURRENT_BUFFER_LVALUE->yy_is_interactive ) { \
        int c = '*'; \
        size_t n; \
        for (n = 0; n < max_size \
                && (c = getc(gabc_score_determination_in)) != EOF \
                && c != '\n'; ++n ) { \
            buf[n] = (char) c; \
        } \
        if (c == '\n') { \
            buf[n++] = (char) c; \
        } \
        if (c == EOF && ferror(gabc_score_determination_in)) { \
            YY_FATAL_ERROR("input in flex scanner failed"); \
        } \
        result = n; \
    } else { \
        errno=0; \
        while ((result = fread(buf, 1, max_size, gabc_score_determination_in)) \
                == 0 && ferror(gabc_score_determination_in)) { \
            if (errno != EINTR) { \
                YY_FATAL_ERROR("input in flex scanner failed"); \
                break; \
            } \
            errno = 0; \
            clearerr(gabc_score_determination_in); \
        } \
    } \
    gabc_digest(buf, result)

#define YY_USER_ACTION gabc_update_location(&gabc_score_determination_lloc, \
        gabc_score_determination_text, gabc_score_determination_leng);

%}

%x attribute
%x score
%x notes
%x style
%x verb
%x comments
%x inicomments
%x alt

%option stack
%option pointer
%option nounput
%option noyy_push_state
%option noyy_pop_state
%option noyy_top_state
%option align
%option noread
%option nomain
%option noalways-interactive
%option nonever-interactive
%option prefix="gabc_score_determination_"
%option noyywrap
%option 8bit


/* The expression for attribute below is rather messy because we allow
for (a) single-line values, ending with a semicolon at end of line or a
double semicolon, (b) multi-line values, which end at a double
semicolon. */

%%
<INITIAL>^(\xBB|\xEF|\xBF)* {
        /* BOM written by a lot of windows softwares when they write UTF-8 */
    }
<INITIAL>^[\n\r]+ {
        /* ignoring empty lines */
    }
<INITIAL>^[\%#] {
        BEGIN(inicomments);
    }
<inicomments>(\n|\r)+ {
        BEGIN(INITIAL);
    }
<inicomments>[^\n\r]* {
        /* ignored */
    }
<INITIAL>:(\ )? {
        BEGIN(attribute);
        return COLON;
    }
<attribute>[^;\n\r]*(;[^;\n\r]+)*([\n\r]+[^;]*(;[^;]+)*)? {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return ATTRIBUTE;
    }
<attribute>;(;|[\n\r]+) {
         BEGIN(INITIAL);
         return SEMICOLON;
    }
<INITIAL>def-m[0-9] {
         gabc_score_determination_lval.character = gabc_score_determination_text[5];
         return DEF_MACRO;
    }
<INITIAL>number-of-voices {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
         return NUMBER_OF_VOICES;
    }
<INITIAL>name {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
         return NAME;
    }
<INITIAL>score-copyright {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return SCORE_COPYRIGHT;
    }
<INITIAL>gabc-copyright {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return GABC_COPYRIGHT;
    }
<INITIAL>initial-style {
        /* DEPRECATED by 4.1 */
        return INITIAL_STYLE;
    }
<INITIAL>mode {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return MODE;
    }
<INITIAL>mode-modifier {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return MODE_MODIFIER;
    }
<INITIAL>mode-differentia {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return MODE_DIFFERENTIA;
    }
<INITIAL>annotation {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return ANNOTATION;
    }
<INITIAL>author {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return AUTHOR;
    }
<INITIAL>language {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return LANGUAGE;
    }
<INITIAL>staff-lines {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return STAFF_LINES;
    }
<INITIAL>nabc-lines {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return NABC_LINES;
    }
<INITIAL>oriscus-orientation {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return ORISCUS_ORIENTATION;
    }
<INITIAL>[A-Za-z0-9_]+(-[A-Za-z0-9_]+)* {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return OTHER_HEADER;
    }
<INITIAL>--(.*) {
       return VOICE_CHANGE;
    }
<INITIAL>%(%)*(\n|\r)+ {
       BEGIN(score);
       return END_OF_DEFINITIONS;
    }
<INITIAL>. {
        gregorio_messagef("det_score", VERBOSITY_ERROR, 0,
                _("unrecognized character: \"%c\" in definition part"),
                gabc_score_determination_text[0]);
    }
<score>[^-\{\}\(\[\]<%]+ {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return CHARACTERS;
    }
<score>- {
        return HYPHEN;
    }
<score,style><i> {
        BEGIN(style);
        style_stack ++;
        return I_BEGINNING;
    }
<score,style><nlba> {
        return BNLBA;
    }
<score,style><\/nlba> {
        return ENLBA;
    }
<style><\/i> {
        style_stack --;
        if (style_stack == 0) {
            BEGIN(score);
        }
        return I_END;
    }
<score,style><tt> {
        BEGIN(style);
        style_stack ++;
        return TT_BEGINNING;
    }
<style><\/tt> {
        style_stack --;
        if (style_stack == 0) {
            BEGIN(score);
        }
        return TT_END;
    }
<score,style><ul> {
        BEGIN(style);
        style_stack ++;
        return UL_BEGINNING;
    }
<style><\/ul> {
        style_stack --;
        if (style_stack == 0) {
            BEGIN(score);
        }
        return UL_END;
    }
<score,style><c> {
        BEGIN(style);
        style_stack ++;
        return C_BEGINNING;
    }
<style><\/c> {
        style_stack --;
        if (style_stack == 0) {
            BEGIN(score);
        }
        return C_END;
    }
<score,style><b> {
        BEGIN(style);
        style_stack ++;
        return B_BEGINNING;
    }
<style><\/b> {
        style_stack --;
        if (style_stack == 0) {
            BEGIN(score);
        }
        return B_END;
    }
<score,style><sc> {
        BEGIN(style);
        style_stack ++;
        return SC_BEGINNING;
    }
<style><\/sc> {
        style_stack --;
        if (style_stack == 0) {
            BEGIN(score);
        }
        return SC_END;
    }
<score,style><sp> {
        BEGIN(style);
        style_stack ++;
        return SP_BEGINNING;
    }
<style><\/sp> {
        style_stack --;
        if (style_stack == 0) {
            BEGIN(score);
        }
        return SP_END;
    }
<style>[^<\{\}]* {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return CHARACTERS;
    }
<score>\% {
        BEGIN(comments);
    }
<comments>(\n|\r)+ {
        BEGIN(score);
    }
<comments>[^\n\r]* {
        /* ignored */
    }
<style,score><v> {
        BEGIN(verb);
        return VERB_BEGINNING;
    }
<verb><\/v> {
        if (style_stack == 0) {
            BEGIN(score);
        } else {
            BEGIN(style);
        }
        return VERB_END;
    }
<verb>[^<]* {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return CHARACTERS;
    }
<verb,style,score,alt>< {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return CHARACTERS;
    }
<score,style>\{ {
        return CENTER_BEGINNING;
    }
<score,style>\} {
        return CENTER_END;
    }
<score><alt> {
        BEGIN(alt);
        return ALT_BEGIN;
    }
<alt>[^<]* {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return CHARACTERS;
    }
<alt><\/alt> {
        BEGIN(score);
        return ALT_END;
    }
<score><eu>  {
        return EUOUAE_B;
    }
<score><\/eu>  {
        return EUOUAE_E;
    }
<score>\[\/] {
        return TRANSLATION_CENTER_END;
    }
<score>\[ {
        return TRANSLATION_BEGINNING;
    }
<score>\] {
        return TRANSLATION_END;
    }
<score>\( {
        BEGIN(notes);
        return OPENING_BRACKET;
    }
<notes>[^&|\)]* {
        gabc_score_determination_lval.text =
                gregorio_strdup(gabc_score_determination_text);
        return NOTES;
    }
<notes>& {
        return VOICE_CUT;
    }
<notes>\| {
        return NABC_CUT;
    }
<notes>\) {
        BEGIN(score);
        return CLOSING_BRACKET;
    }
<notes>\)(\ |\t|\n|\r)+ {
        BEGIN(score);
        return CLOSING_BRACKET_WITH_SPACE;
    }
<<EOF>> {
        if (!eof_found) {
            eof_found = true;
            return END_OF_FILE;
        } else {
            yyterminate();
        }
    }
%%

