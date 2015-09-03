/*
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
 * The file starts by the definition of all the structures used in
 * gregorio. As it is certainly the most important file for
 * understanding, read it carefully.
 */

#include "unicode.h"

#ifndef STRUCT_H
#define STRUCT_H

#include "bool.h"
#include "sha1.h"

#ifdef __cplusplus
#define ENUM_BITFIELD(TYPE) enum TYPE
#elif defined(__GNUC__) && __GNUC__ >= 2
#define ENUM_BITFIELD(TYPE) __extension__ enum TYPE
#else
#define ENUM_BITFIELD(TYPE) unsigned int
#endif

/* Lilypond-compatible location counters */
typedef struct gregorio_scanner_location {
    /* - line is the 1-based line number
     * - column is the 0-based* column number, tabs expanded to 8-column stops
     * - offset is the 0-based number of characters from the start of the line
     *
     * * = column is calculated and stored as a 0-based number so the tabstop
     * math is simpler; it should be printed 1-based */

    unsigned short first_line;
    unsigned short first_column;
    unsigned short first_offset;
    unsigned short last_line;
    unsigned short last_column;
    unsigned short last_offset;
} gregorio_scanner_location;

/* all the different types of things a gregorio_* can be */

typedef enum gregorio_type {
    GRE_NOTE = 1,
    GRE_GLYPH,
    GRE_ELEMENT,
    GRE_FLAT,
    GRE_SHARP,
    GRE_NATURAL,
    GRE_C_KEY_CHANGE,
    GRE_C_KEY_CHANGE_FLATED,
    GRE_SYLLABLE,
    GRE_F_KEY_CHANGE,
    GRE_F_KEY_CHANGE_FLATED,
    GRE_END_OF_LINE,
    GRE_SPACE,
    GRE_BAR,
    GRE_END_OF_PAR,
    GRE_CUSTOS,
    /* I don't really know how I could use the a TEXVERB_NOTE in gregoriotex,
     * as we don't write note by note... */
    /* GRE_TEXVERB_NOTE, */
    GRE_TEXVERB_GLYPH,
    GRE_TEXVERB_ELEMENT,
    /* above lines text, quite the same as GRE_TEXVERB_ELEMENT, but counted
     * differently for the spaces above the lines */
    GRE_ALT,
    GRE_NLBA,
    GRE_MANUAL_CUSTOS
} gregorio_type;

/* the different shapes, only for notes */

typedef enum gregorio_shape {
    S_UNDETERMINED = 0,
    S_PUNCTUM,
    S_PUNCTUM_END_OF_GLYPH,
    S_PUNCTUM_INCLINATUM,
    S_PUNCTUM_INCLINATUM_DEMINUTUS,
    S_PUNCTUM_INCLINATUM_AUCTUS,
    S_VIRGA,
    S_VIRGA_REVERSA,
    S_BIVIRGA,
    S_TRIVIRGA,
    S_ORISCUS,
    S_ORISCUS_AUCTUS,
    S_ORISCUS_DEMINUTUS,
    S_ORISCUS_SCAPUS,
    S_QUILISMA,
    S_STROPHA,
    S_STROPHA_AUCTA,
    S_DISTROPHA,
    S_DISTROPHA_AUCTA,
    S_TRISTROPHA,
    S_TRISTROPHA_AUCTA,
    S_PUNCTUM_CAVUM,
    S_LINEA_PUNCTUM,
    S_LINEA_PUNCTUM_CAVUM,
    S_PUNCTUM_CAVUM_INCLINATUM,
    S_PUNCTUM_CAVUM_INCLINATUM_AUCTUS,
    /* special shapes that must not appear in the final form of the score :
     * quadratum is the shape of the first note of a punctum quadratum
     * and quilisma quadratum is the shape of the first note of a pes
     * quislisma quadratum */
    S_QUADRATUM,
    /* those shapes are for now used only in gregoriotex */
    S_QUILISMA_QUADRATUM,
    S_PUNCTUM_AUCTUS_ASCENDENS,
    S_PUNCTUM_AUCTUS_DESCENDENS,
    S_PUNCTUM_DEMINUTUS,
    S_LINEA
} gregorio_shape;

/* the different kind of bars */

typedef enum gregorio_bar {
    B_NO_BAR = 0,
    B_VIRGULA,
    B_DIVISIO_MINIMA,
    B_DIVISIO_MINOR,
    B_DIVISIO_MAIOR,
    B_DIVISIO_FINALIS,
    B_DIVISIO_MINOR_D1,
    B_DIVISIO_MINOR_D2,
    B_DIVISIO_MINOR_D3,
    B_DIVISIO_MINOR_D4,
    B_DIVISIO_MINOR_D5,
    B_DIVISIO_MINOR_D6
} gregorio_bar;

/* definition of the signs. You can notice that the values are made so
 * that if you wan to add a vertical episemus to a note, you juste
 * make note->signs+=_V_EPISEMUS, so please don't change the value as
 * this trick is used. */

typedef enum gregorio_sign {
    _NO_SIGN = 0x00,
    _PUNCTUM_MORA = 0x01,
    _AUCTUM_DUPLEX = 0x02,
    _V_EPISEMUS = 0x10,
    _V_EPISEMUS_PUNCTUM_MORA = 0x11,
    _V_EPISEMUS_AUCTUM_DUPLEX = 0x12,
    /* more rare signs, for now they can't be used with the others */
    _ACCENTUS = 0x03,
    _ACCENTUS_REVERSUS = 0x04,
    _CIRCULUS = 0x05,
    _SEMI_CIRCULUS = 0x06,
    _SEMI_CIRCULUS_REVERSUS = 0x07,
    /* signs of a bar */
    _BAR_H_EPISEMUS = 0x08,
    _V_EPISEMUS_BAR_H_EPISEMUS = 0x18
} gregorio_sign;

/* the different spaces */

typedef enum gregorio_space {
    SP_DEFAULT = 1,
    SP_NO_SPACE,
    SP_ZERO_WIDTH,
    SP_NEUMATIC_CUT,
    SP_LARGER_SPACE,
    SP_GLYPH_SPACE,
    SP_NEUMATIC_CUT_NB,
    SP_LARGER_SPACE_NB,
    SP_GLYPH_SPACE_NB
} gregorio_space;

/* the different liquescences, like for the signs, have special
 * values: to say that something is initio_debilis, just do
 * glyph->liquescentia+=L_INITIO_DEBILIS. So don't change the value,
 * the trick is much used */

typedef enum gregorio_liquescentia {
    L_NO_LIQUESCENTIA = 0,
    L_DEMINUTUS = 0x01,
    L_AUCTUS_ASCENDENS = 0x02,
    L_AUCTUS_DESCENDENS = 0x04,
    L_AUCTA = 0x08,
    L_INITIO_DEBILIS = 0x10,
    L_DEMINUTUS_INITIO_DEBILIS = 0x11,
    L_AUCTUS_ASCENDENS_INITIO_DEBILIS = 0x12,
    L_AUCTUS_DESCENDENS_INITIO_DEBILIS = 0x14,
    L_AUCTA_INITIO_DEBILIS = 0x18
} gregorio_liquescentia;

typedef enum grehepisemus_size {
    H_NORMAL = 0,
    H_SMALL_LEFT,
    H_SMALL_CENTRE,
    H_SMALL_RIGHT
} grehepisemus_size;

/* values are chosen so BELOW/ABOVE can be added to a pitch */
typedef enum gregorio_vposition {
    VPOS_AUTO = 0,
    VPOS_BELOW = -1,
    VPOS_ABOVE = 1
} gregorio_vposition;

/* The different types of glyph */

typedef enum gregorio_glyph_type {
    G_PUNCTUM_INCLINATUM = 1,
    G_2_PUNCTA_INCLINATA_DESCENDENS,
    G_3_PUNCTA_INCLINATA_DESCENDENS,
    G_4_PUNCTA_INCLINATA_DESCENDENS,
    G_5_PUNCTA_INCLINATA_DESCENDENS,
    G_2_PUNCTA_INCLINATA_ASCENDENS,
    G_3_PUNCTA_INCLINATA_ASCENDENS,
    G_4_PUNCTA_INCLINATA_ASCENDENS,
    G_5_PUNCTA_INCLINATA_ASCENDENS,
    G_TRIGONUS,
    G_PUNCTA_INCLINATA,
    /* !!! DO NOT CHANGE THE ENUM ORDERING BEFORE THIS LINE !!! */
    G_UNDETERMINED,
    G_VIRGA,
    G_STROPHA,
    G_STROPHA_AUCTA,
    G_PUNCTUM,
    G_PODATUS,
    G_PES_QUADRATUM,
    G_FLEXA,
    G_TORCULUS,
    G_TORCULUS_RESUPINUS,
    G_TORCULUS_RESUPINUS_FLEXUS,
    G_PORRECTUS,
    G_PORRECTUS_FLEXUS,
    G_BIVIRGA,
    G_TRIVIRGA,
    G_DISTROPHA,
    G_DISTROPHA_AUCTA,
    G_TRISTROPHA,
    G_TRISTROPHA_AUCTA,
    G_PES_QUADRATUM_FIRST_PART,
    G_SCANDICUS,
    G_PES_QUILISMA_QUADRATUM_FIRST_PART,
    G_ANCUS,
    G_ONE_NOTE,
    G_PUNCTA_ASCENDENS,
    G_PUNCTA_DESCENDENS,
    G_VIRGA_REVERSA,
    G_SALICUS,
    G_VIRGA_STRATA,
    G_TORCULUS_LIQUESCENS,
    /* additional glyph types, necessary for determination */
    G_PORRECTUS_NO_BAR,
    G_PORRECTUS_FLEXUS_NO_BAR,
    G_PES_QUILISMA
} gregorio_glyph_type;

/*
 * 
 * The we define the characters. First we define the different styles. You
 * can notice that center is just a style like the others
 * 
 */

typedef enum grestyle_style {
    ST_NO_STYLE = 0,
    ST_ITALIC,
    ST_CENTER,
    /* when the user types a {}, basically the same behaviour, except for
     * the initial */
    ST_FORCED_CENTER,
    ST_BOLD,
    ST_TT,
    ST_SMALL_CAPS,
    ST_SPECIAL_CHAR,
    ST_VERBATIM,
    ST_INITIAL, /* a style used to determine the initial */
    ST_UNDERLINED,
    ST_COLORED,
    ST_FIRST_SYLLABLE,
    ST_FIRST_SYLLABLE_INITIAL
} grestyle_style;

/*
 * Then the different types of styles. See the next comments for further
 * readings. 
 */

typedef enum grestyle_type {
    ST_T_NOTHING = 0,
    ST_T_BEGIN,
    ST_T_END
} grestyle_type;

/*
 * The different types of translation centerings 
 */

typedef enum gregorio_tr_centering {
    TR_NORMAL = 0,
    TR_WITH_CENTER_BEGINNING,
    TR_WITH_CENTER_END
} gregorio_tr_centering;

/*
 * Nothing, beginning or end of area without linebreak 
 */

typedef enum gregorio_nlba {
    NLBA_NORMAL = 0,
    NLBA_BEGINNING,
    NLBA_END
} gregorio_nlba;

typedef enum gregorio_euouae {
    EUOUAE_NORMAL = 0,
    EUOUAE_BEGINNING,
    EUOUAE_END
} gregorio_euouae;

typedef enum gregorio_word_position {
    WORD_BEGINNING = 1,
    WORD_MIDDLE,
    WORD_END,
    WORD_ONE_SYLLABLE
} gregorio_word_position;

/* the centering schemes for gabc: */
typedef enum gregorio_lyric_centering {
    SCHEME_DEFAULT = 0,
    SCHEME_VOWEL,
    SCHEME_SYLLABLE
} gregorio_lyric_centering;

typedef struct gregorio_extra_info {
    /* the sub-type of GRE_END_OF_LINE */
    ENUM_BITFIELD(gregorio_type) sub_type:8;
    ENUM_BITFIELD(gregorio_bar) bar:8;
    ENUM_BITFIELD(gregorio_space) space:8;
    ENUM_BITFIELD(gregorio_nlba) nlba:8;
} gregorio_extra_info;

typedef union gregorio_misc_element_info {
    /* pitched is used for GRE_CUSTOS, GRE_FLAT, GRE_SHARP, GRE_NATURAL,
     * GRE_C_KEY_CHANGE, GRE_F_KEY_CHANGE, GRE_C_KEY_CHANGE_FLATED, and
     * GRE_F_KEY_CHANGE_FLATED */
    struct {
        /* The pitch of the glyph for GRE_FLAT, GRE_NATURAL, GRE_SHARP.
         * If a clef change, pitch will be a number indicating the line of
         * the clef. */
        signed char pitch;
        /* boolean indicating a clef with a B-flat */
        bool flatted_key:1;
        bool force_pitch:1;
    } pitched;
    /* unpitched is used for everything else */
    struct {
        struct gregorio_extra_info info;
        /* an element might carry a sign. */
        ENUM_BITFIELD(gregorio_sign) special_sign:8;
    } unpitched;
} gregorio_misc_element_info;

/*
 * ! We start with the most precise structure, the note structure. The
 * note is always a real note (we'll see that glyphs and elements can be
 * other things). 
 */
typedef struct gregorio_note {
    /* then two pointer to other notes, to make a chained list. */
    struct gregorio_note *previous;
    struct gregorio_note *next;
    /* choral sign is a letter that appears next to a note in some choral
     * scores we put it as char* because sometimes two letters appear */
    char *choral_sign;
    /* a string containing a possible TeX verbatim; necessary during
     * structure generation. */
    char *texverb;
    union {
        /* note is used for GRE_NOTE, GRE_FLAT, GRE_SHARP, GRE_NATURAL,
         * GRE_C_KEY_CHANGE, GRE_F_KEY_CHANGE, GRE_C_KEY_CHANGE_FLATED, and
         * GRE_F_KEY_CHANGE_FLATED */
        struct {
            /* the pitch is the height of the note on the score, that is to
             * say the letter it is represented by in gabc.  If a clef
             * change, pitch will be a number indicating the line of the
             * clef. */
            signed char pitch;
            /* shape is the shape of the note... if you want to know the
             * different possible shapes, see above. */
            ENUM_BITFIELD(gregorio_shape) shape:8;
            /* liquescentia is the liquescence on the note, it is not really
             * used in the final score, but it is, like type, used in the
             * determination of glyphs. */
            ENUM_BITFIELD(gregorio_liquescentia) liquescentia:8;
        } note;
        /* other is used for everything else */
        struct gregorio_extra_info other;
    } u;

    /* these go to the end for structure alignment */
    unsigned short src_line, src_column, src_offset;

    /* we have seen that notes are always real notes, that is to say
     * GRE_NOTE. the type is always that in the final structure. But there
     * is however this field in the structure because of the temporary
     * states that can appear in the determination, where notes can be
     * other things. This is the case for example in gabc reading. */
    ENUM_BITFIELD(gregorio_type) type:8;
    /* signs is the signs on the notes, see above for all possible values */
    ENUM_BITFIELD(gregorio_sign) signs:8;
    /* special_sign is the sign we sometimes encounter on punctum cavum, like
     * accentus, semi-circulus, etc. */
    ENUM_BITFIELD(gregorio_sign) special_sign:8;
    /* h_episemus_type is the type of horizontal episemus, possible values
     * are H_ALONE for an isolated horizontal episemus, H_MULTI_BEGINNING
     * if the note is the first note of an episemus on several notes,
     * H_MULTI_MIDDLE if it is inside an episemus on several notes. I let
     * you guess what could be the use of H_MULTI_END. Other values are
     * temporary values used in determination, they must not appear in the
     * final structure. */

    const char *gtex_offset_case;
    signed char v_episemus_height;
    signed char h_episemus_above;
    signed char h_episemus_below;
    ENUM_BITFIELD(grehepisemus_size) h_episemus_above_size:2;
    ENUM_BITFIELD(grehepisemus_size) h_episemus_below_size:2;
    bool h_episemus_above_connect:1;
    bool h_episemus_below_connect:1;
    bool is_lower_note:1;
    bool is_upper_note:1;
    ENUM_BITFIELD(gregorio_vposition) mora_vposition:2;
    bool choral_sign_is_nabc:1;
} gregorio_note;

/*
 * ! @brief The gregorio glyph structure Unlike gregorio_note,
 * gregorio_glyph can be other things besides \c GRE_GLYPH: it can also be 
 * \c GRE_FLAT, \c GRE_NATURAL or \c GRE_SPACE 
 */
typedef struct gregorio_glyph {
    /* two pointer to make a chained list */
    struct gregorio_glyph *previous;
    struct gregorio_glyph *next;
    /* a string containing a possible TeX verbatim; necessary during structure
     * generation. */
    char *texverb;
    union {
        /* glyph is used for GRE_GLYPH */
        struct {
            /* a pointer to a (chained list of) gregorio_notes, the first of
             * the glyph. */
            struct gregorio_note *first_note;
            /* The glyph type for a GRE_GLYPH (porrectus, pes, etc.).  They
             * are all listed above. */
            ENUM_BITFIELD(gregorio_glyph_type) glyph_type:8;
            /* liquescentia is really used, because that will determine the
             * shape we will have to use. */
            ENUM_BITFIELD(gregorio_liquescentia) liquescentia:8;
        } notes;
        union gregorio_misc_element_info misc;
    } u;

    /* characters go to the end for structure alignment */

    /* type can have the values explained in the comment just above. */
    ENUM_BITFIELD(gregorio_type) type:8;

    /* There is no additional parameter for GRE_SPACE in a gregorio_glyph
     * because gregorio_space in this case can have only one value:
     * SP_ZERO_WIDTH, as other spaces would break the glyphs into
     * different elements. */
} gregorio_glyph;

typedef struct gregorio_element {
    /* pointers to the next and previous elements. */
    struct gregorio_element *previous;
    struct gregorio_element *next;
    /* a string containing a possible TeX verbatim; necessary during structure
     * generation. */
    char *texverb;
    /* The nabc string */
    char **nabc;
    /* we put it here to get the length of the array here too */
    size_t nabc_lines;
    union {
        /* first_glyph is used for GRE_ELEMENT */
        /* a pointer to the first glyph of the element. */
        struct gregorio_glyph *first_glyph;
        union gregorio_misc_element_info misc;
    } u;

    /* type can have the values GRE_ELEMENT, GRE_BAR, GRE_C_KEY_CHANGE,
     * GRE_F_KEY_CHANGE, GRE_END_OF_LINE, GRE_SPACE, GRE_TEXVERB_ELEMENT
     * or GRE_NLBA */
    ENUM_BITFIELD(gregorio_type) type:8;
} gregorio_element;

/*
 * 
 * gregorio_characters are a bit specials. As there can be styles in the
 * text, I had to find a structure mode adapted that just grewchar *. So
 * basically a gregorio_character is a double-chained list of things that
 * can be grewchar or gregorio_styles. For example if you type (in gabc)
 * p<i>o</i>t, the corresponding gregorio_character list will be
 * P->style(type: beginning, style italic) -> o -> style(type:end, style:
 * italic). But for this list to be coherent, it is mandatory that it is
 * xml-compliant, that is to say that a<b>c<i>d</b>e</i> will be
 * interpreted as a<b>c<i>d</i></b><i>e</i>. This MUST be done when reading 
 * a file, so that the structure in memory is coherent. It makes input
 * modules more comple, but output modules muche more simpler. The last
 * particularity is that center must also be determined in the input
 * modules, so that it is already defined in memory. But it is a bit more
 * complex, because for TeX-like output modules, we need to close all
 * styles before the center style: if the user types <i>pot</i> it must be
 * represented as <i>p</i>{<i>o</i>}<i>t</i>.
 * 
 * Here is the declaration of the gregorio_style struct. It is simply two
 * chars, one telling the type of style character it is (beginning for a
 * character that marks the beginning of a style, and end for a character
 * marking the end of a style). The other char simply is the style
 * represented by the character (italic, etc.)
 * 
 */

typedef struct gregorio_style {
    ENUM_BITFIELD(grestyle_style) style:8;
    ENUM_BITFIELD(grestyle_type) type:8;
} gregorio_style;

/*
 * 
 * This union is quite ugly... but necessary for a gregorio_character to be 
 * able to be a grewchar or gregorio_style.
 * 
 */
typedef union character_or_style {
    grewchar character;
    struct gregorio_style s;
} character_or_style;

/*
 * 
 * Finally the gregorio_character structure in itself, It is first a char
 * determining the type (character or gregorio_style). This char is 0 when
 * it is a style and something else when it is a character. Then the two
 * pointers to build the double chained list, and finally the union. So
 * when you want to access to the style of a gregorio_character (when you
 * know it is a character of style), you must access to
 * mygregoriochar.cos.s.style, and for the character
 * mygregoriochar.cos.character .
 * 
 */

typedef struct gregorio_character {
    bool is_character;
    struct gregorio_character *next_character;
    struct gregorio_character *previous_character;
    union character_or_style cos;
} gregorio_character;

typedef struct gregorio_syllable {
    /* pointer to a gregorio_text structure corresponding to the text. */
    struct gregorio_character *text;
    /* pointer to a gregorio_text structure corresponding to the
     * translation */
    struct gregorio_character *translation;
    /* a string representing the text above the lines (raw TeX) */
    char *abovelinestext;
    /* pointer to the next and previous syllable */
    struct gregorio_syllable *next_syllable;
    struct gregorio_syllable *previous_syllable;
    /* and finally a pointer to the elements of the structure. Here we see
     * that we point to an array of elements. In fact it is the array of
     * the first elements of the different voices of the syllable, for the
     * case of polyphonic score. In most scores (monophonic), the array
     * has only one element. */
    struct gregorio_element **elements;
    unsigned short src_line, src_column, src_offset;
    /* a syllable can be a GRE_SYLLABLE, a GRE_*_KEY_CHANGE or a
     * GRE_BAR. It is useful when there is only that in a syllable. */
    char type;
    /* again, an additional field to put some signs or other things... */
    ENUM_BITFIELD(gregorio_sign) special_sign:8;
    /* type of translation (with center beginning or only center end) */
    ENUM_BITFIELD(gregorio_tr_centering) translation_type:2;
    /* beginning or end of area without linebreak? */
    ENUM_BITFIELD(gregorio_nlba) no_linebreak_area:2;
    /* beginning or end of euouae area */
    ENUM_BITFIELD(gregorio_euouae) euouae:2;
    /* position is WORD_BEGINNING for the beginning of a multi-syllable
     * word, WORD_ONE_SYLLABLE for syllable that are alone in their word,
     * and i let you gess what are WORD_MIDDLE and WORD_END. */
    ENUM_BITFIELD(gregorio_word_position) position:3;
} gregorio_syllable;

/* The items in source_info used to be -- well, most of them -- in
 * gregorio_voice_info.  This is because the different `voices' may
 * in future be used for different variants of a melody:
 * e.g. notated in square notation, notated in some early neumatic
 * form from manuscript A, and another in manuscript B.  In that
 * case the different voices would naturally have different source
 * info.  However, this enhancement to gregorio is not yet planned,
 * and so this structure is made part of gregorio_score. */
typedef struct source_info {
    char *author;
    char *date;
    char *manuscript;
    char *manuscript_reference; /* was reference */
    char *manuscript_storage_place; /* was storage_place */
    char *book;
    char *transcriber;
    char *transcription_date;
} source_info;

/*
 * 
 * Score is the top structure, the structure in which we will convert
 * everything, and from which we will construct XML
 * 
 */

#define MAX_ANNOTATIONS 2

typedef struct gregorio_score {
    unsigned char digest[SHA1_DIGEST_SIZE];
    /* the structure starts by a pointer to the first syllable of the
     * score. */
    struct gregorio_syllable *first_syllable;
    /* the number of voices is very important. In monophony it is one. If
     * there are more voices thant number_of_voices, the additional voices
     * won't be taken into consideration. */
    int number_of_voices;
    /* then start some metadata: */
    char *name;
    char *gabc_copyright;
    char *score_copyright;
    char *office_part;
    char *occasion;
    /* the meter, numbers of syllables per line, as e.g. 8.8.8.8 */
    char *meter;
    char *commentary;
    char *arranger;
    struct source_info si;
    /* the mode of a song is between 1 and 8 */
    char mode;
    /* There is one annotation for each line above the initial letter */
    char *annotation[MAX_ANNOTATIONS];
    /* field giving informations on the initial (no initial, normal initial 
     * or two lines initial) */
    char initial_style;
    /* the font to use in gregoriotex */
    char *gregoriotex_font;
    size_t nabc_lines;
    char *user_notes;
    /* the determination method (maximal ambitus, etc.) */
    unsigned char det_method;
    /* then, as there are some metadata that are voice-specific, we add a
     * pointer to the first voice_info. (see comments below) */
    struct gregorio_voice_info *first_voice_info;
    gregorio_lyric_centering centering;
} gregorio_score;

/*
 * 
 * gregorio_voice info contains everything that is voice_specific, for
 * example the key, etc. that can be different from one voice to another.
 * The order of the voice_info (it is a chained list) is the same as the
 * order of the voices (from top to bottom in their representation on the
 * score).
 * 
 */

typedef struct gregorio_voice_info {
    /* the only thing that is worth a comment here is the key. We have a
     * special representation for the key. See comments on
     * src/struct-utils.c for further reading. */
    int initial_key;
    /* See source_info above for comments about the move of author etc. */
    char *style;
    char *virgula_position;
    struct gregorio_voice_info *next_voice_info;
    bool flatted_key;
} gregorio_voice_info;

/* the maximum number of voices, more than this is total nonsense in
 * gregorian chant. */
#define MAX_NUMBER_OF_VOICES 10

#define MAX_TEXT_LENGTH 200

#define C_KEY 'c'
#define F_KEY 'f'
#define NO_KEY -5
#define DEFAULT_KEY 5

#define MONOPHONY 0

/* the different initial styles */

#define NO_INITIAL 0
#define NORMAL_INITIAL 1
#define BIG_INITIAL 2

#define NO_ALTERATION USELESS_VALUE
#define FLAT GRE_FLAT
#define NATURAL GRE_NATURAL

#define USELESS_VALUE 0

static __inline bool is_puncta_inclinata(char glyph)
{
    return glyph <= G_5_PUNCTA_INCLINATA_ASCENDENS;
}

#define IS_INITIO_DEBILIS 5
#define NO_INITIO_DEBILIS 0

static __inline bool is_liquescentia(char liquescentia)
{
    return liquescentia == L_DEMINUTUS || liquescentia == L_AUCTUS_ASCENDENS
        || liquescentia == L_AUCTUS_DESCENDENS || liquescentia == L_AUCTA;
}

static __inline bool is_initio_debilis(char liquescentia)
{
    return liquescentia >= L_INITIO_DEBILIS;
}

#define HEPISEMUS_NONE 0
#define HEPISEMUS_AUTO -1
#define HEPISEMUS_FORCED -2

/* The first pitch MUST be an odd number */
#define LOWEST_PITCH 3
#define HIGHEST_PITCH (LOWEST_PITCH + 12)
#define DUMMY_PITCH (LOWEST_PITCH + 6)

gregorio_score *gregorio_new_score(void);
void gregorio_add_note(gregorio_note **current_note, signed char pitch,
        gregorio_shape shape, gregorio_sign signs,
        gregorio_liquescentia liquescentia, gregorio_note* prototype,
        const gregorio_scanner_location *loc);
void gregorio_add_glyph(gregorio_glyph **current_glyph,
        gregorio_glyph_type type, gregorio_note *first_note,
        gregorio_liquescentia liquescentia);
void gregorio_add_element(gregorio_element **current_element,
        gregorio_glyph *first_glyph);
void gregorio_add_syllable(gregorio_syllable **current_syllable,
        int number_of_voices, gregorio_element *elements[],
        gregorio_character *first_character,
        gregorio_character *first_translation_character,
        gregorio_word_position position, char *abovelinestext,
        gregorio_tr_centering translation_type,
        gregorio_nlba no_linebreak_area,
        gregorio_euouae euouae, const gregorio_scanner_location *loc);
void gregorio_add_special_sign(gregorio_note *current_note, gregorio_sign sign);
void gregorio_change_shape(gregorio_note *note, gregorio_shape shape);
void gregorio_position_h_episemus_above(gregorio_note *note, signed char height,
        bool connect);
void gregorio_position_h_episemus_below(gregorio_note *note, signed char height,
        bool connect);
void gregorio_add_h_episemus(gregorio_note *note, grehepisemus_size size,
        gregorio_vposition vposition, bool disable_bridge,
        unsigned int *nbof_isolated_episemus);
void gregorio_add_sign(gregorio_note *note, gregorio_sign sign,
        gregorio_vposition vposition);
void gregorio_add_liquescentia(gregorio_note *note,
        gregorio_liquescentia liquescentia);
void gregorio_add_voice_info(gregorio_voice_info **current_voice_info);
void gregorio_free_voice_infos(gregorio_voice_info *voice_info);
void gregorio_free_one_note(gregorio_note **note);
void gregorio_free_one_glyph(gregorio_glyph **glyph);
void gregorio_free_score(gregorio_score *score);
void gregorio_go_to_first_character(gregorio_character **character);
void gregorio_add_pitched_element_as_glyph(gregorio_glyph **current_glyph,
        gregorio_type type, signed char pitch, bool flatted_key,
        bool force_pitch, char *texverb);
void gregorio_add_unpitched_element_as_glyph(gregorio_glyph **current_glyph,
        gregorio_type type, gregorio_extra_info info, gregorio_sign sign,
        char *texverb);
void gregorio_add_end_of_line_as_note(gregorio_note **current_note,
        gregorio_type sub_type, const gregorio_scanner_location *loc);
void gregorio_add_custo_as_note(gregorio_note **current_note,
        const gregorio_scanner_location *loc);
void gregorio_add_manual_custos_as_note(gregorio_note **current_note,
        signed char pitch, const gregorio_scanner_location *loc);
void gregorio_add_clef_change_as_note(gregorio_note **current_note,
        gregorio_type type, signed char clef_line,
        const gregorio_scanner_location *loc);
void gregorio_add_bar_as_note(gregorio_note **current_note, gregorio_bar bar,
        const gregorio_scanner_location *loc);
void gregorio_add_alteration_as_note(gregorio_note **current_note,
        gregorio_type type, signed char pitch,
        const gregorio_scanner_location *loc);
void gregorio_add_space_as_note(gregorio_note **current_note,
        gregorio_space space,
        const gregorio_scanner_location *loc);
void gregorio_add_texverb_as_note(gregorio_note **current_note, char *str,
        gregorio_type type, const gregorio_scanner_location *loc);
void gregorio_add_nlba_as_note(gregorio_note **current_note,
        gregorio_nlba type, const gregorio_scanner_location *loc);
void gregorio_add_texverb_to_note(gregorio_note **current_note, char *str);
void gregorio_add_cs_to_note(gregorio_note *const*current_note, char *str,
        bool nabc);
void gregorio_add_misc_element(gregorio_element **current_element,
        gregorio_type type, gregorio_misc_element_info info, char *texverb);
void gregorio_reinitialize_alterations(char alterations[][13],
        int number_of_voices);
void gregorio_reinitialize_one_voice_alterations(char alterations[13]);
void gregorio_set_score_name(gregorio_score *score, char *name);
void gregorio_set_score_gabc_copyright(gregorio_score *score, char *gabc_copyright);
void gregorio_set_score_score_copyright(gregorio_score *score,
        char *score_copyright);
void gregorio_set_score_office_part(gregorio_score *score, char *office_part);
void gregorio_set_score_occasion(gregorio_score *score, char *occasion);
void gregorio_set_score_meter(gregorio_score *score, char *meter);
void gregorio_set_score_commentary(gregorio_score *score, char *commentary);
void gregorio_set_score_arranger(gregorio_score *score, char *arranger);
void gregorio_set_score_gabc_version(gregorio_score *score, char *gabc_version);
void gregorio_set_score_number_of_voices(gregorio_score *score,
        int number_of_voices);
void gregorio_set_score_annotation(gregorio_score *score, char *annotation);
void gregorio_set_score_author(gregorio_score *score, char *author);
void gregorio_set_score_date(gregorio_score *score, char *date);
void gregorio_set_score_manuscript(gregorio_score *score, char *manuscript);
void gregorio_set_score_book(gregorio_score *score, char *book);
void gregorio_set_score_manuscript_reference(gregorio_score *score,
        char *reference);
void gregorio_set_score_manuscript_storage_place(gregorio_score *score,
        char *storage_place);
void gregorio_set_score_transcriber(gregorio_score *score, char *transcriber);
void gregorio_set_score_transcription_date(gregorio_score *score,
        char *transcription_date);
void gregorio_set_score_user_notes(gregorio_score *score, char *user_notes);
void gregorio_set_voice_style(gregorio_voice_info *voice_info, char *style);
void gregorio_set_voice_virgula_position(gregorio_voice_info *voice_info,
        char *virgula_position);
void gregorio_fix_initial_keys(gregorio_score *score, int default_key);
void gregorio_go_to_first_note(gregorio_note **note);
void gregorio_go_to_first_glyph(gregorio_glyph **glyph);
void gregorio_det_step_and_line_from_key(int key, char *step, int *line);
bool gregorio_is_only_special(gregorio_element *element);
int gregorio_calculate_new_key(char step, int line);
void gregorio_add_character(gregorio_character **current_character,
        grewchar wcharacter);
void gregorio_begin_style(gregorio_character **current_character,
        grestyle_style style);
void gregorio_end_style(gregorio_character **current_character,
        grestyle_style style);
signed char gregorio_determine_next_pitch(gregorio_syllable *syllable,
        gregorio_element *element, gregorio_glyph *glyph);

#endif
