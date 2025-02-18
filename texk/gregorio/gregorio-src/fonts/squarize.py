# This is a fontforge python script; use fontforge -lang=py -script to run it
# coding=utf-8
# pylint: disable=too-many-branches, too-many-arguments, too-many-lines
# pylint: disable=import-error, no-member, C0326

"""
    Python fontforge script to build a square notation font.

    Copyright (C) 2013-2025 The Gregorio Project (see CONTRIBUTORS.md)

    This file is part of Gregorio.

    Gregorio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation, either version 3 of the License,
    or (at your option) any later version.

    Gregorio is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Gregorio.  If not, see <http://www.gnu.org/licenses/>.

    This script takes a very simple .sfd file with a few symbols and
    builds a complete square notation font. See gregorio-base.sfd for
    naming conventions of these symbols.

    To build your own font, look at gregorio-base.sfd, and build your
    own glyphs from it.
"""


import sys
import os
import os.path
import json
import argparse
import fontforge
import psMat
import stemsschemas

GPLV3 = """Gregorio is free software: you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Gregorio is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

As a special exception, if you create a document which uses this font,
and embed this font or unaltered portions of this font into the
document, this font does not by itself cause the resulting document to
be covered by the GNU General Public License. This exception does not
however invalidate any other reasons why the document might be covered
by the GNU General Public license. If you modify this font, you may
extend this exception to your version of the font, but you are not
obligated to do so. If you do not wish to do so, delete this exception
statement from your version."""

# defines the maximal interval between two notes, the bigger this number is,
# the more glyphs you'll have to generate
MAX_INTERVAL = 5
ALL_AMBITUS = list(range(1, MAX_INTERVAL + 1))
AMBITUS_ONE_ONLY = [ 1 ]

# this dictionary must have a value for 0 to 14 (the maximum overall ambitus)
AMBITUS = {
    0  : 'Zero',
    1  : 'One',
    2  : 'Two',
    3  : 'Three',
    4  : 'Four',
    5  : 'Five',
    6  : 'Six',
    7  : 'Seven',
    8  : 'Eight',
    9  : 'Nine',
    10 : 'Ten',
    11 : 'Eleven',
    12 : 'Twelve',
    13 : 'Thirteen',
    14 : 'Fourteen',
}

GREGORIO_VERSION = '6.1.0-beta2'

# The unicode character at which we start our numbering:
# U+E000 is the start of the BMP Private Use Area
glyphnumber = 0xe000 - 1

# see defaults in get_parser()
FONT_CONFIG = None
STEM_SCHEMA = None
oldfont = None
newfont = None
font_name = None
subspecies = None
cavum = None
all_glyph_names = {}
FLATTENED_ORISCUS = {}

def get_parser():
    "Return command line parser"
    parser = argparse.ArgumentParser(
        description="""Converts a small set of glyphs into a complete
gregorian square notation font. The initial glyphs have a name
convention, see gregorio-base.sfd.""")
    parser.add_argument('-o', '--outfile',
                        help='output ttf file name',
                        action='store', default=False, dest='outfile')
    parser.add_argument('-s', '--sub-species',
                        help='subspecies (may be \'op\')',
                        action='store', default=False, dest='subspecies')
    parser.add_argument('-v', '--variant',
                        help='variant (may be \'hollow\' or \'hole\')',
                        action='store', default=False, dest='cavum')
    parser.add_argument('-c', '--config-font',
                        help='font-specific configuration',
                        action='store', dest='config_file')
    parser.add_argument('-n', '--name',
                        help='name of the font, should match output file name to be found by luaotfload',
                        action='store', default=False, dest='font_name')
    parser.add_argument('-sc', '--stems-schema',
                        help='stem length schema, can be \'default\' or \'solesmes\'',
                        action='store', default='default', dest='stems_schema')
    parser.add_argument('base_font', help="input sfd file name", action='store')
    return parser

def main():
    "Main function"
    global oldfont, newfont, font_name, subspecies, cavum, FONT_CONFIG, STEM_SCHEMA
    parser = get_parser()
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(1)
    args = parser.parse_args()
    with open(args.config_file, 'r') as stream:
        font_config = json.load(stream)
    subspecies = '_%s' % args.subspecies if args.subspecies else ''
    cavum = args.cavum
    if 'base height' not in font_config:
        font_config['base height'] = 157.5
    if 'bracket shift' not in font_config:
        font_config['bracket shift'] = font_config['base height'] / 2.0
    if 'hepisema additional width' not in font_config:
        font_config['hepisema additional width'] = 5
    if 'deminutus vertical shift' not in font_config:
        font_config['deminutus vertical shift'] = 10
    if 'additional upper queue height' not in font_config:
        font_config['additional upper queue height'] = 10
    FONT_CONFIG = font_config
    STEM_SCHEMA = stemsschemas.get_stem_schema(args.stems_schema, font_config)
    outfile = args.outfile
    inputfile = args.base_font
    if not outfile:
        pre = os.path.splitext(inputfile)
        outfile = '%s.ttf' % pre
    oldfont = fontforge.open(inputfile)
    if args.font_name:
        font_name = args.font_name
    else:
        font_name = oldfont.fontname
    if args.subspecies:
        font_name = '%s-%s' % (font_name, args.subspecies)
    if cavum:
        font_name = '%s-%s' % (font_name, cavum)
    newfont = fontforge.font()
    # newfont.encoding = "UnicodeFull"
    newfont.encoding = "ISO10646-1"
    newfont.fontname = font_name
    newfont.fullname = font_name
    newfont.familyname = font_name
    newfont.version = GREGORIO_VERSION
    newfont.copyright = oldfont.copyright.replace('<<GPLV3>>', GPLV3)
    newfont.weight = "regular"
    initialize_glyphs()
    flattened_oriscus()
    hepisema()
    brackets()
    measures()
    virga()
    pes()
    pes_quadratum()
    pes_oriscus()
    flexus()
    scandicus()
    ancus()
    salicus()
    salicus_flexus()
    torculus()
    torculus_liquescens()
    porrectus()
    porrectusflexus()
    torculusresupinus()
    leading()
    fusion()
    fusion_pes()
    fusion_pes_quadratum()
    fusion_flexus()
    fusion_porrectus()
    # variants and extras must be copied last!
    copy_variant_glyphs()
    copy_extra_glyphs()
    newfont.generate(outfile)
    oldfont.close()
    newfont.close()
    print('last code point in', font_name, 'is', hex(glyphnumber))

def new_glyph():
    """ Create a new glyph and select it.
    """
    global newfont, glyphnumber
    glyphnumber += 1
    if glyphnumber > 0xf8ff:
        print("WARNING: exceeded private use area")
    newfont.selection.select('u%05x' % glyphnumber)

def set_glyph_name(name):
    """ Sets the name of current glyph.
    """
    global all_glyph_names, newfont, glyphnumber
    if glyphnumber in newfont:
        if name in all_glyph_names:
            print(f'ERROR: duplicate glyph name [{name}]', file=sys.stderr)
            sys.exit(1)
        else:
            all_glyph_names[name] = True
        newfont[glyphnumber].glyphname = name

def message(glyph_name):
    """Prints a message to stdout, so that the user gets less bored when
    building the font.
    """
    print("generating", glyph_name, "for", font_name)

def precise_message(glyph_name):
    "Prints more information to entertain the user."
    print("  *", glyph_name)

# This is a dictionary of glyph name to boolean that indicates whether the
# glyph name requires a corresponding cavum variant; the names must consist
# wholly of ASCII letters
DIRECT_GLYPHS = {
    'CClef' : False,
    'FClef' : False,
    'CClefChange' : False,
    'FClefChange' : False,
    'Flat' : False,
    'FlatHole' : False,
    'FlatParen' : False,
    'FlatParenHole' : False,
    'Natural' : False,
    'NaturalHole' : False,
    'NaturalParen' : False,
    'NaturalParenHole' : False,
    'Sharp' : False,
    'SharpHole' : False,
    'SharpParen' : False,
    'SharpParenHole' : False,
    'VirgulaTwo' : False,
    'VirgulaThree' : False,
    'VirgulaFour' : False,
    'VirgulaFive' : False,
    'VirgulaSix' : False,
    'VirgulaParenTwo' : False,
    'VirgulaParenThree' : False,
    'VirgulaParenFour' : False,
    'VirgulaParenFive' : False,
    'VirgulaParenSix' : False,
    'DivisioMinimisTwo' : False,
    'DivisioMinimisThree' : False,
    'DivisioMinimisFour' : False,
    'DivisioMinimisFive' : False,
    'DivisioMinimisSix' : False,
    'DivisioMinimaTwo' : False,
    'DivisioMinimaThree' : False,
    'DivisioMinimaFour' : False,
    'DivisioMinimaFive' : False,
    'DivisioMinimaSix' : False,
    'DivisioMinimaParenTwo' : False,
    'DivisioMinimaParenThree' : False,
    'DivisioMinimaParenFour' : False,
    'DivisioMinimaParenFive' : False,
    'DivisioMinimaParenSix' : False,
    'DivisioMinorTwo' : False,
    'DivisioMinorThree' : False,
    'DivisioMinorFour' : False,
    'DivisioMinorFive' : False,
    'DivisioMaiorTwo' : False,
    'DivisioMaiorThree' : False,
    'DivisioMaiorFour' : False,
    'DivisioMaiorFive' : False,
    'DivisioMaiorDottedTwo' : False,
    'DivisioMaiorDottedThree' : False,
    'DivisioMaiorDottedFour' : False,
    'DivisioMaiorDottedFive' : False,
    'DivisioMaiorDottedBackingTwo' : False,
    'DivisioMaiorDottedBackingThree' : False,
    'DivisioMaiorDottedBackingFour' : False,
    'DivisioMaiorDottedBackingFive' : False,
    'PunctumDeminutus' : False,
    'AuctumMora' : False,
    'Punctum' : True,
    'AscendensPunctumInclinatum' : True,
    'DescendensPunctumInclinatum' : True,
    'StansPunctumInclinatum' : True,
    'Stropha' : True,
    'StrophaAucta' : True,
    'StrophaAuctaLongtail' : True,
    'VirgaBaseLineBL' : True,
    'Quilisma' : True,
    'QuilismaLineTR' : True,
    'AscendensOriscus' : True,
    'AscendensOriscusLineBL' : True,
    'AscendensOriscusLineTL' : True,
    'AscendensOriscusLineTR' : True,
    'DescendensOriscus' : True,
    'DescendensOriscusLineBL' : True,
    'DescendensOriscusLineTL' : True,
    'PunctumInclinatumAuctus' : True,
    'PunctumInclinatumDeminutus' : True,
    'VEpisema' : False,
    'VEpisema.circumflexus' : False,
    'LineaPunctum' : True,
    'Circulus' : False,
    'Semicirculus' : False,
    'Accentus' : False,
    'CustosUpLong' : False,
    'CustosUpShort' : False,
    'CustosUpMedium' : False,
    'CustosDownLong' : False,
    'CustosDownShort' : False,
    'CustosDownMedium' : False,
    'AccentusReversus' : False,
    'SemicirculusReversus' : False,
    'PunctumAscendens' : True,
    'PunctumDescendens' : True,
    'DivisioDominican' : False,
    'DivisioDominicanAlt' : False,
    'Linea' : True,
    'RoundBrace' : False,
    'CurlyBrace' : False,
    'BarBrace' : False,
    'RoundBraceDown' : False,
    'OriscusDeminutus' : True,
    'PunctumSmall' : False,
    'PunctumLineBR' : True,
    'PunctumLineBL' : True,
    'PunctumLineTL' : True,
    'PunctumLineTR' : True,
    'PunctumLineBLBR' : True,
    'PunctumAuctusLineBL' : True,
    'AscendensOriscusLineBLTR' : True,
}

VARIANT_CAVUM = {
    'Punctum#Cavum' : 'Punctum',
    'LineaPunctum#Cavum' : 'LineaPunctum',
}

GLYPH_FALLBACKS = {
    '#Cavum' : {
        'auctusa1' : 'PunctumAscendens',
        'auctusa2' : 'auctusa1',
        'auctusd1' : 'PunctumDescendens',
        'AscendensOriscusFlatBottom' : 'AscendensOriscus',
        'AscendensOriscusLineBRFlatBottom' : 'AscendensOriscusFlatBottom',
        'AscendensOriscusLineTRFlatBottom' : 'AscendensOriscusFlatBottom',
        'AscendensOriscusFlattened' : 'AscendensOriscus',
        'AscendensOriscusFlatTop' : 'AscendensOriscus',
        'AscendensOriscusLineBLFlatTop' : 'AscendensOriscusFlatTop',
        'AscendensOriscusLineTLFlatTop' : 'AscendensOriscusFlatTop',
        'AscendensOriscusLineBL' : 'AscendensOriscus',
        'AscendensOriscusLineBLBR' : 'AscendensOriscus',
        'AscendensOriscusLineBLTR' : 'AscendensOriscus',
        'AscendensOriscusLineBR' : 'AscendensOriscus',
        'AscendensOriscusLineTL' : 'AscendensOriscus',
        'AscendensOriscusLineTLBR' : 'AscendensOriscus',
        'AscendensOriscusLineTLTR' : 'AscendensOriscus',
        'AscendensOriscusLineTR' : 'AscendensOriscus',
        'DescendensOriscusFlatBottom' : 'DescendensOriscus',
        'DescendensOriscusLineBLFlatBottom' : 'DescendensOriscusFlatBottom',
        'DescendensOriscusLineTLFlatBottom' : 'DescendensOriscusFlatBottom',
        'DescendensOriscusFlattened' : 'DescendensOriscus',
        'DescendensOriscusFlatTop' : 'DescendensOriscus',
        'DescendensOriscusLineBRFlatTop' : 'DescendensOriscusFlatTop',
        'DescendensOriscusLineTRFlatTop' : 'DescendensOriscusFlatTop',
        'DescendensOriscusLineBL' : 'DescendensOriscus',
        'DescendensOriscusLineBLBR' : 'DescendensOriscus',
        'DescendensOriscusLineBLTR' : 'DescendensOriscus',
        'DescendensOriscusLineBR' : 'DescendensOriscus',
        'DescendensOriscusLineTL' : 'DescendensOriscus',
        'DescendensOriscusLineTLBR' : 'DescendensOriscus',
        'DescendensOriscusLineTLTR' : 'DescendensOriscus',
        'DescendensOriscusLineTR' : 'DescendensOriscus',
        'mademinutus' : 'Punctum',
        'mdeminutus' : 'Punctum',
        'mnbdeminutus' : 'Punctum',
        'mnbpdeminutus' : 'p2base',
        'mpdeminutus' : 'p2base',
        'msdeminutus' : 'p2base',
        'pesdeminutus' : 'p2base',
        'PunctumAuctusLineBL' : 'PunctumDescendens',
        'PunctumLineTLTR' : 'Punctum',
        'PunctumLineBL' : 'Punctum',
        'PunctumLineBLBR' : 'Punctum',
        'PunctumLineBR' : 'Punctum',
        'PunctumLineTL' : 'Punctum',
        'PunctumLineTR' : 'Punctum',
        'QuilismaLineTR' : 'Quilisma',
        'rvirgabase' : 'virgabase',
        'rvbase' : 'VirgaBaseLineBL',
        'StrophaAuctaLongtail' : 'StrophaAucta',
        'virgabase' : 'Punctum',
        'VirgaBaseLineBL' : 'virgabase',
        'VirgaReversaDescendens' : 'mdeminutus',
        'VirgaReversaLongqueueDescendens' : 'VirgaReversaDescendens',
        'VirgaReversaOpenqueueDescendens' : 'VirgaReversaDescendens',
        'VirgaReversaAscendens' : 'pesdeminutus',
        'VirgaReversaLongqueueAscendens' : 'VirgaReversaAscendens',
        'VirgaReversaOpenqueueAscendens' : 'VirgaReversaAscendens',
    },
}

GLYPH_EXISTS = {}

def glyph_exists(glyph_name):
    "returns if glyph named glyphName exists in font (boolean)"
    global GLYPH_EXISTS, oldfont
    if glyph_name in GLYPH_EXISTS:
        return GLYPH_EXISTS[glyph_name]
    result = True
    try:
        oldfont.selection.select(glyph_name + '')
    except Exception:
        result = False
    GLYPH_EXISTS[glyph_name] = result
    return result

Empty = {}

def subspecies_of(glyph_name, variant, suffix=''):
    fallbacks = GLYPH_FALLBACKS.get(suffix, Empty)
    name_to_try = glyph_name
    while name_to_try:
        if subspecies and glyph_exists(name_to_try + subspecies + suffix + variant):
            return name_to_try + subspecies + suffix + variant
        elif glyph_exists(name_to_try + suffix + variant):
            return name_to_try + suffix + variant
        else:
            name_to_try = fallbacks.get(name_to_try)
            if not name_to_try and variant != '':
                variant = ''
                name_to_try = glyph_name
    return glyph_name + suffix + variant

def copy_existing_glyph(glyph_name, has_cavum=True, variant=''):
    "copies the named glyph, if it exists, and returns whether it was copied"
    if glyph_exists(subspecies_of(glyph_name, variant)):
        complete_paste(glyph_name, has_cavum, variant)
        set_glyph_name(glyph_name)
        return True
    else:
        return False

def copy_existing_glyph_as_new(glyph_name, as_name=None, fallback=None,
        has_cavum=True, variant=''):
    "copies the named glyph as a new glyph"
    glyph_to_copy = glyph_name
    if not glyph_exists(subspecies_of(glyph_to_copy, variant)) and fallback:
        glyph_to_copy = fallback
    if glyph_exists(subspecies_of(glyph_to_copy, variant)):
        new_glyph()
        complete_paste(glyph_to_copy, has_cavum, variant)
        set_glyph_name(as_name or glyph_name)
        return True
    else:
        return False

# This will be populated by initialize_glyphs
COMMON_DIRECT_VARIANTS = {}

def initialize_glyphs():
    "Builds the first glyphs."
    global newfont, oldfont, font_name, cavum
    if cavum == 'hollow':
        # when generating the hollow font, pre-invert Cavum glyphs
        print('inverting cavum glyphs for', font_name)
        for glyph in oldfont.glyphs():
            if "#Cavum" in glyph.glyphname:
                # this is a COPY of the foreground layer
                layer = glyph.foreground
                for contour in layer:
                    contour.reverseDirection()
                # since layer is a COPY, it needs to be put back to take effect
                glyph.foreground = layer
    # DIRECT_GLYPHS contains the names of the glyphs that are already in
    # gregorio_base, mostly one-note glyphs.
    direct_glyphs = list(DIRECT_GLYPHS)
    direct_glyphs.sort()
    # not iterating the dictionary directly because we want sorted key order
    for name in direct_glyphs:
        if glyph_exists(name):
            new_glyph()
            dot_position = name.find('.')
            if dot_position <= 0:
                complete_paste(name, DIRECT_GLYPHS[name])
            else:
                COMMON_DIRECT_VARIANTS[name] = True
                complete_paste(name[:dot_position], DIRECT_GLYPHS[name],
                        name[dot_position:])
            set_glyph_name(name)
        else:
            print('Warning: glyph '+name+' missing from '+font_name)

def use_default_oriscus(orientation, varietal, ignore_varietal = False, line = ''):
    "Uses the base oriscus"
    FLATTENED_ORISCUS[orientation+'Oriscus'+line+varietal] = orientation+'Oriscus'+line

def use_flattened_oriscus(orientation, varietal, ignore_varietal = False, line = ''):
    "Uses flattened oriscus if it exists"
    full_name = orientation+'Oriscus'+line+varietal;
    if glyph_exists(full_name) and not ignore_varietal:
        FLATTENED_ORISCUS[full_name] = full_name
    else:
        if not ignore_varietal:
            print('Warning: glyph '+full_name+' missing from '+font_name)
        FLATTENED_ORISCUS[full_name] = orientation+'Oriscus'+line

def flattened_oriscus():
    "Copies and sets up the flattened oriscus glyphs"
    copy_existing_glyph_as_new('AscendensOriscusFlatBottom',
            S_UPPER_OBLATUS_ASCENDENS_ORISCUS, 'AscendensOriscus')
    copy_existing_glyph_as_new('DescendensOriscusFlatTop',
            S_LOWER_OBLATUS_DESCENDENS_ORISCUS, 'DescendensOriscus')
    global oldfont, FONT_CONFIG, FLATTENED_ORISCUS
    if 'flattened oriscus' in FONT_CONFIG and FONT_CONFIG['flattened oriscus']:
        use = use_flattened_oriscus
    else:
        use = use_default_oriscus
    for orientation in [ 'Ascendens', 'Descendens' ]:
        for varietal in [ 'Flattened', 'FlatTop', 'FlatBottom' ]:
            use(orientation, varietal)
    for line in [ 'LineTL', 'LineBL' ]:
        use('Ascendens', 'FlatTop', False, line)
        use('Ascendens', 'FlatBottom', True, line)
        use('Descendens', 'FlatTop', True, line)
        use('Descendens', 'FlatBottom', False, line)
    for line in [ 'LineTR', 'LineBR' ]:
        use('Ascendens', 'FlatTop', True, line)
        use('Ascendens', 'FlatBottom', False, line)
        use('Descendens', 'FlatTop', False, line)
        use('Descendens', 'FlatBottom', True, line)

def copy_variant_glyphs():
    "Copies the variant glyphs."
    global newfont, oldfont
    for glyph in oldfont.glyphs():
        name = glyph.glyphname
        dot_position = name.find('.')
        if (glyph.isWorthOutputting() and dot_position > 0 and
                name.find("_") == -1 and name not in COMMON_DIRECT_VARIANTS):
            base_name = name[:dot_position]
            variant = name[dot_position:]
            if base_name in DIRECT_GLYPHS:
                new_glyph()
                complete_paste(base_name, DIRECT_GLYPHS[base_name], variant)
                set_glyph_name(name)
            elif base_name in VARIANT_CAVUM:
                new_glyph()
                if cavum:
                    base_name = VARIANT_CAVUM[base_name]
                    complete_paste(base_name, DIRECT_GLYPHS[base_name], variant)
                    set_glyph_name(base_name + variant)

def copy_extra_glyphs():
    "Copies the extra glyphs."
    global FONT_CONFIG, cavum
    # extra glyphs are only supported in the non-cavum font
    if not cavum:
        extra_glyphs = FONT_CONFIG.get('extra glyphs', [])
        if len(extra_glyphs) > 0:
            message('extra glyphs')
            for name in extra_glyphs:
                # must use str(name) here because the JSON strings are
                # apparently not normal strings
                if copy_existing_glyph_as_new(str(name), has_cavum=False):
                    precise_message(name)

# this will be populated by get_width
WIDTHS = {}

def get_width(glyphName):
    "Get length of glyph glyphName in the base font."
    global oldfont
    if glyphName not in WIDTHS:
        if glyph_exists(glyphName):
            WIDTHS[glyphName] = oldfont[subspecies_of(glyphName, '')].width
        else:
            WIDTHS[glyphName] = 0
    return WIDTHS[glyphName]

# Shapes
S_PES                                        = 'Pes'
S_UPPER_PES                                  = 'UpperPes'
S_LOWER_PES                                  = 'LowerPes'
S_PES_QUADRATUM                              = 'PesQuadratum'
S_UPPER_PES_QUADRATUM                        = 'UpperPesQuadratum'
S_LOWER_PES_QUADRATUM                        = 'LowerPesQuadratum'
S_PES_QUADRATUM_LONGQUEUE                    = 'PesQuadratumLongqueue'
S_UPPER_PES_QUADRATUM_LONGQUEUE              = 'UpperPesQuadratumLongqueue'
S_LOWER_PES_QUADRATUM_LONGQUEUE              = 'LowerPesQuadratumLongqueue'
S_PES_QUADRATUM_OPENQUEUE                    = 'PesQuadratumOpenqueue'
S_UPPER_PES_QUADRATUM_OPENQUEUE              = 'UpperPesQuadratumOpenqueue'
S_LOWER_PES_QUADRATUM_OPENQUEUE              = 'LowerPesQuadratumOpenqueue'
S_QUILISMA_PES                               = 'QuilismaPes'
S_PES_QUASSUS                                = 'PesQuassus'
S_UPPER_PES_QUASSUS                          = 'UpperPesQuassus'
S_LOWER_PES_QUASSUS                          = 'LowerPesQuassus'
S_UPPER_OBLATUS_PES_QUASSUS                  = 'UpperOblatusPesQuassus'
S_PES_QUASSUS_LONGQUEUE                      = 'PesQuassusLongqueue'
S_UPPER_PES_QUASSUS_LONGQUEUE                = 'UpperPesQuassusLongqueue'
S_LOWER_PES_QUASSUS_LONGQUEUE                = 'LowerPesQuassusLongqueue'
S_UPPER_OBLATUS_PES_QUASSUS_LONGQUEUE        = 'UpperOblatusPesQuassusLongqueue'
S_PES_QUASSUS_OPENQUEUE                      = 'PesQuassusOpenqueue'
S_UPPER_PES_QUASSUS_OPENQUEUE                = 'UpperPesQuassusOpenqueue'
S_LOWER_PES_QUASSUS_OPENQUEUE                = 'LowerPesQuassusOpenqueue'
S_UPPER_OBLATUS_PES_QUASSUS_OPENQUEUE        = 'UpperOblatusPesQuassusOpenqueue'
S_PES_QUASSUS_INUSITATUS                     = 'PesQuassusInusitatus'
S_UPPER_PES_QUASSUS_INUSITATUS               = 'UpperPesQuassusInusitatus'
S_LOWER_PES_QUASSUS_INUSITATUS               = 'LowerPesQuassusInusitatus'
S_LOWER_OBLATUS_PES_QUASSUS_INUSITATUS       = 'LowerOblatusPesQuassusInusitatus'
S_PES_QUASSUS_INUSITATUS_LONGQUEUE           = 'PesQuassusInusitatusLongqueue'
S_UPPER_PES_QUASSUS_INUSITATUS_LONGQUEUE     = 'UpperPesQuassusInusitatusLongqueue'
S_LOWER_PES_QUASSUS_INUSITATUS_LONGQUEUE     = 'LowerPesQuassusInusitatusLongqueue'
S_LOWER_OBLATUS_PES_QUASSUS_INUSITATUS_LONGQUEUE = 'LowerOblatusPesQuassusInusitatusLongqueue'
S_PES_QUASSUS_INUSITATUS_OPENQUEUE           = 'PesQuassusInusitatusOpenqueue'
S_UPPER_PES_QUASSUS_INUSITATUS_OPENQUEUE     = 'UpperPesQuassusInusitatusOpenqueue'
S_LOWER_PES_QUASSUS_INUSITATUS_OPENQUEUE     = 'LowerPesQuassusInusitatusOpenqueue'
S_LOWER_OBLATUS_PES_QUASSUS_INUSITATUS_OPENQUEUE = 'LowerOblatusPesQuassusInusitatusOpenqueue'
S_QUILISMA_PES_QUADRATUM                     = 'QuilismaPesQuadratum'
S_QUILISMA_PES_QUADRATUM_LONGQUEUE           = 'QuilismaPesQuadratumLongqueue'
S_QUILISMA_PES_QUADRATUM_OPENQUEUE           = 'QuilismaPesQuadratumOpenqueue'
S_FLEXUS                                     = 'Flexus'
S_UPPER_FLEXUS                               = 'UpperFlexus'
S_LOWER_FLEXUS                               = 'LowerFlexus'
S_FLEXUS_NOBAR                               = 'FlexusNobar'
S_FLEXUS_LONGQUEUE                           = 'FlexusLongqueue'
S_FLEXUS_OPENQUEUE                           = 'FlexusOpenqueue'
S_FLEXUS_ORISCUS                             = 'FlexusOriscus'
S_UPPER_FLEXUS_ORISCUS                       = 'UpperFlexusOriscus'
S_LOWER_FLEXUS_ORISCUS                       = 'LowerFlexusOriscus'
S_LOWER_OBLATUS_FLEXUS_ORISCUS               = 'LowerOblatusFlexusOriscus'
S_FLEXUS_ORISCUS_INUSITATUS                  = 'FlexusOriscusInusitatus'
S_UPPER_FLEXUS_ORISCUS_INUSITATUS            = 'UpperFlexusOriscusInusitatus'
S_UPPER_OBLATUS_FLEXUS_ORISCUS_INUSITATUS    = 'UpperOblatusFlexusOriscusInusitatus'
S_LOWER_FLEXUS_ORISCUS_INUSITATUS            = 'LowerFlexusOriscusInusitatus'
S_PORRECTUS_FLEXUS                           = 'PorrectusFlexus'
S_PORRECTUS_FLEXUS_LONGQUEUE                 = 'PorrectusFlexusLongqueue'
S_PORRECTUS_FLEXUS_NOBAR                     = 'PorrectusFlexusNobar'
S_PORRECTUS                                  = 'Porrectus'
S_PORRECTUS_LONGQUEUE                        = 'PorrectusLongqueue'
S_PORRECTUS_NOBAR                            = 'PorrectusNobar'
# for stem length determination only:
S_PORRECTUS_DEMINUTUS_ALT                    = 'PorrectusDeminutus.alt'
S_TORCULUS                                   = 'Torculus'
S_TORCULUS_RESUPINUS                         = 'TorculusResupinus'
S_TORCULUS_QUILISMA                          = 'TorculusQuilisma'
S_TORCULUS_RESUPINUS_QUILISMA                = 'TorculusResupinusQuilisma'
S_SCANDICUS                                  = 'Scandicus'
S_ANCUS                                      = 'Ancus'
S_ANCUS_LONGQUEUE                            = 'AncusLongqueue'
S_ANCUS_OPENQUEUE                            = 'AncusOpenqueue'
S_PES_ASCENDENS_ORISCUS                      = 'PesAscendensOriscus'
S_PES_DESCENDENS_ORISCUS                     = 'PesDescendensOriscus'
S_SALICUS                                    = 'Salicus'
S_SALICUS_LONGQUEUE                          = 'SalicusLongqueue'
S_SALICUS_OPENQUEUE                          = 'SalicusOpenqueue'
S_SALICUS_FLEXUS                             = 'SalicusFlexus'
S_TORCULUS_LIQUESCENS                        = 'TorculusLiquescens'
S_TORCULUS_LIQUESCENS_QUILISMA               = 'TorculusLiquescensQuilisma'
S_FLEXUS_ORISCUS_SCAPUS                      = 'FlexusOriscusScapus'
S_FLEXUS_ORISCUS_SCAPUS_LONGQUEUE            = 'FlexusOriscusScapusLongqueue'
S_FLEXUS_ORISCUS_SCAPUS_OPENQUEUE            = 'FlexusOriscusScapusOpenqueue'
S_FLEXUS_ORISCUS_SCAPUS_INUSITATUS           = 'FlexusOriscusScapusInusitatus'
S_FLEXUS_ORISCUS_SCAPUS_INUSITATUS_LONGQUEUE = 'FlexusOriscusScapusInusitatusLongqueue'
S_FLEXUS_ORISCUS_SCAPUS_INUSITATUS_OPENQUEUE = 'FlexusOriscusScapusInusitatusOpenqueue'
S_LEADING_PUNCTUM                            = 'LeadingPunctum'
S_LEADING_QUILISMA                           = 'LeadingQuilisma'
S_LEADING_ORISCUS                            = 'LeadingOriscus'
S_PUNCTUM                                    = 'Punctum'
S_UPPER_PUNCTUM                              = 'UpperPunctum'
S_LOWER_PUNCTUM                              = 'LowerPunctum'
S_QUILISMA                                   = 'Quilisma'
S_ASCENDENS_ORISCUS                          = 'AscendensOriscus'
S_DESCENDENS_ORISCUS                         = 'DescendensOriscus'
S_ASCENDENS_ORISCUS_SCAPUS                   = 'AscendensOriscusScapus'
S_ASCENDENS_ORISCUS_SCAPUS_LONGQUEUE         = 'AscendensOriscusScapusLongqueue'
S_ASCENDENS_ORISCUS_SCAPUS_OPENQUEUE         = 'AscendensOriscusScapusOpenqueue'
S_DESCENDENS_ORISCUS_SCAPUS                  = 'DescendensOriscusScapus'
S_DESCENDENS_ORISCUS_SCAPUS_LONGQUEUE        = 'DescendensOriscusScapusLongqueue'
S_DESCENDENS_ORISCUS_SCAPUS_OPENQUEUE        = 'DescendensOriscusScapusOpenqueue'
S_UPPER_ASCENDENS_ORISCUS                    = 'UpperAscendensOriscus'
S_UPPER_DESCENDENS_ORISCUS                   = 'UpperDescendensOriscus'
S_UPPER_OBLATUS_ASCENDENS_ORISCUS            = 'UpperOblatusAscendensOriscus'
S_LOWER_ASCENDENS_ORISCUS                    = 'LowerAscendensOriscus'
S_LOWER_DESCENDENS_ORISCUS                   = 'LowerDescendensOriscus'
S_LOWER_OBLATUS_DESCENDENS_ORISCUS           = 'LowerOblatusDescendensOriscus'
S_VIRGA                                      = 'Virga'
S_VIRGA_LONGQUEUE                            = 'VirgaLongqueue'
S_VIRGA_OPENQUEUE                            = 'VirgaOpenqueue'
S_UPPER_VIRGA                                = 'UpperVirga'
S_UPPER_VIRGA_LONGQUEUE                      = 'UpperVirgaLongqueue'
S_UPPER_VIRGA_OPENQUEUE                      = 'UpperVirgaOpenqueue'
S_VIRGA_REVERSA                              = 'VirgaReversa'
S_VIRGA_REVERSA_LONGQUEUE                    = 'VirgaReversaLongqueue'
S_VIRGA_REVERSA_OPENQUEUE                    = 'VirgaReversaOpenqueue'

# Liquescentiae
L_NOTHING                   = 'Nothing'
L_INITIO_DEBILIS            = 'InitioDebilis'
L_DEMINUTUS                 = 'Deminutus'
L_ASCENDENS                 = 'Ascendens'
L_DESCENDENS                = 'Descendens'
L_UP                        = 'Up'
L_DOWN                      = 'Down'
L_INITIO_DEBILIS_DEMINUTUS  = 'InitioDebilisDeminutus'
L_INITIO_DEBILIS_ASCENDENS  = 'InitioDebilisAscendens'
L_INITIO_DEBILIS_DESCENDENS = 'InitioDebilisDescendens'
L_INITIO_DEBILIS_UP         = 'InitioDebilisUp'
# A very special case for stem length: when the queue of ancus must go to the
# bottom of the second note (which is the first note of a deminutus), we use
# this.
L_DEMINUTUS_FIRST           = "DeminutusFirst"

def complete_paste(src, has_cavum, variant=''):
    "Copy and paste a glyph."
    global oldfont, newfont, glyphnumber, cavum
    if cavum == 'hollow':
        paste_glyph(src, has_cavum=has_cavum, variant=variant)
        set_width(get_width(src))
    else:
        if has_cavum or cavum != 'hole':
            oldfont.selection.select(subspecies_of(src, variant,
                '#Cavum' if cavum == 'hole' else ''))
            oldfont.copy()
            newfont.selection.select('u%05x' % glyphnumber)
            newfont.paste()

def simplify():
    """ Simplify a glyph. Does nothing in current version of fontforge (seems to
     be a bug).
     """
    global newfont, glyphnumber
    newfont[glyphnumber].simplify(0, ['mergelines'])
    newfont[glyphnumber].simplify()
    newfont[glyphnumber].removeOverlap()
    newfont[glyphnumber].simplify(0, ['mergelines'])
    newfont[glyphnumber].simplify()

def paste_glyph(src, xdimen=0, ydimen=0, has_cavum=True, variant=''):
    "Copy and paste a glyph, possibly moving it in the process."
    global oldfont, newfont, glyphnumber, cavum
    if not cavum or (has_cavum and cavum == 'hollow'):
        paste_and_move(src, variant, xdimen, ydimen)
    if cavum and has_cavum:
        paste_and_move(src, variant, xdimen, ydimen, '#Cavum')

def paste_and_move(src, variant, xdimen, ydimen, suffix=''):
    "Pastes the src glyph into dst, and moves it with horiz and vert offsets."
    global oldfont, newfont, glyphnumber
    src = subspecies_of(src, variant, suffix)
    if xdimen != 0 or ydimen != 0:
        oldfont[src].transform(psMat.translate(xdimen, ydimen))
    try:
        oldfont.selection.select(src)
    except:
        raise ValueError('unable to select ' + src)
    oldfont.copy()
    newfont.selection.select('u%05x' % glyphnumber)
    newfont.pasteInto()
    if xdimen != 0 or ydimen != 0:
        oldfont[src].transform(psMat.translate(-xdimen, -ydimen))

def end_glyph(name):
    "Final function to call when building a glyph."
    global newfont, glyphnumber
    set_glyph_name(name)
    newfont[glyphnumber].simplify(0, ['mergelines'])
    newfont[glyphnumber].simplify(0)
    newfont[glyphnumber].removeOverlap()
    newfont[glyphnumber].simplify(0, ['mergelines'])
    newfont[glyphnumber].simplify(0)
    newfont[glyphnumber].canonicalContours()
    newfont[glyphnumber].canonicalStart()

def scale(xdimen, ydimen):
    "Scales a glyph, horizontally by x and vertically by y"
    global newfont, glyphnumber
    newfont[glyphnumber].transform(psMat.scale(xdimen, ydimen))

def move(xdimen, ydimen):
    "moves a glyph, horizontally by x and vertically by y"
    global newfont, glyphnumber
    newfont[glyphnumber].transform(psMat.translate(xdimen, ydimen))

def set_width(width):
    "Set the width of a glyph"
    global newfont, glyphnumber
    if glyphnumber in newfont:
        newfont[glyphnumber].width = int(width)

def get_queue_glyph(height, rev = False):
    "Creates the asked line glyph in tmpglyph"
    global oldfont, STEM_SCHEMA
    oldfont.selection.select('queuebase')
    oldfont.copy()
    oldfont.selection.select('tmpglyph')
    oldfont.paste()
    # Mind height sign here, transform doesn't like negative values
    oldfont['tmpglyph'].transform(psMat.scale(1,-height+FONT_CONFIG['stem bottom']+
                                              FONT_CONFIG['additional upper queue height']))
    oldfont['tmpglyph'].transform(psMat.translate(0,height-FONT_CONFIG['stem bottom']))
    queueglyphname = 'queue' if not rev else 'rqueue'
    oldfont.selection.select(queueglyphname)
    oldfont[queueglyphname].transform(psMat.translate(0, height-FONT_CONFIG['stem bottom']))
    oldfont.copy()
    oldfont.selection.select('tmpglyph')
    oldfont.pasteInto()
    oldfont[queueglyphname].transform(psMat.translate(0, -height+FONT_CONFIG['stem bottom']))
    oldfont['tmpglyph'].removeOverlap()
    oldfont['tmpglyph'].simplify(0)
    oldfont['tmpglyph'].simplify(0, ['mergelines'])
    return 'tmpglyph'

def draw_line(i, length, height):
    "Writes a line in currently selected glyph, with length and height offsets"
    if cavum != 'hole' and i > 1:
        linename = "line%d" % i
        paste_and_move(linename, '', length, height)

STEM_LIQ_FALLBACKS = {
    L_DESCENDENS : L_ASCENDENS,
    L_ASCENDENS : L_NOTHING,
    L_DEMINUTUS : L_NOTHING,
    L_INITIO_DEBILIS: L_DEMINUTUS,
    L_INITIO_DEBILIS_DEMINUTUS: L_DEMINUTUS,
    L_INITIO_DEBILIS_ASCENDENS: L_ASCENDENS,
    L_INITIO_DEBILIS_DESCENDENS: L_DESCENDENS,
    L_UP: L_NOTHING,
    L_DOWN: L_NOTHING,
    L_DEMINUTUS_FIRST: L_NOTHING
}

STEM_SHAPE_FALLBACKS = {
    S_VIRGA_REVERSA : S_VIRGA,
    S_ASCENDENS_ORISCUS_SCAPUS : S_VIRGA,
    S_DESCENDENS_ORISCUS_SCAPUS: S_VIRGA,
    S_FLEXUS_ORISCUS_SCAPUS: S_FLEXUS,
    S_PES_QUADRATUM: S_FLEXUS,
    S_PES_QUASSUS: S_PES_QUADRATUM,
    S_QUILISMA_PES_QUADRATUM: S_PES_QUADRATUM,
    S_UPPER_PES_QUADRATUM: S_PES_QUADRATUM,
    S_LOWER_PES_QUADRATUM: S_PES_QUADRATUM,
    S_UPPER_PES_QUASSUS: S_PES_QUASSUS,
    S_UPPER_OBLATUS_PES_QUASSUS: S_UPPER_PES_QUASSUS,
    S_LOWER_PES_QUASSUS: S_PES_QUASSUS,
    S_PORRECTUS: S_FLEXUS,
    S_PORRECTUS_FLEXUS: S_PORRECTUS,
    S_PORRECTUS_DEMINUTUS_ALT: S_FLEXUS,
    # 3 notes glyphs are handled in a different way, see get_default_shift
    #S_ANCUS: S_FLEXUS,
    #S_SALICUS: S_PES_QUASSUS,
}

def get_shift(qtype, shape, liq, i, j):
    """Returns the shift corresponding to the arguments in the queue length
       schema or False
    """
    global STEM_SCHEMA
    #print('get shift %s %s %s %d %d' % (qtype, shape, liq, i, j))
    i = str(i)
    j = str(j)
    if shape not in STEM_SCHEMA:
        return False
    if liq not in STEM_SCHEMA[shape]:
        return False
    if i=='0':
        return STEM_SCHEMA[shape][liq].get(qtype, False)
    elif i not in STEM_SCHEMA[shape][liq]:
        return False
    elif j=='0':
        return STEM_SCHEMA[shape][liq][i].get(qtype, False)
    else:
        if j in STEM_SCHEMA[shape][liq][i] and qtype in STEM_SCHEMA[shape][liq][i][j]:
            return STEM_SCHEMA[shape][liq][i][j][qtype]
        else:
            return False
    return False # Shouldn't happen

def get_default_shift(qtype, shape, liq, i, j, side):
    """Returns the default shift associated with the schema. This
       function should be used for ancus or salicus only. The idea
       is to take i+j when reasonable, i otherwise.
    """
    global STEM_SCHEMA
    if i==0:
        print('fatal error, this should not happen!')
        exit()
    if shape == S_ANCUS:
        if j==0 or STEM_SCHEMA['ignore j']:
            return get_shift(qtype, S_FLEXUS, L_DEMINUTUS_FIRST, i, 0)
        else:
            if MAX_INTERVAL < (i+j):
                return get_shift(qtype, S_FLEXUS, L_DEMINUTUS_FIRST, i, 0)
            else:
                return get_shift(qtype, S_FLEXUS, L_DEMINUTUS, i+j, 0)
    elif shape == S_SALICUS:
        # for this shape, i is the second ambitus j is the first one
        # so that it's just a mirror of ancus
        if j==0 or STEM_SCHEMA['ignore j']:
            return get_queue_shift(qtype, S_PES_QUASSUS, L_NOTHING, i, 'right', 0)
        else:
            if MAX_INTERVAL < (i+j):
                return get_queue_shift(qtype, S_PES_QUASSUS, L_NOTHING, i, 'right', 0)
            else:
                return get_queue_shift(qtype, S_PES_QUADRATUM, L_NOTHING, i+j, 'right', 0)
    else:
        print('calling get_default_shift with shape %s, this should not happen!' % shape)
        exit()

def get_queue_shift(qtype, shape, liq, i=0, side='left', j=0):
    """ Returns the queue shift associated with the arguments in the queue
        length schema, applying all fallbacks.
    """
    global STEM_SCHEMA
    queueshape = shape
    shift = False
    # kind of do-while loop...
    while not shift:
        shift = get_shift(qtype, queueshape, liq, i, j)
        #print(shift)
        queueliq = liq
        while not shift and queueliq in STEM_LIQ_FALLBACKS:
            queueliq = STEM_LIQ_FALLBACKS[queueliq]
            shift = get_shift(qtype, queueshape, queueliq, i, j)
            #print(shift)
        if queueshape in STEM_SHAPE_FALLBACKS:
            queueshape = STEM_SHAPE_FALLBACKS[queueshape]
        else:
            break
    if not shift:
        shift = get_default_shift(qtype, shape, liq, i, j, side)
        #print(shift)
    return shift

def draw_left_queue(i, qtype, stemshape, liq = L_NOTHING, j=0, length=0, shift=0):
    """ Write the left queue of a glyph. Non-obvious args:
         - i: the ambitus between the first and second note
         - j: ambitus between the second and third note in case of an ancus
         - length: horizontal shift to apply to the queue
         - shift: the vertical shift to apply to the queue
    """
    if cavum != 'hole':
        qshift = get_queue_shift(qtype, stemshape, liq, i, 'left', j)
        queue_glyph = get_queue_glyph(qshift, False)
        paste_and_move(queue_glyph, '', length, shift)

def draw_right_queue(i, length, qtype, stemshape, liq = L_NOTHING, j=0, shift=0):
    """ Write the right queue of a glyph. Same arguments as draw_left_queue
    """
    if cavum != 'hole':
        qshift = get_queue_shift(qtype, stemshape, liq, i, 'right', j)
        queue_glyph = get_queue_glyph(qshift, True)
        paste_and_move(queue_glyph, '', length, shift)

def draw_virga_in(right_queue, firstglyph, qtype, liq, stemshape=S_VIRGA, i=0):
    "Draws a virga at height i."
    if not right_queue:
        draw_left_queue(i, qtype, stemshape, liq)
    paste_glyph(firstglyph, 0, (i)*FONT_CONFIG['base height'])
    first_width = get_width(firstglyph)
    if right_queue:
        draw_right_queue(i, first_width-get_width('line2'), qtype, stemshape)
    return first_width

def write_virga(shape, lique=L_NOTHING, right_queue=False, firstglyph='rvirgabase',
                qtype='short', stemshape=S_VIRGA):
    "Writes the virga glyphs."
    new_glyph()
    if lique == L_NOTHING:
        glyph_name = shape
    else:
        glyph_name = '%s%s' % (shape, lique)
    if copy_existing_glyph(glyph_name):
        return
    thislen = draw_virga_in(right_queue, firstglyph, qtype, lique, stemshape)
    set_width(thislen)
    end_glyph(glyph_name)

def virga():
    "Creates virgas"
    message("virgas")
    write_virga(S_VIRGA, L_NOTHING, True, 'virgabase', 'short', S_VIRGA)
    write_virga(S_VIRGA_LONGQUEUE, L_NOTHING, True, 'virgabase', 'long', S_VIRGA)
    write_virga(S_VIRGA_OPENQUEUE, L_NOTHING, True, 'virgabase', 'open', S_VIRGA)
    write_virga(S_UPPER_VIRGA, L_NOTHING, True, 'VirgaBaseLineBL', 'short', S_VIRGA)
    write_virga(S_UPPER_VIRGA_LONGQUEUE, L_NOTHING, True, 'VirgaBaseLineBL', 'long', S_VIRGA)
    write_virga(S_UPPER_VIRGA_OPENQUEUE, L_NOTHING, True, 'VirgaBaseLineBL', 'open', S_VIRGA)
    write_virga(S_VIRGA_REVERSA, L_NOTHING, False, 'rvirgabase', 'short',
            S_VIRGA_REVERSA)
    write_virga(S_VIRGA_REVERSA_LONGQUEUE, L_NOTHING, False, 'rvirgabase',
            'long', S_VIRGA_REVERSA)
    write_virga(S_VIRGA_REVERSA_OPENQUEUE, L_NOTHING, False, 'rvirgabase',
            'open', S_VIRGA_REVERSA)
    write_virga(S_VIRGA_REVERSA, L_DESCENDENS, False, 'PunctumAuctusLineBL',
            'short', S_VIRGA)
    write_virga(S_VIRGA_REVERSA_LONGQUEUE, L_DESCENDENS, False,
            'PunctumAuctusLineBL', 'long', S_VIRGA)
    write_virga(S_VIRGA_REVERSA_OPENQUEUE, L_DESCENDENS, False,
            'PunctumAuctusLineBL', 'open', S_VIRGA)
    write_virga(S_VIRGA_REVERSA, L_ASCENDENS, False, 'auctusa2', 'short',
            S_VIRGA)
    write_virga(S_VIRGA_REVERSA_LONGQUEUE, L_ASCENDENS, False, 'auctusa2',
            'long', S_VIRGA)
    write_virga(S_VIRGA_REVERSA_OPENQUEUE, L_ASCENDENS, False, 'auctusa2',
            'open', S_VIRGA)
    write_virga(S_ASCENDENS_ORISCUS_SCAPUS, L_NOTHING, False,
            'AscendensOriscusLineBL', 'short', S_ASCENDENS_ORISCUS_SCAPUS)
    write_virga(S_ASCENDENS_ORISCUS_SCAPUS_LONGQUEUE, L_NOTHING, False,
            'AscendensOriscusLineBL', 'long', S_ASCENDENS_ORISCUS_SCAPUS)
    write_virga(S_ASCENDENS_ORISCUS_SCAPUS_OPENQUEUE, L_NOTHING, False,
            'AscendensOriscusLineBL', 'open', S_ASCENDENS_ORISCUS_SCAPUS)
    write_virga(S_DESCENDENS_ORISCUS_SCAPUS, L_NOTHING, False,
            'DescendensOriscusLineBL', 'short', S_DESCENDENS_ORISCUS_SCAPUS)
    write_virga(S_DESCENDENS_ORISCUS_SCAPUS_LONGQUEUE, L_NOTHING, False,
            'DescendensOriscusLineBL', 'long', S_DESCENDENS_ORISCUS_SCAPUS)
    write_virga(S_DESCENDENS_ORISCUS_SCAPUS_OPENQUEUE, L_NOTHING, False,
            'DescendensOriscusLineBL', 'open', S_DESCENDENS_ORISCUS_SCAPUS)

def draw_deminutus(i, j, length=0, tosimplify=0, firstbar=1):
    """As the glyph before a deminutus is not the same as a normal glyph,
    and always the same, we can call this function each
    time. Sometimes we have to simplify before building the last glyph
    (tosimplify=1), and length is the offset.
    """
    first_glyph_is_complete = False
    first_glyph = 'mademinutus' # firstbar == 2
    if firstbar == 1:
        first_glyph = 'mdeminutus'
    elif firstbar == 0:
        first_glyph = 'mnbdeminutus'
    if j == 1:
        if glyph_exists('%sam1flexus' % first_glyph):
            first_glyph = '%sam1flexus' % first_glyph
            first_glyph_is_complete = True
        elif glyph_exists('%sam1' % first_glyph):
            first_glyph = '%sam1flexus' % first_glyph
    paste_glyph(first_glyph, length, i*FONT_CONFIG['base height'])
    if not first_glyph_is_complete:
        draw_line(j, length+get_width(first_glyph)-get_width('line2'),
                   (i-j+1)*FONT_CONFIG['base height'])
    if tosimplify:
        simplify()
    if not first_glyph_is_complete:
        paste_glyph("deminutus",
                       length+get_width(first_glyph)-get_width('deminutus'),
                       (i-j)*FONT_CONFIG['base height'])
    return get_width(first_glyph)

def measures():
    "Creates glyphs used only for measurement."
    message("glyphs for measurement")
    new_glyph()
    first_glyph = 'PunctumLineBLBR'
    second_glyph = 'PunctumLineTL'
    paste_glyph(first_glyph)
    draw_line(1, get_width(first_glyph) - get_width('line2'), -FONT_CONFIG['base height'])
    paste_glyph(second_glyph, get_width(first_glyph) - get_width('line2'),
                   -FONT_CONFIG['base height'])
    set_width(get_width(first_glyph)+get_width(second_glyph)-get_width('line2'))
    end_glyph('FlexusLineBL')
    new_glyph()
    first_glyph = 'PunctumLineBL'
    second_glyph = 'Punctum'
    paste_glyph(first_glyph)
    paste_glyph(second_glyph, get_width(first_glyph), -FONT_CONFIG['base height'])
    set_width(get_width(first_glyph)+get_width(second_glyph))
    end_glyph('FlexusAmOneLineBL')

HEPISEMA_GLYPHS = {
    'HEpisemaPunctum': 'Punctum',
    'HEpisemaFlexusDeminutus': 'mpdeminutus',
    'HEpisemaDebilis': 'idebilis',
    'HEpisemaInclinatum': 'DescendensPunctumInclinatum',
    'HEpisemaInclinatumDeminutus': 'PunctumInclinatumDeminutus',
    'HEpisemaStropha': 'Stropha',
    'HEpisemaQuilisma': 'Quilisma',
    'HEpisemaQuilismaLineTR': 'QuilismaLineTR',
    'HEpisemaHighPes': 'PunctumSmall',
    'HEpisemaAscendensOriscus': 'AscendensOriscus',
    'HEpisemaVirga': 'rvirgabase',
    'HEpisemaVirgaBaseLineBL': 'VirgaBaseLineBL',
    'HEpisemaAscendensOriscusLineTR': 'AscendensOriscusLineTR',
    'HEpisemaPunctumLineBR': 'PunctumLineBR',
    'HEpisemaPunctumLineBL': 'PunctumLineBL',
    'HEpisemaPunctumLineTL': 'PunctumLineTL',
    'HEpisemaPunctumLineTR': 'PunctumLineTR',
    'HEpisemaPunctumLineBLBR': 'PunctumLineBLBR',
    'HEpisemaPunctumAuctusLineBL': 'PunctumAuctusLineBL',
    'HEpisemaAscendensOriscusLineBLTR': 'AscendensOriscusLineBLTR',
    'HEpisemaFlat': 'Flat',
    'HEpisemaSharp': 'Sharp',
    'HEpisemaNatural': 'Natural',
    'HEpisemaBarStandard': 'DivisioMinimaTwo',
    'HEpisemaBarVirgula': 'VirgulaTwo',
    'HEpisemaBarParen': 'DivisioMinimaParenTwo',
    'HEpisemaBarVirgulaParen': 'VirgulaParenTwo',
    'HEpisemaFlatParen': 'FlatParen',
    'HEpisemaSharpParen': 'SharpParen',
    'HEpisemaNaturalParen': 'NaturalParen',
}

def hepisema():
    "Creates horizontal episemata."
    message("horizontal episema")
    for target, source in list(HEPISEMA_GLYPHS.items()):
        write_hepisema(get_width(source), target)
        write_hepisema(get_width(source) * 2.0 / 3.0, target + "Reduced")
    reduction = get_width('PunctumSmall')
    for i in ALL_AMBITUS:
        write_hepisema(get_width("porrectus%d"%i),
                       'HEpisemaPorrectus%s' % AMBITUS[i], reduction)
    for i in ALL_AMBITUS:
        if glyph_exists("porrectusam1%d"%i):
            write_hepisema(get_width("porrectusam1%d"%i),
                           'HEpisemaPorrectusAmOne%s' % AMBITUS[i], reduction)
        else:
            write_hepisema(get_width("porrectus%d"%i),
                           'HEpisemaPorrectusAmOne%s' % AMBITUS[i], reduction)
    # porrectus flexus does not get reduced because the note after is to the right
    for i in ALL_AMBITUS:
        write_hepisema(get_width("porrectusflexus%d"%i),
                       'HEpisemaPorrectusFlexus%s' % AMBITUS[i])

def write_hepisema(shape_width, glyphname, reduction=0):
    "Writes the horizontal episema glyphs."
    global FONT_CONFIG
    new_glyph()
    if not cavum:
        paste_glyph("hepisema_base", has_cavum=False)
        drawn_width = shape_width - reduction
        scale(drawn_width + 2*FONT_CONFIG['hepisema additional width'], 1)
        move(-FONT_CONFIG['hepisema additional width'], 0)
        if glyph_exists('hepisemaleft'):
            paste_glyph("hepisemaleft", -FONT_CONFIG['hepisema additional width'],
                    0, False)
        if glyph_exists('hepisemaright'):
            paste_glyph("hepisemaright",
                    drawn_width + FONT_CONFIG['hepisema additional width'], 0,
                    False)
        # use the original width for the glyph for the sake of ledger lines
        set_width(shape_width)
        end_glyph(glyphname)

def pes():
    "Creates the pes."
    message("pes")
    precise_message("pes")
    write_all_pes("p2base", S_PES)
    precise_message("pes deminutus")
    write_all_pes_deminutus("pesdeminutus", S_PES, L_DEMINUTUS)
    precise_message("pes quilisma")
    write_all_pes("QuilismaLineTR", S_QUILISMA_PES)
    precise_message("pes quilisma deminutus")
    write_all_pes_deminutus("QuilismaLineTR", S_QUILISMA_PES, L_DEMINUTUS)
    precise_message("pes quassus deminutus")
    write_all_pes_deminutus("AscendensOriscusLineTR", S_PES_QUASSUS, L_DEMINUTUS)
    write_all_pes_deminutus("DescendensOriscusLineTR", S_PES_QUASSUS_INUSITATUS,
            L_DEMINUTUS)
    precise_message("pes initio debilis")
    write_all_pes_debilis(S_PES, L_INITIO_DEBILIS)
    precise_message("pes initio debilis deminutus")
    write_all_pes_debilis_deminutus(S_PES, L_INITIO_DEBILIS_DEMINUTUS)

def fusion_pes():
    "Creates the fusion pes."
    message("fusion pes")
    precise_message("fusion pes")
    write_all_pes("msdeminutus", S_UPPER_PES)
    precise_message("fusion pes deminutus")
    write_all_pes_deminutus("msdeminutus", S_UPPER_PES, L_DEMINUTUS)
    write_all_pes_deminutus("mpdeminutus", S_LOWER_PES, L_DEMINUTUS)
    precise_message("fusion pes quassus deminutus")
    write_all_pes_deminutus("AscendensOriscusLineBLTR", S_UPPER_PES_QUASSUS,
            L_DEMINUTUS)
    write_all_pes_deminutus(FLATTENED_ORISCUS["AscendensOriscusLineTRFlatBottom"],
            S_UPPER_OBLATUS_PES_QUASSUS, L_DEMINUTUS)
    write_all_pes_deminutus("DescendensOriscusLineBLTR",
            S_UPPER_PES_QUASSUS_INUSITATUS, L_DEMINUTUS)
    write_all_pes_deminutus("AscendensOriscusLineTLTR", S_LOWER_PES_QUASSUS,
            L_DEMINUTUS)
    write_all_pes_deminutus("DescendensOriscusLineTLTR",
            S_LOWER_PES_QUASSUS_INUSITATUS, L_DEMINUTUS)
    write_all_pes_deminutus(FLATTENED_ORISCUS["DescendensOriscusLineTRFlatTop"],
            S_LOWER_OBLATUS_PES_QUASSUS_INUSITATUS, L_DEMINUTUS)

def write_all_pes(first_glyph, shape, lique=L_NOTHING, i_range=ALL_AMBITUS):
    for i in i_range:
        write_pes(i, first_glyph, shape, lique)

def write_pes(i, first_glyph, shape, lique=L_NOTHING):
    "Writes the pes glyphs."
    new_glyph()
    glyph_name = '%s%s%s' % (shape, AMBITUS[i], lique)
    if copy_existing_glyph(glyph_name):
        return
    get_width('QuilismaLineTR')
    temp_width = 0
    width_difference = get_width(first_glyph)-get_width('PunctumSmall')
    if width_difference < 0:
        paste_glyph(first_glyph, -width_difference, 0)
    else:
        paste_glyph(first_glyph)
    temp_width = get_width(first_glyph)-get_width('line2')
    if width_difference < 0:
        temp_width = temp_width-width_difference
    draw_line(i, temp_width, FONT_CONFIG['base height'])
    if width_difference != 0:
        paste_glyph('PunctumSmall', width_difference,
                i*FONT_CONFIG['base height'])
    else:
        paste_glyph('PunctumSmall', 0, i*FONT_CONFIG['base height'])
    if width_difference < 0:
        set_width(get_width('PunctumSmall'))
    else:
        set_width(get_width(first_glyph))
    end_glyph(glyph_name)

def write_all_pes_debilis(shape, lique=L_NOTHING, i_range=ALL_AMBITUS):
    for i in i_range:
        write_pes_debilis(i, shape, lique)

def write_pes_debilis(i, shape, lique=L_NOTHING):
    "Writes the pes debilis glyphs."
    new_glyph()
    glyph_name = '%s%s%s' % (shape, AMBITUS[i], lique)
    if copy_existing_glyph(glyph_name):
        return
    # with a deminutus it is much more beautiful than with a idebilis
    paste_glyph("deminutus")
    draw_line(i, get_width('deminutus')-get_width('line2'), FONT_CONFIG['base height'])
    simplify()
    paste_glyph("PunctumLineBL", get_width('deminutus')-get_width('line2'),
                   i*FONT_CONFIG['base height'])
    set_width(get_width('deminutus')+get_width('PunctumLineBL')-get_width('line2'))
    end_glyph(glyph_name)

def write_all_pes_deminutus(first_glyph, shape, lique=L_NOTHING,
        i_range=ALL_AMBITUS):
    for i in i_range:
        write_pes_deminutus(i, first_glyph, shape, lique)

def write_pes_deminutus(i, first_glyph, shape, lique=L_NOTHING):
    "Writes the pes deminutus glyphs."
    new_glyph()
    glyph_name = '%s%s%s' % (shape, AMBITUS[i], lique)
    if copy_existing_glyph(glyph_name):
        return
    paste_glyph(first_glyph)
    temp_width = get_width(first_glyph)-get_width('line2')
    draw_line(i, temp_width, FONT_CONFIG['base height'])
    paste_glyph("rdeminutus",
                   get_width(first_glyph)-get_width('rdeminutus'),
                   i*FONT_CONFIG['base height'])
    set_width(get_width(first_glyph))
    end_glyph(glyph_name)

def write_all_pes_debilis_deminutus(shape, lique=L_NOTHING, i_range=ALL_AMBITUS):
    for i in i_range:
        write_pes_debilis_deminutus(i, shape, lique)

def write_pes_debilis_deminutus(i, shape, lique=L_NOTHING):
    "Writes the pes debilis deminutus glyphs."
    new_glyph()
    glyph_name = '%s%s%s' % (shape, AMBITUS[i], lique)
    if copy_existing_glyph(glyph_name):
        return
    paste_glyph("deminutus")
    draw_line(i, get_width('deminutus')-get_width('line2'), FONT_CONFIG['base height'])
    simplify()
    paste_glyph("rdeminutus", 0, i*FONT_CONFIG['base height'])
    set_width(get_width('deminutus'))
    end_glyph(glyph_name)

def pes_quadratum():
    "Makes the pes quadratum."
    message("pes quadratum")
    precise_message("pes quadratum")
    write_all_pes_quadratum("PunctumLineTR", "VirgaBaseLineBL", S_PES_QUADRATUM,
            stemshape=S_PES_QUADRATUM, qtype='short')
    write_all_pes_quadratum("PunctumLineTR", "VirgaBaseLineBL",
            S_PES_QUADRATUM_LONGQUEUE, stemshape=S_PES_QUADRATUM, qtype='long')
    write_pes_quadratum(1, "PunctumLineTR", "VirgaBaseLineBL",
            S_PES_QUADRATUM_OPENQUEUE, stemshape=S_PES_QUADRATUM, qtype='open')
    write_all_pes_quadratum("idebilis", "VirgaBaseLineBL", S_PES_QUADRATUM,
            lique=L_INITIO_DEBILIS, stemshape=S_PES_QUADRATUM, qtype='short')
    write_all_pes_quadratum("idebilis", "VirgaBaseLineBL",
            S_PES_QUADRATUM_LONGQUEUE, lique=L_INITIO_DEBILIS,
            stemshape=S_PES_QUADRATUM, qtype='long')
    write_pes_quadratum(1, "idebilis", "VirgaBaseLineBL",
            S_PES_QUADRATUM_OPENQUEUE, lique=L_INITIO_DEBILIS,
            stemshape=S_PES_QUADRATUM, qtype='open')
    precise_message("pes quassus")
    write_all_pes_quadratum("AscendensOriscusLineTR", "VirgaBaseLineBL",
            S_PES_QUASSUS, stemshape=S_PES_QUASSUS, qtype='short')
    write_all_pes_quadratum("AscendensOriscusLineTR", "VirgaBaseLineBL",
            S_PES_QUASSUS_LONGQUEUE, stemshape=S_PES_QUASSUS, qtype='long')
    write_pes_quadratum(1, "AscendensOriscusLineTR", "VirgaBaseLineBL",
            S_PES_QUASSUS_OPENQUEUE, stemshape=S_PES_QUASSUS, qtype='open')
    write_all_pes_quadratum("DescendensOriscusLineTR", "VirgaBaseLineBL",
            S_PES_QUASSUS_INUSITATUS, stemshape=S_PES_QUASSUS, qtype='short')
    write_all_pes_quadratum("DescendensOriscusLineTR", "VirgaBaseLineBL",
            S_PES_QUASSUS_INUSITATUS_LONGQUEUE, stemshape=S_PES_QUASSUS,
            qtype='long')
    write_pes_quadratum(1, "DescendensOriscusLineTR", "VirgaBaseLineBL",
            S_PES_QUASSUS_INUSITATUS_OPENQUEUE, stemshape=S_PES_QUASSUS,
            qtype='open')
    precise_message("pes quilisma quadratum")
    write_all_pes_quadratum("QuilismaLineTR", "VirgaBaseLineBL",
            S_QUILISMA_PES_QUADRATUM, stemshape=S_QUILISMA_PES_QUADRATUM,
            qtype='short')
    write_all_pes_quadratum("QuilismaLineTR", "VirgaBaseLineBL",
            S_QUILISMA_PES_QUADRATUM_LONGQUEUE, stemshape=S_QUILISMA_PES_QUADRATUM,
            qtype='long')
    write_pes_quadratum(1, "QuilismaLineTR", "VirgaBaseLineBL",
            S_QUILISMA_PES_QUADRATUM_OPENQUEUE,
            stemshape=S_QUILISMA_PES_QUADRATUM, qtype='open')
    precise_message("pes auctus ascendens")
    write_all_pes_quadratum("PunctumLineTR", "auctusa2", S_PES_QUADRATUM,
            L_ASCENDENS)
    precise_message("pes initio debilis auctus ascendens")
    write_all_pes_quadratum("idebilis", "auctusa2", S_PES_QUADRATUM,
            L_INITIO_DEBILIS_ASCENDENS)
    precise_message("pes quassus auctus ascendens")
    write_all_pes_quadratum("AscendensOriscusLineTR", "auctusa2", S_PES_QUASSUS,
            L_ASCENDENS)
    write_all_pes_quadratum("DescendensOriscusLineTR", "auctusa2",
            S_PES_QUASSUS_INUSITATUS, L_ASCENDENS)
    precise_message("pes quilisma auctus ascendens")
    write_all_pes_quadratum("QuilismaLineTR", "auctusa2",
            S_QUILISMA_PES_QUADRATUM, L_ASCENDENS)
    precise_message("pes auctus descendens")
    write_all_pes_quadratum("PunctumLineTR", "PunctumAuctusLineBL",
            S_PES_QUADRATUM, L_DESCENDENS)
    precise_message("pes initio debilis auctus descendens")
    write_all_pes_quadratum("idebilis", "PunctumAuctusLineBL", S_PES_QUADRATUM,
            L_INITIO_DEBILIS_DESCENDENS)
    precise_message("pes quassus auctus descendens")
    write_all_pes_quadratum("AscendensOriscusLineTR", "PunctumAuctusLineBL",
            S_PES_QUASSUS, L_DESCENDENS)
    write_all_pes_quadratum("DescendensOriscusLineTR", "PunctumAuctusLineBL",
            S_PES_QUASSUS_INUSITATUS, L_DESCENDENS)
    precise_message("pes quilisma auctus descendens")
    write_all_pes_quadratum("QuilismaLineTR", "PunctumAuctusLineBL",
            S_QUILISMA_PES_QUADRATUM, L_DESCENDENS)

def fusion_pes_quadratum():
    "Makes the fusion pes quadratum."
    message("fusion pes quadratum")
    precise_message("fusion pes quadratum")
    write_all_pes_quadratum("msdeminutus", "VirgaBaseLineBL",
            S_UPPER_PES_QUADRATUM, stemshape=S_UPPER_PES_QUADRATUM,
            qtype='short')
    write_all_pes_quadratum("PunctumLineTLTR", "VirgaBaseLineBL", S_LOWER_PES_QUADRATUM,
            stemshape=S_LOWER_PES_QUADRATUM, qtype='short')
    write_all_pes_quadratum("msdeminutus", "VirgaBaseLineBL",
            S_UPPER_PES_QUADRATUM_LONGQUEUE, stemshape=S_UPPER_PES_QUADRATUM,
            qtype='long')
    write_pes_quadratum(1, "msdeminutus", "VirgaBaseLineBL",
            S_UPPER_PES_QUADRATUM_OPENQUEUE, stemshape=S_UPPER_PES_QUADRATUM,
            qtype='open')
    write_all_pes_quadratum("PunctumLineTLTR", "VirgaBaseLineBL",
            S_LOWER_PES_QUADRATUM_LONGQUEUE, stemshape=S_LOWER_PES_QUADRATUM,
            qtype='long')
    write_pes_quadratum(1, "PunctumLineTLTR", "VirgaBaseLineBL",
            S_LOWER_PES_QUADRATUM_OPENQUEUE, stemshape=S_LOWER_PES_QUADRATUM,
            qtype='open')
    precise_message("fusion pes quassus")
    write_all_pes_quadratum("AscendensOriscusLineBLTR", "VirgaBaseLineBL",
            S_UPPER_PES_QUASSUS, stemshape=S_UPPER_PES_QUASSUS, qtype='short')
    write_all_pes_quadratum("AscendensOriscusLineBLTR", "VirgaBaseLineBL",
            S_UPPER_PES_QUASSUS_LONGQUEUE, stemshape=S_UPPER_PES_QUASSUS,
            qtype='long')
    write_pes_quadratum(1, "AscendensOriscusLineBLTR", "VirgaBaseLineBL",
            S_UPPER_PES_QUASSUS_OPENQUEUE, stemshape=S_UPPER_PES_QUASSUS,
            qtype='open')
    write_all_pes_quadratum(FLATTENED_ORISCUS["AscendensOriscusLineTRFlatBottom"],
            "VirgaBaseLineBL", S_UPPER_OBLATUS_PES_QUASSUS,
            stemshape=S_UPPER_OBLATUS_PES_QUASSUS, qtype='short')
    write_all_pes_quadratum(FLATTENED_ORISCUS["AscendensOriscusLineTRFlatBottom"],
            "VirgaBaseLineBL", S_UPPER_OBLATUS_PES_QUASSUS_LONGQUEUE,
            stemshape=S_UPPER_OBLATUS_PES_QUASSUS, qtype='long')
    write_pes_quadratum(1, FLATTENED_ORISCUS["AscendensOriscusLineTRFlatBottom"],
            "VirgaBaseLineBL", S_UPPER_OBLATUS_PES_QUASSUS_OPENQUEUE,
            stemshape=S_UPPER_OBLATUS_PES_QUASSUS, qtype='open')
    write_all_pes_quadratum("DescendensOriscusLineBLTR", "VirgaBaseLineBL",
            S_UPPER_PES_QUASSUS_INUSITATUS, stemshape=S_UPPER_PES_QUASSUS,
            qtype='short')
    write_all_pes_quadratum("DescendensOriscusLineBLTR", "VirgaBaseLineBL",
            S_UPPER_PES_QUASSUS_INUSITATUS_LONGQUEUE,
            stemshape=S_UPPER_PES_QUASSUS, qtype='long')
    write_pes_quadratum(1, "DescendensOriscusLineBLTR", "VirgaBaseLineBL",
            S_UPPER_PES_QUASSUS_INUSITATUS_OPENQUEUE,
            stemshape=S_UPPER_PES_QUASSUS, qtype='open')
    write_all_pes_quadratum("AscendensOriscusLineTLTR", "VirgaBaseLineBL",
            S_LOWER_PES_QUASSUS, stemshape=S_LOWER_PES_QUASSUS, qtype='short')
    write_all_pes_quadratum("AscendensOriscusLineTLTR", "VirgaBaseLineBL",
            S_LOWER_PES_QUASSUS_LONGQUEUE, stemshape=S_LOWER_PES_QUASSUS,
            qtype='long')
    write_pes_quadratum(1, "AscendensOriscusLineTLTR", "VirgaBaseLineBL",
            S_LOWER_PES_QUASSUS_OPENQUEUE, stemshape=S_LOWER_PES_QUASSUS,
            qtype='open')
    write_all_pes_quadratum("DescendensOriscusLineTLTR", "VirgaBaseLineBL",
            S_LOWER_PES_QUASSUS_INUSITATUS, stemshape=S_LOWER_PES_QUASSUS,
            qtype='short')
    write_all_pes_quadratum(FLATTENED_ORISCUS["DescendensOriscusLineTRFlatTop"],
            "VirgaBaseLineBL", S_LOWER_OBLATUS_PES_QUASSUS_INUSITATUS,
            stemshape=S_LOWER_PES_QUASSUS, qtype='short')
    write_all_pes_quadratum("DescendensOriscusLineTLTR", "VirgaBaseLineBL",
            S_LOWER_PES_QUASSUS_INUSITATUS_LONGQUEUE,
            stemshape=S_LOWER_PES_QUASSUS, qtype='long')
    write_all_pes_quadratum(FLATTENED_ORISCUS["DescendensOriscusLineTRFlatTop"],
            "VirgaBaseLineBL", S_LOWER_OBLATUS_PES_QUASSUS_INUSITATUS_LONGQUEUE,
            stemshape=S_LOWER_PES_QUASSUS, qtype='long')
    write_pes_quadratum(1, "DescendensOriscusLineTLTR", "VirgaBaseLineBL",
            S_LOWER_PES_QUASSUS_INUSITATUS_OPENQUEUE,
            stemshape=S_LOWER_PES_QUASSUS, qtype='open')
    write_pes_quadratum(1, FLATTENED_ORISCUS["DescendensOriscusLineTRFlatTop"],
            "VirgaBaseLineBL", S_LOWER_OBLATUS_PES_QUASSUS_INUSITATUS_OPENQUEUE,
            stemshape=S_LOWER_PES_QUASSUS, qtype='open')
    precise_message("fusion pes auctus ascendens")
    write_all_pes_quadratum("msdeminutus", "auctusa2", S_UPPER_PES_QUADRATUM,
            L_ASCENDENS)
    write_all_pes_quadratum("PunctumLineTLTR", "auctusa2", S_LOWER_PES_QUADRATUM,
            L_ASCENDENS)
    precise_message("fusion pes quassus auctus ascendens")
    write_all_pes_quadratum("AscendensOriscusLineBLTR", "auctusa2",
            S_UPPER_PES_QUASSUS, L_ASCENDENS)
    write_all_pes_quadratum("DescendensOriscusLineBLTR", "auctusa2",
            S_UPPER_PES_QUASSUS_INUSITATUS, L_ASCENDENS)
    write_all_pes_quadratum(FLATTENED_ORISCUS["AscendensOriscusLineTRFlatBottom"],
            "auctusa2", S_UPPER_OBLATUS_PES_QUASSUS, L_ASCENDENS)
    write_all_pes_quadratum("AscendensOriscusLineTLTR", "auctusa2",
            S_LOWER_PES_QUASSUS, L_ASCENDENS)
    write_all_pes_quadratum("DescendensOriscusLineTLTR", "auctusa2",
            S_LOWER_PES_QUASSUS_INUSITATUS, L_ASCENDENS)
    write_all_pes_quadratum(FLATTENED_ORISCUS["DescendensOriscusLineTRFlatTop"],
            "auctusa2", S_LOWER_OBLATUS_PES_QUASSUS_INUSITATUS, L_ASCENDENS)
    precise_message("fusion pes auctus descendens")
    write_all_pes_quadratum("msdeminutus", "PunctumAuctusLineBL",
            S_UPPER_PES_QUADRATUM, L_DESCENDENS)
    write_all_pes_quadratum("PunctumLineTLTR", "PunctumAuctusLineBL",
            S_LOWER_PES_QUADRATUM, L_DESCENDENS)
    precise_message("fusion pes quassus auctus descendens")
    write_all_pes_quadratum("AscendensOriscusLineBLTR", "PunctumAuctusLineBL",
            S_UPPER_PES_QUASSUS, L_DESCENDENS)
    write_all_pes_quadratum(FLATTENED_ORISCUS["AscendensOriscusLineTRFlatBottom"],
            "PunctumAuctusLineBL", S_UPPER_OBLATUS_PES_QUASSUS, L_DESCENDENS)
    write_all_pes_quadratum("DescendensOriscusLineBLTR", "PunctumAuctusLineBL",
            S_UPPER_PES_QUASSUS_INUSITATUS, L_DESCENDENS)
    write_all_pes_quadratum("AscendensOriscusLineTLTR", "PunctumAuctusLineBL",
            S_LOWER_PES_QUASSUS, L_DESCENDENS)
    write_all_pes_quadratum("DescendensOriscusLineTLTR", "PunctumAuctusLineBL",
            S_LOWER_PES_QUASSUS_INUSITATUS, L_DESCENDENS)
    write_all_pes_quadratum(FLATTENED_ORISCUS["DescendensOriscusLineTRFlatTop"],
            "PunctumAuctusLineBL", S_LOWER_OBLATUS_PES_QUASSUS_INUSITATUS,
            L_DESCENDENS)

def write_all_pes_quadratum(first_glyph, last_glyph, shape, lique=L_NOTHING,
        stemshape=None, qtype=None, i_range=ALL_AMBITUS):
    for i in i_range:
        write_pes_quadratum(i, first_glyph, last_glyph, shape, lique, stemshape,
                qtype)

def write_pes_quadratum(i, first_glyph, last_glyph, shape, lique=L_NOTHING,
        stemshape=None, qtype=None):
    "Writes the pes quadratum glyphs."
    new_glyph()
    glyph_name = '%s%s%s' % (shape, AMBITUS[i], lique)
    if copy_existing_glyph(glyph_name):
        return
    if i == 1 and first_glyph != 'idebilis' and first_glyph != 'QuilismaLineTR':
        if first_glyph == 'PunctumLineTR':
            first_glyph = 'Punctum'
        elif first_glyph == 'msdeminutus':
            first_glyph = 'PunctumLineBL'
        elif first_glyph == 'PunctumLineTLTR':
            first_glyph = 'PunctumLineTL'
        elif 'OriscusLine' in first_glyph:
            if first_glyph.endswith('LineTRFlatTop'):
                first_glyph = FLATTENED_ORISCUS[first_glyph[:-13] + 'Flattened']
            elif first_glyph.endswith('LineTRFlatBottom'):
                first_glyph = FLATTENED_ORISCUS[first_glyph[:-16] + 'Flattened']
            elif first_glyph.endswith('LineTR'):
                first_glyph = FLATTENED_ORISCUS[first_glyph[:-6] + 'FlatTop']
            else:
                first_glyph = FLATTENED_ORISCUS[first_glyph[:-2] + 'FlatTop']

        if last_glyph == 'PunctumLineTL':
            last_glyph = 'Punctum'
        elif last_glyph == 'auctusa2':
            last_glyph = 'PunctumAscendens'
        elif last_glyph == 'PunctumAuctusLineBL':
            last_glyph = 'PunctumDescendens'
        elif last_glyph == 'VirgaBaseLineBL':
            last_glyph = 'virgabase'

        first_width = get_width(first_glyph)
    else:
        first_width = get_width(first_glyph)-get_width('line2')
    paste_glyph(first_glyph)
    if i != 1:
        draw_line(i, first_width, FONT_CONFIG['base height'])
    paste_glyph(last_glyph, first_width, i*FONT_CONFIG['base height'])
    width = first_width+get_width(last_glyph)
    if qtype:
        draw_right_queue(i, width-get_width('line2'), qtype, stemshape, lique,
                          shift=i*FONT_CONFIG['base height'])
    set_width(width)
    end_glyph(glyph_name)

def pes_oriscus():
    "Creates the pes oriscus."
    precise_message("pes oriscus")
    write_all_pes_oriscus("PunctumLineTR", "AscendensOriscusLineBL",
            S_PES_ASCENDENS_ORISCUS)
    write_all_pes_oriscus("PunctumLineTR", "DescendensOriscusLineBL",
            S_PES_DESCENDENS_ORISCUS)

def write_all_pes_oriscus(first_glyph, last_glyph, shape, lique=L_NOTHING,
        i_range=ALL_AMBITUS):
    for i in i_range:
        write_pes_oriscus(i, first_glyph, last_glyph, shape, lique)

def write_pes_oriscus(i, first_glyph, last_glyph, shape, lique=L_NOTHING):
    "Writes the pes oriscus glyphs."
    new_glyph()
    glyph_name = '%s%s%s' % (shape, AMBITUS[i], lique)
    if copy_existing_glyph(glyph_name):
        return
    if i == 1:
        first_glyph = 'Punctum'
        first_width = get_width(first_glyph)
        if last_glyph == 'AscendensOriscusLineBL':
            last_glyph = FLATTENED_ORISCUS['AscendensOriscusFlatBottom']
        elif last_glyph == 'DescendensOriscusLineBL':
            last_glyph = FLATTENED_ORISCUS['DescendensOriscusFlatBottom']
    else:
        first_width = get_width(first_glyph)-get_width('line2')
    paste_glyph(first_glyph)
    if i != 1:
        draw_line(i, first_width, FONT_CONFIG['base height'])
    paste_glyph(last_glyph, first_width, i*FONT_CONFIG['base height'])
    set_width(first_width+get_width(last_glyph))
    end_glyph(glyph_name)

def salicus():
    "Creates the salicus."
    message("salicus")
    write_all_salicus("VirgaBaseLineBL", S_SALICUS, qtype='short')
    write_all_salicus("VirgaBaseLineBL", S_SALICUS_LONGQUEUE, qtype='long')
    write_all_salicus("rdeminutus", S_SALICUS, L_DEMINUTUS)
    write_all_salicus("auctusa2", S_SALICUS, L_ASCENDENS)
    write_all_salicus("PunctumAuctusLineBL", S_SALICUS, L_DESCENDENS)

def write_all_salicus(last_glyph, shape, lique=L_NOTHING, qtype=None,
        i_range=ALL_AMBITUS, j_range=ALL_AMBITUS):
    for i in i_range:
        for j in j_range:
            write_salicus(i, j, last_glyph, shape, lique, qtype)

def write_salicus(i, j, last_glyph, shape, lique=L_NOTHING, qtype=None):
    "Writes the salicus glyphs."
    new_glyph()
    glyph_name = '%s%s%s%s' % (shape, AMBITUS[i], AMBITUS[j], lique)
    if copy_existing_glyph(glyph_name):
        return
    length = draw_salicus(i, j, last_glyph, lique, qtype)
    set_width(length)
    end_glyph(glyph_name)

def draw_salicus(i, j, last_glyph, lique=L_NOTHING, qtype=None):
    "Draw a salicus in current glyph."
    deminutus = last_glyph == 'rdeminutus'
    no_third_glyph = False
    if j == 1 and not deminutus and last_glyph == 'VirgaBaseLineBL':
        last_glyph = 'virgabase'
    if i == 1 and j == 1 and not deminutus:
        first_glyph = 'Punctum'
        first_width = get_width(first_glyph)
        middle_glyph = FLATTENED_ORISCUS['AscendensOriscusFlattened']
        middle_width = get_width(middle_glyph)
    elif i == 1 and (not deminutus or not glyph_exists('PesQuassusOneDeminutus')):
        first_glyph = 'Punctum'
        first_width = get_width(first_glyph)
        middle_glyph = FLATTENED_ORISCUS['AscendensOriscusLineTRFlatBottom']
        middle_width = get_width(middle_glyph)-get_width('line2')
    elif j == 1 and not deminutus:
        first_glyph = 'PunctumLineTR'
        first_width = get_width(first_glyph)-get_width('line2')
        middle_glyph = FLATTENED_ORISCUS['AscendensOriscusLineBLFlatTop']
        middle_width = get_width(middle_glyph)
    elif (j == 1 and deminutus and glyph_exists('PesQuassusOneDeminutus') and
          glyph_exists('UpperPesQuassusOneDeminutus')):
        if i == 1:
            first_glyph = 'Punctum'
            first_width = get_width(first_glyph)
            middle_glyph = 'PesQuassusOneDeminutus'
            middle_width = get_width(middle_glyph)
        else:
            first_glyph = 'PunctumLineTR'
            first_width = get_width(first_glyph)-get_width('line2')
            middle_glyph = 'UpperPesQuassusOneDeminutus'
            middle_width = get_width(middle_glyph)
        no_third_glyph = True
    else:
        first_glyph = 'PunctumLineTR'
        first_width = get_width(first_glyph)-get_width('line2')
        middle_glyph = 'AscendensOriscusLineBLTR'
        middle_width = get_width(middle_glyph)-get_width('line2')
    paste_glyph(first_glyph)
    if i != 1:
        draw_line(i, first_width, FONT_CONFIG['base height'])
    paste_glyph(middle_glyph, first_width, i*FONT_CONFIG['base height'])
    length = first_width+middle_width
    if not no_third_glyph:
        if j != 1:
            draw_line(j, length, (i+1)*FONT_CONFIG['base height'])
        elif not deminutus:
            length = length-0.01
            if last_glyph == 'auctusa2':
                last_glyph = 'PunctumAscendens'
            elif last_glyph == 'PunctumAuctusLineBL':
                last_glyph = 'PunctumDescendens'
            elif last_glyph == 'VirgaBaseLineBL':
                last_glyph = 'virgabase'
            elif last_glyph == 'PunctumLineBLBR':
                last_glyph = 'PunctumLineBR'
            elif last_glyph == 'PunctumLineBL':
                last_glyph = 'Punctum'
        if not last_glyph:
            return length
        if not deminutus:
            paste_glyph(last_glyph, length, (i+j)*FONT_CONFIG['base height'])
            length = length + get_width(last_glyph)
            if qtype:
                draw_right_queue(j, length-get_width('line2'), qtype, S_SALICUS,
                                  lique, i, (i+j)*FONT_CONFIG['base height'])
        else:
            length = length+get_width('line2')
            paste_glyph(last_glyph, (length-get_width(last_glyph)),
                           (i+j)*FONT_CONFIG['base height'])
    return length

def salicus_flexus():
    "Creates the salicus flexus."
    message("salicus flexus")
    write_all_salicus_flexus("PunctumLineTL")
    write_all_salicus_flexus("deminutus", L_DEMINUTUS)
    write_all_salicus_flexus("auctusa1", L_ASCENDENS)
    write_all_salicus_flexus("auctusd1", L_DESCENDENS)

def write_all_salicus_flexus(last_glyph, lique=L_NOTHING, i_range=ALL_AMBITUS,
        j_range=ALL_AMBITUS, k_range=ALL_AMBITUS):
    for i in i_range:
        for j in j_range:
            for k in k_range:
                write_salicus_flexus(i, j, k, last_glyph, lique)

def write_salicus_flexus(i, j, k, last_glyph, lique=L_NOTHING):
    "Writes the salicus glyphs."
    new_glyph()
    glyph_name = '%s%s%s%s%s' % (S_SALICUS_FLEXUS, AMBITUS[i], AMBITUS[j],
                                 AMBITUS[k], lique)
    if copy_existing_glyph(glyph_name):
        return
    is_deminutus = last_glyph == 'deminutus'
    if is_deminutus:
        penult_glyph = None
    elif k == 1:
        penult_glyph = 'PunctumLineBL'
    else:
        penult_glyph = 'PunctumLineBLBR'
    length = draw_salicus(i, j, penult_glyph)
    if is_deminutus:
        width_dem = draw_deminutus(j+i, k, length,
                                    firstbar = 0 if j == 1 else 1)
        length = length + width_dem
    else:
        if k == 1 and not is_deminutus:
            length = length-0.01
            if last_glyph == 'PunctumLineTL':
                last_glyph = 'Punctum'
            elif last_glyph == 'auctusa1':
                last_glyph = 'PunctumAscendens'
            elif last_glyph == 'auctusd1':
                last_glyph = 'PunctumDescendens'
        if k != 1:
            length = length - get_width('line2')
            draw_line(k, length, (1+i+j-k)*FONT_CONFIG['base height'])
        paste_glyph(last_glyph, length, (i+j-k)*FONT_CONFIG['base height'])
        length = length + get_width(last_glyph)
    set_width(length)
    end_glyph(glyph_name)

def flexus():
    "Creates the flexus."
    message("flexus")
    precise_message("flexus")
    write_all_flexus("PunctumLineBR", 'PunctumLineTL', S_FLEXUS_NOBAR)
    write_all_flexus("DescendensOriscusLineBR", 'PunctumLineTL',
            S_FLEXUS_ORISCUS)
    write_all_flexus("AscendensOriscusLineBR", 'PunctumLineTL',
            S_FLEXUS_ORISCUS_INUSITATUS)
    write_all_flexus("rvbase", 'PunctumLineTL', S_FLEXUS, qtype='short')
    write_all_flexus("rvbase", 'PunctumLineTL', S_FLEXUS_LONGQUEUE, qtype='long')
    write_flexus(1, "rvbase", 'PunctumLineTL', S_FLEXUS_OPENQUEUE, qtype='open')
    write_all_flexus("DescendensOriscusLineBLBR", 'PunctumLineTL',
            S_FLEXUS_ORISCUS_SCAPUS, stemshape=S_FLEXUS_ORISCUS_SCAPUS,
            qtype='short')
    write_all_flexus("DescendensOriscusLineBLBR", 'PunctumLineTL',
            S_FLEXUS_ORISCUS_SCAPUS_LONGQUEUE,
            stemshape=S_FLEXUS_ORISCUS_SCAPUS, qtype='long')
    write_flexus(1, "DescendensOriscusLineBLBR", 'PunctumLineTL',
            S_FLEXUS_ORISCUS_SCAPUS_OPENQUEUE, stemshape=S_FLEXUS_ORISCUS_SCAPUS,
            qtype='open')
    write_all_flexus("AscendensOriscusLineBLBR", 'PunctumLineTL',
            S_FLEXUS_ORISCUS_SCAPUS_INUSITATUS,
            stemshape=S_FLEXUS_ORISCUS_SCAPUS, qtype='short')
    write_all_flexus("AscendensOriscusLineBLBR", 'PunctumLineTL',
            S_FLEXUS_ORISCUS_SCAPUS_INUSITATUS_LONGQUEUE,
            stemshape=S_FLEXUS_ORISCUS_SCAPUS, qtype='long')
    write_flexus(1, "AscendensOriscusLineBLBR", 'PunctumLineTL',
            S_FLEXUS_ORISCUS_SCAPUS_INUSITATUS_OPENQUEUE,
            stemshape=S_FLEXUS_ORISCUS_SCAPUS, qtype='open')
    precise_message("flexus deminutus")
    write_all_flexus("mdeminutus", 'PunctumLineTL', S_FLEXUS_NOBAR, L_DEMINUTUS)
    write_all_flexus("DescendensOriscusLineBR", 'deminutus', S_FLEXUS_ORISCUS,
            L_DEMINUTUS)
    write_all_flexus("AscendensOriscusLineBR", 'deminutus',
            S_FLEXUS_ORISCUS_INUSITATUS, L_DEMINUTUS)
    write_all_flexus("mdeminutus", 'PunctumLineTL', S_FLEXUS, L_DEMINUTUS,
            qtype='short')
    write_all_flexus("mdeminutus", 'PunctumLineTL', S_FLEXUS_LONGQUEUE,
            L_DEMINUTUS, qtype='long')
    write_flexus(1, "mdeminutus", 'PunctumLineTL', S_FLEXUS_OPENQUEUE,
            L_DEMINUTUS, qtype='open')
    precise_message("flexus auctus ascendens")
    write_all_flexus("PunctumLineBR", 'auctusa1', S_FLEXUS_NOBAR, L_ASCENDENS)
    write_all_flexus("DescendensOriscusLineBR", 'auctusa1', S_FLEXUS_ORISCUS,
            L_ASCENDENS)
    write_all_flexus("AscendensOriscusLineBR", 'auctusa1',
            S_FLEXUS_ORISCUS_INUSITATUS, L_ASCENDENS)
    write_all_flexus("rvbase", 'auctusa1', S_FLEXUS, L_ASCENDENS, qtype='short')
    write_all_flexus("rvbase", 'auctusa1', S_FLEXUS_LONGQUEUE, L_ASCENDENS,
            qtype='long')
    write_flexus(1, "rvbase", 'auctusa1', S_FLEXUS_OPENQUEUE, L_ASCENDENS,
            qtype='open')
    write_all_flexus("DescendensOriscusLineBLBR", 'auctusa1',
            S_FLEXUS_ORISCUS_SCAPUS, L_ASCENDENS,
            stemshape=S_FLEXUS_ORISCUS_SCAPUS, qtype='short')
    write_all_flexus("DescendensOriscusLineBLBR", 'auctusa1',
            S_FLEXUS_ORISCUS_SCAPUS_LONGQUEUE, L_ASCENDENS,
            stemshape=S_FLEXUS_ORISCUS_SCAPUS, qtype='long')
    write_flexus(1, "DescendensOriscusLineBLBR", 'auctusa1',
            S_FLEXUS_ORISCUS_SCAPUS_OPENQUEUE, L_ASCENDENS,
            stemshape=S_FLEXUS_ORISCUS_SCAPUS, qtype='open')
    write_all_flexus("AscendensOriscusLineBLBR", 'auctusa1',
            S_FLEXUS_ORISCUS_SCAPUS_INUSITATUS, L_ASCENDENS,
            stemshape=S_FLEXUS_ORISCUS_SCAPUS, qtype='short')
    write_all_flexus("AscendensOriscusLineBLBR", 'auctusa1',
            S_FLEXUS_ORISCUS_SCAPUS_INUSITATUS_LONGQUEUE, L_ASCENDENS,
            stemshape=S_FLEXUS_ORISCUS_SCAPUS, qtype='long')
    write_flexus(1, "AscendensOriscusLineBLBR", 'auctusa1',
            S_FLEXUS_ORISCUS_SCAPUS_INUSITATUS_OPENQUEUE, L_ASCENDENS,
            stemshape=S_FLEXUS_ORISCUS_SCAPUS, qtype='open')
    precise_message("flexus auctus descendens")
    write_all_flexus("PunctumLineBR", 'auctusd1', S_FLEXUS_NOBAR, L_DESCENDENS)
    write_all_flexus("DescendensOriscusLineBR", 'auctusd1', S_FLEXUS_ORISCUS,
            L_DESCENDENS)
    write_all_flexus("AscendensOriscusLineBR", 'auctusd1',
            S_FLEXUS_ORISCUS_INUSITATUS, L_DESCENDENS)
    write_all_flexus("rvbase", 'auctusd1', S_FLEXUS, L_DESCENDENS, qtype='short')
    write_all_flexus("rvbase", 'auctusd1', S_FLEXUS_LONGQUEUE, L_DESCENDENS,
            qtype='long')
    write_flexus(1, "rvbase", 'auctusd1', S_FLEXUS_OPENQUEUE, L_DESCENDENS,
            qtype='open')
    write_all_flexus("DescendensOriscusLineBLBR", 'auctusd1',
            S_FLEXUS_ORISCUS_SCAPUS, L_DESCENDENS, S_FLEXUS_ORISCUS_SCAPUS,
            stemshape=S_FLEXUS_ORISCUS_SCAPUS, qtype='short')
    write_all_flexus("DescendensOriscusLineBLBR", 'auctusd1',
            S_FLEXUS_ORISCUS_SCAPUS_LONGQUEUE, L_DESCENDENS,
            S_FLEXUS_ORISCUS_SCAPUS, stemshape=S_FLEXUS_ORISCUS_SCAPUS,
            qtype='long')
    write_flexus(1, "DescendensOriscusLineBLBR", 'auctusd1',
            S_FLEXUS_ORISCUS_SCAPUS_OPENQUEUE, L_DESCENDENS,
            S_FLEXUS_ORISCUS_SCAPUS, stemshape=S_FLEXUS_ORISCUS_SCAPUS,
            qtype='open')
    write_all_flexus("AscendensOriscusLineBLBR", 'auctusd1',
            S_FLEXUS_ORISCUS_SCAPUS_INUSITATUS, L_DESCENDENS,
            S_FLEXUS_ORISCUS_SCAPUS, stemshape=S_FLEXUS_ORISCUS_SCAPUS,
            qtype='short')
    write_all_flexus("AscendensOriscusLineBLBR", 'auctusd1',
            S_FLEXUS_ORISCUS_SCAPUS_INUSITATUS_LONGQUEUE, L_DESCENDENS,
            S_FLEXUS_ORISCUS_SCAPUS, stemshape=S_FLEXUS_ORISCUS_SCAPUS,
            qtype='long')
    write_flexus(1, "AscendensOriscusLineBLBR", 'auctusd1',
            S_FLEXUS_ORISCUS_SCAPUS_INUSITATUS_OPENQUEUE, L_DESCENDENS,
            S_FLEXUS_ORISCUS_SCAPUS, stemshape=S_FLEXUS_ORISCUS_SCAPUS,
            qtype='open')

def fusion_flexus():
    "Creates the fusion flexus."
    message("fusion flexus")
    precise_message("fusion flexus")
    write_all_flexus("mademinutus", 'PunctumLineTL', S_LOWER_FLEXUS)
    write_all_flexus("rvbase", 'PunctumLineTL', S_UPPER_FLEXUS)
    write_all_flexus("DescendensOriscusLineBLBR", 'PunctumLineTL',
            S_UPPER_FLEXUS_ORISCUS)
    write_all_flexus("DescendensOriscusLineTLBR", 'PunctumLineTL',
            S_LOWER_FLEXUS_ORISCUS)
    write_all_flexus(FLATTENED_ORISCUS["DescendensOriscusLineBRFlatTop"],
            'PunctumLineTL', S_LOWER_OBLATUS_FLEXUS_ORISCUS)
    write_all_flexus("AscendensOriscusLineBLBR", 'PunctumLineTL',
            S_UPPER_FLEXUS_ORISCUS_INUSITATUS)
    write_all_flexus(FLATTENED_ORISCUS["AscendensOriscusLineBRFlatBottom"],
            'PunctumLineTL', S_UPPER_OBLATUS_FLEXUS_ORISCUS_INUSITATUS)
    write_all_flexus("AscendensOriscusLineTLBR", 'PunctumLineTL',
            S_LOWER_FLEXUS_ORISCUS_INUSITATUS)
    precise_message("fusion flexus deminutus")
    write_all_flexus("mademinutus", 'deminutus', S_LOWER_FLEXUS, L_DEMINUTUS)
    write_all_flexus("mdeminutus", 'deminutus', S_UPPER_FLEXUS, L_DEMINUTUS)
    write_all_flexus("DescendensOriscusLineBLBR", 'deminutus',
            S_UPPER_FLEXUS_ORISCUS, L_DEMINUTUS)
    write_all_flexus("DescendensOriscusLineTLBR", 'deminutus',
            S_LOWER_FLEXUS_ORISCUS, L_DEMINUTUS)
    write_all_flexus(FLATTENED_ORISCUS["DescendensOriscusLineBRFlatTop"],
            'deminutus', S_LOWER_OBLATUS_FLEXUS_ORISCUS, L_DEMINUTUS)
    write_all_flexus("AscendensOriscusLineBLBR", 'deminutus',
            S_UPPER_FLEXUS_ORISCUS_INUSITATUS, L_DEMINUTUS)
    write_all_flexus(FLATTENED_ORISCUS["AscendensOriscusLineBRFlatBottom"],
            'deminutus', S_UPPER_OBLATUS_FLEXUS_ORISCUS_INUSITATUS, L_DEMINUTUS)
    write_all_flexus("AscendensOriscusLineTLBR", 'deminutus',
            S_LOWER_FLEXUS_ORISCUS_INUSITATUS, L_DEMINUTUS)
    precise_message("fusion flexus auctus ascendens")
    write_all_flexus("mademinutus", 'auctusa1', S_LOWER_FLEXUS, L_ASCENDENS)
    write_all_flexus("rvbase", 'auctusa1', S_UPPER_FLEXUS, L_ASCENDENS)
    write_all_flexus("DescendensOriscusLineBLBR", 'auctusa1',
            S_UPPER_FLEXUS_ORISCUS, L_ASCENDENS)
    write_all_flexus("DescendensOriscusLineTLBR", 'auctusa1',
            S_LOWER_FLEXUS_ORISCUS, L_ASCENDENS)
    write_all_flexus(FLATTENED_ORISCUS["DescendensOriscusLineBRFlatTop"],
            'auctusa1', S_LOWER_OBLATUS_FLEXUS_ORISCUS, L_ASCENDENS)
    write_all_flexus("AscendensOriscusLineBLBR", 'auctusa1',
            S_UPPER_FLEXUS_ORISCUS_INUSITATUS, L_ASCENDENS)
    write_all_flexus(FLATTENED_ORISCUS["AscendensOriscusLineBRFlatBottom"],
            'auctusa1', S_UPPER_OBLATUS_FLEXUS_ORISCUS_INUSITATUS, L_ASCENDENS)
    write_all_flexus("AscendensOriscusLineTLBR", 'auctusa1',
            S_LOWER_FLEXUS_ORISCUS_INUSITATUS, L_ASCENDENS)
    precise_message("fusion flexus auctus descendens")
    write_all_flexus("mademinutus", 'auctusd1', S_LOWER_FLEXUS, L_DESCENDENS)
    write_all_flexus("rvbase", 'auctusd1', S_UPPER_FLEXUS, L_DESCENDENS)
    write_all_flexus("DescendensOriscusLineBLBR", 'auctusd1',
            S_UPPER_FLEXUS_ORISCUS, L_DESCENDENS)
    write_all_flexus("DescendensOriscusLineTLBR", 'auctusd1',
            S_LOWER_FLEXUS_ORISCUS, L_DESCENDENS)
    write_all_flexus(FLATTENED_ORISCUS["DescendensOriscusLineBRFlatTop"],
            'auctusd1', S_LOWER_OBLATUS_FLEXUS_ORISCUS, L_DESCENDENS)
    write_all_flexus("AscendensOriscusLineBLBR", 'auctusd1',
            S_UPPER_FLEXUS_ORISCUS_INUSITATUS, L_DESCENDENS)
    write_all_flexus(FLATTENED_ORISCUS["AscendensOriscusLineBRFlatBottom"],
            'auctusd1', S_UPPER_OBLATUS_FLEXUS_ORISCUS_INUSITATUS, L_DESCENDENS)
    write_all_flexus("AscendensOriscusLineTLBR", 'auctusd1',
            S_LOWER_FLEXUS_ORISCUS_INUSITATUS, L_DESCENDENS)

def write_all_flexus(first_glyph, last_glyph, shape, lique=L_NOTHING,
        firstglyph_amone=None, lastglyph_amone=None, stemshape=S_FLEXUS,
        qtype=None, i_range=ALL_AMBITUS):
    for i in i_range:
        write_flexus(i, first_glyph, last_glyph, shape, lique, firstglyph_amone,
                lastglyph_amone, stemshape, qtype)

def write_flexus(i, first_glyph, last_glyph, shape, lique=L_NOTHING,
        firstglyph_amone=None, lastglyph_amone=None, stemshape=S_FLEXUS,
        qtype=None):
    # pylint: disable=too-many-statements
    "Writes the flexus glyphs."
    new_glyph()
    glyph_name = '%s%s%s' % (shape, AMBITUS[i], lique)
    if copy_existing_glyph(glyph_name):
        return
    # we add a queue if it is a deminutus
    if first_glyph == "mdeminutus" and shape != S_UPPER_FLEXUS:
        if shape == S_FLEXUS_NOBAR:
            length = draw_deminutus(0, i, length=0, tosimplify=1, firstbar=0)
        else:
            draw_left_queue(i, qtype, stemshape, lique)
            length = draw_deminutus(0, i, length=0, tosimplify=1, firstbar=1)
    elif last_glyph == 'deminutus' and (shape == S_UPPER_FLEXUS or shape == S_LOWER_FLEXUS):
        firstbar = 1 if first_glyph == 'mdeminutus' else 2
        length = draw_deminutus(0, i, length=0, tosimplify=1, firstbar=firstbar)
    elif last_glyph == 'deminutus':
        paste_glyph(first_glyph)
        draw_line(i, get_width(first_glyph) - get_width('line2'),
                   (1-i)*FONT_CONFIG['base height'])
        simplify()
        paste_glyph("deminutus",
                       get_width(first_glyph) -
                       get_width(last_glyph), (-i)*FONT_CONFIG['base height'])
        length = get_width(first_glyph)
    else:
        if qtype:
            draw_left_queue(i, qtype, stemshape, lique)
        if i == 1 and first_glyph != 'DescendensOriscusLineTLBR':
            if last_glyph == 'PunctumLineTL':
                last_glyph = 'Punctum'
            elif last_glyph == 'auctusa1':
                last_glyph = 'PunctumAscendens'
            elif last_glyph == 'auctusd1':
                last_glyph = 'PunctumDescendens'

            if first_glyph == 'PunctumLineBR':
                first_glyph = 'Punctum'
            elif 'OriscusLine' in first_glyph:
                if first_glyph.endswith('LineBRFlatBottom'):
                    first_glyph = FLATTENED_ORISCUS[first_glyph[:-16] + 'Flattened']
                elif first_glyph.endswith('LineBRFlatTop'):
                    first_glyph = FLATTENED_ORISCUS[first_glyph[:-13] + 'Flattened']
                elif first_glyph.endswith('LineBR'):
                    first_glyph = FLATTENED_ORISCUS[first_glyph[:-6] + 'FlatBottom']
                else:
                    first_glyph = FLATTENED_ORISCUS[first_glyph[:-2] + 'FlatBottom']
            elif first_glyph == 'VirgaLineBR':
                first_glyph = 'VirgaReversa'
            elif first_glyph == 'rvbase':
                first_glyph = 'rvirgabase'
            elif first_glyph == 'mademinutus':
                first_glyph = 'PunctumLineTL'
            elif first_glyph == 'PunctumLineBLBR':
                first_glyph = 'PunctumLineBL'

            length = get_width(first_glyph)
        else:
            length = get_width(first_glyph)-get_width('line2')
        paste_glyph(first_glyph)
        if i != 1:
            draw_line(i, length, (1-i)*FONT_CONFIG['base height'])
        paste_glyph(last_glyph, length, (-i)*FONT_CONFIG['base height'])
        length = length + get_width(last_glyph)
    set_width(length)
    end_glyph(glyph_name)

def porrectus():
    "Creates the porrectus."
    message("porrectus")
    precise_message("porrectus")
    write_all_porrectus('PunctumSmall', S_PORRECTUS, qtype='short')
    write_all_porrectus('PunctumSmall', S_PORRECTUS_LONGQUEUE, qtype='long',
            i_range=AMBITUS_ONE_ONLY)
    write_all_porrectus('PunctumSmall', S_PORRECTUS_NOBAR)
    precise_message("porrectus auctus ascendens")
    write_all_porrectus("auctusa2", S_PORRECTUS, L_ASCENDENS, qtype='short')
    write_all_porrectus("auctusa2", S_PORRECTUS_LONGQUEUE, L_ASCENDENS,
            qtype='long', i_range=AMBITUS_ONE_ONLY)
    write_all_porrectus("auctusa2", S_PORRECTUS_NOBAR, L_ASCENDENS)
    precise_message("porrectus auctus descendens")
    write_all_porrectus("PunctumAuctusLineBL", S_PORRECTUS, L_DESCENDENS,
            qtype='short')
    write_all_porrectus("PunctumAuctusLineBL", S_PORRECTUS_LONGQUEUE,
            L_DESCENDENS, qtype='long', i_range=AMBITUS_ONE_ONLY)
    write_all_porrectus("PunctumAuctusLineBL", S_PORRECTUS_NOBAR, L_DESCENDENS)
    precise_message("porrectus deminutus")
    write_all_porrectus("rdeminutus", S_PORRECTUS, L_DEMINUTUS, qtype='short')
    write_all_porrectus("rdeminutus", S_PORRECTUS_LONGQUEUE, L_DEMINUTUS,
            qtype='long')
    write_all_porrectus("rdeminutus", S_PORRECTUS_NOBAR, L_DEMINUTUS)
    precise_message("porrectus deminutus alt")
    write_all_alt_porrectus_deminutus()
    write_all_alt_porrectus_deminutus(qtype='long')

def fusion_porrectus():
    "Write fusion porrectus."
    precise_message("porrectus-like fusion")
    write_all_porrectus('', S_FLEXUS, L_UP, qtype='short')
    write_all_porrectus('', S_FLEXUS_LONGQUEUE, L_UP, qtype='long',
            i_range=AMBITUS_ONE_ONLY)
    write_all_porrectus('', S_FLEXUS_NOBAR, L_UP)

def write_all_porrectus(last_glyph, shape, lique=L_NOTHING, qtype=None,
        i_range=ALL_AMBITUS, j_range=ALL_AMBITUS):
    for i in i_range:
        for j in j_range:
            write_porrectus(i, j, last_glyph, shape, lique, qtype)

def write_porrectus(i, j, last_glyph, shape, lique=L_NOTHING, qtype=None):
    "Writes the porrectus glyphs."
    new_glyph()
    glyph_name = '%s%s%s%s' % (shape, AMBITUS[i], AMBITUS[j], lique)
    if copy_existing_glyph(glyph_name):
        return
    first_glyph = "porrectus%d" % i
    if j == 1 and glyph_exists("porrectusam1%d" % i):
        first_glyph = "porrectusam1%d" % i
    if last_glyph == 'auctusa2' or last_glyph == 'PunctumAuctusLineBL' or last_glyph == '':
        if j == 1:
            first_glyph = "porrectusflexusnb%d" % i
            if last_glyph == 'auctusa2':
                last_glyph = 'PunctumAscendens'
            elif last_glyph == 'PunctumAuctusLineBL':
                last_glyph = 'PunctumDescendens'
        else:
            first_glyph = "porrectusflexus%d" % i
    if not glyph_exists(first_glyph):
        return
    if qtype:
        draw_left_queue(i, qtype, S_PORRECTUS, L_NOTHING)
    length = get_width(first_glyph)
    paste_glyph(first_glyph)
    draw_line(j, length-get_width('line2'), (-i+1)*FONT_CONFIG['base height'])
    length = length-get_width('line2')
    if qtype:
        simplify()
    if last_glyph == 'rdeminutus':
        length = length+get_width('line2')
        paste_glyph(last_glyph, (length-get_width(last_glyph)),
                       (j-i)*FONT_CONFIG['base height'])
    elif (last_glyph == 'auctusa2' or last_glyph == 'PunctumAuctusLineBL' or
            last_glyph == 'PunctumAscendens' or last_glyph == 'PunctumDescendens'):
        if j == 1:
            length = length+get_width('line2')
        paste_glyph(last_glyph, length, (j-i)*FONT_CONFIG['base height'])

        length = length + get_width(last_glyph)
    elif last_glyph != '':
        paste_glyph(last_glyph,
                (length-get_width(last_glyph)+get_width('line2')),
                (j-i)*FONT_CONFIG['base height'])
        length = length+get_width('line2')
    elif last_glyph == '' and j == 1:
        length = length+get_width('line2')
    set_width(length)
    end_glyph(glyph_name)

def write_all_alt_porrectus_deminutus(qtype='short', i_range=ALL_AMBITUS,
        j_range=ALL_AMBITUS):
    for i in i_range:
        for j in j_range:
            write_alt_porrectus_deminutus(i, j, qtype)

def write_alt_porrectus_deminutus(i, j, qtype='short'):
    "Writes the alternate porrectur deminutus glyphs."
    new_glyph()
    if qtype=='long':
        glyph_name = 'PorrectusLongqueue%s%sDeminutus.alt' % (AMBITUS[i], AMBITUS[j])
    else:
        glyph_name = 'Porrectus%s%sDeminutus.alt' % (AMBITUS[i], AMBITUS[j])
    if copy_existing_glyph(glyph_name):
        return
    draw_left_queue(i, qtype, S_PORRECTUS_DEMINUTUS_ALT, L_NOTHING)
    if i == 1:
        first_glyph = 'PunctumLineBR'
    else:
        first_glyph = 'PunctumLineBLBR'
    paste_glyph(first_glyph)
    draw_line(i, get_width(first_glyph)-get_width('line2'), (-i+1)*FONT_CONFIG['base height'])
    simplify()
    paste_glyph('mpdeminutus', (get_width(first_glyph)-get_width('line2')),
                   (-i)*FONT_CONFIG['base height'])
    draw_line(j,
               get_width(first_glyph)+get_width('mpdeminutus')-
               2*get_width('line2'), (-i+1)*FONT_CONFIG['base height'])
    paste_glyph('rdeminutus', (get_width(first_glyph)
                                  + get_width('mpdeminutus') -
                                  get_width('line2') -
                                  get_width(('rdeminutus'))),
                   (j-i)*FONT_CONFIG['base height'])
    set_width(get_width(first_glyph)+get_width('mpdeminutus')-
              get_width('line2'))
    end_glyph(glyph_name)


def porrectusflexus():
    "Creates the porrectusflexus."
    message("porrectus flexus")
    precise_message("porrectus flexus")
    write_all_porrectusflexus("PunctumLineTL", S_PORRECTUS_FLEXUS_NOBAR)
    write_all_porrectusflexus("PunctumLineTL", S_PORRECTUS_FLEXUS, qtype='short')
    write_all_porrectusflexus("PunctumLineTL", S_PORRECTUS_FLEXUS_LONGQUEUE,
            qtype='long', i_range=AMBITUS_ONE_ONLY)
    precise_message("porrectus flexus auctus descendens")
    write_all_porrectusflexus("auctusd1", S_PORRECTUS_FLEXUS_NOBAR, L_DESCENDENS)
    write_all_porrectusflexus("auctusd1", S_PORRECTUS_FLEXUS, L_DESCENDENS,
            qtype='short')
    write_all_porrectusflexus("auctusd1", S_PORRECTUS_FLEXUS_LONGQUEUE,
            L_DESCENDENS, qtype='long', i_range=AMBITUS_ONE_ONLY)
    precise_message("porrectus flexus auctus ascendens")
    write_all_porrectusflexus("auctusa1", S_PORRECTUS_FLEXUS_NOBAR, L_ASCENDENS)
    write_all_porrectusflexus("auctusa1", S_PORRECTUS_FLEXUS, L_ASCENDENS,
            qtype='short')
    write_all_porrectusflexus("auctusa1", S_PORRECTUS_FLEXUS_LONGQUEUE,
            L_ASCENDENS, qtype='long', i_range=AMBITUS_ONE_ONLY)
    precise_message("porrectus flexus deminutus")
    write_all_porrectusflexus("deminutus", S_PORRECTUS_FLEXUS_NOBAR, L_DEMINUTUS)
    write_all_porrectusflexus("deminutus", S_PORRECTUS_FLEXUS, L_DEMINUTUS,
            qtype='short')
    write_all_porrectusflexus("deminutus", S_PORRECTUS_FLEXUS_LONGQUEUE,
            L_DEMINUTUS, qtype='long', i_range=AMBITUS_ONE_ONLY)

def write_all_porrectusflexus(last_glyph, shape, lique=L_NOTHING, qtype=None,
        i_range=ALL_AMBITUS, j_range=ALL_AMBITUS, k_range=ALL_AMBITUS):
    for i in i_range:
        for j in j_range:
            for k in k_range:
                write_porrectusflexus(i, j, k, last_glyph, shape, lique, qtype)

def write_porrectusflexus(i, j, k, last_glyph, shape, lique=L_NOTHING,
        qtype=None):
    "Writes the porrectusflexus glyphs."
    new_glyph()
    glyph_name = '%s%s%s%s%s' % (shape, AMBITUS[i], AMBITUS[j], AMBITUS[k], lique)
    if copy_existing_glyph(glyph_name):
        return
    if j == 1:
        first_glyph = "porrectusflexusnb%d" % i
    else:
        first_glyph = "porrectusflexus%d" % i
    if not glyph_exists(first_glyph):
        return
    if qtype:
        draw_left_queue(i, qtype, S_PORRECTUS_FLEXUS, L_NOTHING)
    length = get_width(first_glyph)
    paste_glyph(first_glyph)
    draw_line(j, length-get_width('line2'), (-i+1)*FONT_CONFIG['base height'])
    if last_glyph == "deminutus":
        width_dem = draw_deminutus(j-i, k, length-get_width('line2'),
                                    qtype != None, firstbar=1)
        length = length+width_dem-get_width('line2')
    else:
        simplify()
        middle_glyph = 'PunctumLineBLBR'
        if j == 1:
            if k == 1:
                middle_glyph = 'Punctum'
            else:
                middle_glyph = 'PunctumLineBR'
        else:
            length = length-get_width('line2')
            if k == 1:
                middle_glyph = 'PunctumLineBL'
        paste_glyph(middle_glyph, length, (j-i)*FONT_CONFIG['base height'])
        if k == 1:
            if last_glyph == 'PunctumLineTL':
                last_glyph = 'Punctum'
            elif last_glyph == 'auctusa1':
                last_glyph = 'PunctumAscendens'
            elif last_glyph == 'auctusd1':
                last_glyph = 'PunctumDescendens'
            length = length+get_width(middle_glyph)
        else:
            draw_line(k, length + get_width(middle_glyph) - get_width('line2'),
                       (j-i-k+1)*FONT_CONFIG['base height'])
            length = length + get_width(middle_glyph) - get_width('line2')
        paste_glyph(last_glyph, length, (j-i-k)*FONT_CONFIG['base height'])
        length = length+get_width(last_glyph)
    set_width(length)
    end_glyph(glyph_name)

def torculus():
    "Creates the torculus."
    message("torculus")
    precise_message("torculus")
    write_all_torculus("PunctumLineTR", "PunctumLineTL", S_TORCULUS)
    write_all_torculus("QuilismaLineTR", "PunctumLineTL", S_TORCULUS_QUILISMA)
    precise_message("torculus initio debilis")
    write_all_torculus("idebilis", "PunctumLineTL", S_TORCULUS, L_INITIO_DEBILIS)
    precise_message("torculus auctus descendens")
    write_all_torculus("PunctumLineTR", "auctusd1", S_TORCULUS, L_DESCENDENS)
    write_all_torculus("QuilismaLineTR", "auctusd1", S_TORCULUS_QUILISMA,
            L_DESCENDENS)
    precise_message("torculus initio debilis auctus descendens")
    write_all_torculus("idebilis", "auctusd1", S_TORCULUS,
            L_INITIO_DEBILIS_DESCENDENS)
    precise_message("torculus auctus ascendens")
    write_all_torculus("PunctumLineTR", "auctusa1", S_TORCULUS, L_ASCENDENS)
    write_all_torculus("QuilismaLineTR", "auctusa1", S_TORCULUS_QUILISMA,
            L_ASCENDENS)
    precise_message("torculus initio debilis auctus ascendens")
    write_all_torculus("idebilis", "auctusa1", S_TORCULUS,
            L_INITIO_DEBILIS_ASCENDENS)
    precise_message("torculus deminutus")
    write_all_torculus("PunctumLineTR", "deminutus", S_TORCULUS, L_DEMINUTUS)
    write_all_torculus("QuilismaLineTR", "deminutus", S_TORCULUS_QUILISMA,
            L_DEMINUTUS)
    precise_message("torculus initio debilis deminutus")
    write_all_torculus("idebilis", "deminutus", S_TORCULUS,
            L_INITIO_DEBILIS_DEMINUTUS)

def write_all_torculus(first_glyph, last_glyph, shape, lique=L_NOTHING,
        i_range=ALL_AMBITUS, j_range=ALL_AMBITUS):
    for i in i_range:
        for j in j_range:
            write_torculus(i, j, first_glyph, last_glyph, shape, lique)

def write_torculus(i, j, first_glyph, last_glyph, shape, lique=L_NOTHING):
    "Writes the torculus glyphs."
    new_glyph()
    glyph_name = '%s%s%s%s' % (shape, AMBITUS[i], AMBITUS[j], lique)
    if copy_existing_glyph(glyph_name):
        return
    length = get_width(first_glyph)-get_width('line2')
    if first_glyph == "QuilismaLineTR":
        if i == 1:
            first_glyph = 'Quilisma'
            length = get_width(first_glyph)
    elif i == 1 and first_glyph == 'PunctumLineTR':
        first_glyph = 'Punctum'
        length = length = get_width(first_glyph)+0.1
    paste_glyph(first_glyph)
    if i != 1:
        draw_line(i, length, FONT_CONFIG['base height'])
    if last_glyph == "deminutus":
        if i == 1:
            width_dem = draw_deminutus(i, j, length, firstbar=0)
        else:
            width_dem = draw_deminutus(i, j, length, firstbar=1)
        length = length+ width_dem
    else:
        if j == 1:
            if i == 1:
                second_glyph = 'Punctum'
            else:
                second_glyph = 'PunctumLineBL'
            paste_glyph(second_glyph, length, i*FONT_CONFIG['base height'])
            length = length+get_width(second_glyph)
            if last_glyph == 'PunctumLineTL':
                last_glyph = 'Punctum'
            elif last_glyph == 'auctusa1':
                last_glyph = 'PunctumAscendens'
            elif last_glyph == 'auctusd1':
                last_glyph = 'PunctumDescendens'
        else:
            if i == 1:
                second_glyph = 'PunctumLineBR'
            else:
                second_glyph = 'PunctumLineBLBR'
            paste_glyph(second_glyph, length, i*FONT_CONFIG['base height'])
            length = length+get_width(second_glyph)-get_width('line2')
            draw_line(j, length, (i-j+1)*FONT_CONFIG['base height'])
        paste_glyph(last_glyph, length, (i-j)*FONT_CONFIG['base height'])
        length = length+get_width(last_glyph)
    set_width(length)
    end_glyph(glyph_name)

def torculus_liquescens():
    "Creates the torculus liquescens."
    precise_message("torculus liquescens")
    write_all_torculus_liquescens('PunctumLineTR', S_TORCULUS_LIQUESCENS,
            L_DEMINUTUS)
    precise_message("torculus liquescens quilisma")
    write_all_torculus_liquescens('QuilismaLineTR',
            S_TORCULUS_LIQUESCENS_QUILISMA, L_DEMINUTUS)

def write_all_torculus_liquescens(first_glyph, shape, lique='deminutus',
        i_range=ALL_AMBITUS, j_range=ALL_AMBITUS, k_range=ALL_AMBITUS):
    for i in i_range:
        for j in j_range:
            for k in k_range:
                write_torculus_liquescens(i, j, k, first_glyph, shape, lique)

def write_torculus_liquescens(i, j, k, first_glyph, shape, lique='deminutus'):
    "Writes the torculus liquescens glyphs."
    new_glyph()
    glyph_name = '%s%s%s%s%s' % (shape, AMBITUS[i], AMBITUS[j], AMBITUS[k], lique)
    if copy_existing_glyph(glyph_name):
        return
    length = get_width(first_glyph)-get_width('line2')
    if first_glyph == "QuilismaLineTR":
        if i == 1:
            first_glyph = 'Quilisma'
            length = get_width(first_glyph)
    elif i == 1:
        first_glyph = 'Punctum'
        length = get_width(first_glyph)+0.1
    paste_glyph(first_glyph)
    if i != 1:
        draw_line(i, length, FONT_CONFIG['base height'])
    flexus_firstbar = 2
    if j == 1:
        flexus_firstbar = 0
        if i == 1:
            second_glyph = 'Punctum'
        else:
            second_glyph = 'PunctumLineBL'
        paste_glyph(second_glyph, length, i*FONT_CONFIG['base height'])
        length = length+get_width(second_glyph)
    else:
        if i == 1:
            second_glyph = 'PunctumLineBR'
        else:
            second_glyph = 'PunctumLineBLBR'
        paste_glyph(second_glyph, length, i*FONT_CONFIG['base height'])
        length = length+get_width(second_glyph)-get_width('line2')
        draw_line(j, length, (i-j+1)*FONT_CONFIG['base height'])
    width_dem = draw_deminutus(i-j, k, length, firstbar=flexus_firstbar)
    length = length+width_dem
    set_width(length)
    end_glyph(glyph_name)

def torculusresupinus():
    "Creates the torculusresupinus."
    message("torculus resupinus")
    precise_message("torculus resupinus")
    write_all_torculusresupinus('PunctumLineTR', 'PunctumSmall',
            S_TORCULUS_RESUPINUS)
    write_all_torculusresupinus('idebilis', 'PunctumSmall', S_TORCULUS_RESUPINUS,
            L_INITIO_DEBILIS)
    write_all_torculusresupinus('QuilismaLineTR', 'PunctumSmall',
            S_TORCULUS_RESUPINUS_QUILISMA)
    precise_message("torculus resupinus deminutus")
    write_all_torculusresupinus('PunctumLineTR', 'rdeminutus',
            S_TORCULUS_RESUPINUS, L_DEMINUTUS)
    write_all_torculusresupinus('idebilis', 'rdeminutus', S_TORCULUS_RESUPINUS,
            L_INITIO_DEBILIS_DEMINUTUS)
    write_all_torculusresupinus('QuilismaLineTR', 'rdeminutus',
            S_TORCULUS_RESUPINUS_QUILISMA, L_DEMINUTUS)
    precise_message("torculus resupinus deminutus alt")
    write_all_alt_torculusresupinusdeminutus('PunctumLineTR',
            S_TORCULUS_RESUPINUS, L_DEMINUTUS)
    write_all_alt_torculusresupinusdeminutus('idebilis', S_TORCULUS_RESUPINUS,
            L_INITIO_DEBILIS_DEMINUTUS)
    write_all_alt_torculusresupinusdeminutus('QuilismaLineTR',
            S_TORCULUS_RESUPINUS_QUILISMA, L_DEMINUTUS)
    precise_message("torculus resupinus auctus ascendens")
    write_all_torculusresupinus('PunctumLineTR', "auctusa2",
            S_TORCULUS_RESUPINUS, L_ASCENDENS)
    write_all_torculusresupinus('idebilis', "auctusa2", S_TORCULUS_RESUPINUS,
            L_INITIO_DEBILIS_ASCENDENS)
    write_all_torculusresupinus('QuilismaLineTR', "auctusa2",
            S_TORCULUS_RESUPINUS_QUILISMA, L_ASCENDENS)
    precise_message("torculus resupinus auctus descendens")
    write_all_torculusresupinus('PunctumLineTR', "PunctumAuctusLineBL",
            S_TORCULUS_RESUPINUS, L_DESCENDENS)
    write_all_torculusresupinus('idebilis', "PunctumAuctusLineBL",
            S_TORCULUS_RESUPINUS, L_INITIO_DEBILIS_DESCENDENS)
    write_all_torculusresupinus('QuilismaLineTR', "PunctumAuctusLineBL",
            S_TORCULUS_RESUPINUS_QUILISMA, L_DESCENDENS)

def write_all_torculusresupinus(first_glyph, last_glyph, shape, lique=L_NOTHING,
        i_range=ALL_AMBITUS, j_range=ALL_AMBITUS, k_range=ALL_AMBITUS):
    for i in i_range:
        for j in j_range:
            for k in k_range:
                write_torculusresupinus(i, j, k, first_glyph, last_glyph, shape,
                        lique)

def write_torculusresupinus(i, j, k, first_glyph, last_glyph, shape,
        lique=L_NOTHING):
    "Writes the torculusresupinus glyphs."
    new_glyph()
    glyph_name = '%s%s%s%s%s' % (shape, AMBITUS[i], AMBITUS[j], AMBITUS[k], lique)
    if copy_existing_glyph(glyph_name):
        return
    middle_glyph = "porrectus%d" % j
    if k == 1 and glyph_exists("porrectusam1%d" % j):
        middle_glyph = "porrectusam1%d" % j
    if last_glyph == 'auctusa2' or last_glyph == 'PunctumAuctusLineBL':
        middle_glyph = "porrectusflexus%d" % j
        if k == 1:
            middle_glyph = "porrectusflexusnb%d" % j
            if last_glyph == 'auctusa2':
                last_glyph = 'PunctumAscendens'
            elif last_glyph == 'PunctumAuctusLineBL':
                last_glyph = 'PunctumDescendens'
    if not glyph_exists(middle_glyph):
        return
    if i == 1 and first_glyph != 'idebilis':
        if first_glyph == 'PunctumLineTR':
            first_glyph = 'Punctum'
        elif first_glyph == 'QuilismaLineTR':
            first_glyph = 'Quilisma'
        length = get_width(first_glyph)+0.1
    else:
        length = get_width(first_glyph)-get_width('line2')
    paste_glyph(first_glyph)
    if i != 1:
        draw_line(i, length, FONT_CONFIG['base height'])
    paste_glyph(middle_glyph, length, i*FONT_CONFIG['base height'])
    length = length + get_width(middle_glyph)
    if k != 1:
        draw_line(k, length-get_width('line2'), (i-j+1)*FONT_CONFIG['base height'])
    simplify()
    if last_glyph == "rdeminutus":
        paste_glyph(last_glyph,
                       (length-get_width('rdeminutus')),
                       (i-j+k)*FONT_CONFIG['base height'])
    elif (last_glyph == 'auctusa2' or last_glyph == 'PunctumAuctusLineBL' or
            last_glyph == 'PunctumAscendens' or last_glyph == 'PunctumDescendens'):
        if k > 1:
            length = length-get_width('line2')
        paste_glyph(last_glyph, length, (i-j+k)*FONT_CONFIG['base height'])
        length = length + get_width(last_glyph)
    else:
        paste_glyph(last_glyph, (length-get_width(last_glyph)),
                       (i-j+k)*FONT_CONFIG['base height'])
    set_width(length)
    end_glyph(glyph_name)

def write_all_alt_torculusresupinusdeminutus(first_glyph, shape, lique=L_NOTHING,
        i_range=ALL_AMBITUS, j_range=ALL_AMBITUS, k_range=ALL_AMBITUS):
    for i in i_range:
        for j in j_range:
            for k in k_range:
                write_alt_torculusresupinusdeminutus(i, j, k, first_glyph,
                        shape, lique)

def write_alt_torculusresupinusdeminutus(i, j, k, first_glyph, shape,
        lique=L_NOTHING):
    # pylint: disable=invalid-name
    "Writes the torculusresupinusdeminutus glyphs."
    new_glyph()
    glyph_name = '%s%s%s%s%s.alt' % (shape, AMBITUS[i], AMBITUS[j], AMBITUS[k], lique)
    if copy_existing_glyph(glyph_name):
        return
    length = get_width(first_glyph)-get_width('line2')
    if i == 1:
        if first_glyph == 'PunctumLineTR':
            first_glyph = 'Punctum'
            length = get_width(first_glyph)+0.1
        elif first_glyph == 'QuilismaLineTR':
            first_glyph = 'Quilisma'
            length = get_width(first_glyph)+0.1
    paste_glyph(first_glyph)
    if i != 1:
        draw_line(i, length, FONT_CONFIG['base height'])
    if j == 1 and i == 1:
        if first_glyph == "idebilis":
            second_glyph = 'PunctumLineBL'
            last_glyph = 'mnbpdeminutus'
        else:
            second_glyph = 'Punctum'
            last_glyph = 'mnbpdeminutus'
        paste_glyph(second_glyph, length, i*FONT_CONFIG['base height'])
        length = length + get_width(second_glyph)
    elif j == 1:
        second_glyph = 'PunctumLineBL'
        paste_glyph(second_glyph, length, i*FONT_CONFIG['base height'])
        length = length + get_width(second_glyph)
        last_glyph = 'mnbpdeminutus'
    elif i == 1 and first_glyph != "idebilis":
        second_glyph = 'PunctumLineBR'
        paste_glyph(second_glyph, length, i*FONT_CONFIG['base height'])
        length = length + get_width(second_glyph)-get_width('line2')
        draw_line(j, length, (i-j+1)*FONT_CONFIG['base height'])
        last_glyph = 'mpdeminutus'
    else:
        second_glyph = 'PunctumLineBLBR'
        paste_glyph(second_glyph, length, i*FONT_CONFIG['base height'])
        length = length + get_width(second_glyph)-get_width('line2')
        draw_line(j, length, (i-j+1)*FONT_CONFIG['base height'])
        last_glyph = 'mpdeminutus'
    paste_glyph(last_glyph, length, (i-j)*FONT_CONFIG['base height'])
    length = length+get_width(last_glyph)
    draw_line(k, length-get_width('line2'), (i-j+1)*FONT_CONFIG['base height'])
    paste_glyph('rdeminutus',
                   length-get_width('rdeminutus'), (i-j+k)*FONT_CONFIG['base height'])
    set_width(length)
    end_glyph(glyph_name)

def scandicus():
    "Creates the scandicus."
    message("scandicus")
    write_all_scandicus('PunctumSmall')
    write_all_scandicus('rdeminutus', L_DEMINUTUS)

def write_all_scandicus(last_glyph, lique=L_NOTHING, i_range=ALL_AMBITUS,
                        j_range=ALL_AMBITUS):
    for i in i_range:
        for j in j_range:
                write_scandicus(i, j, last_glyph, lique)

def write_scandicus(i, j, last_glyph, lique=L_NOTHING):
    "Writes the scandicus glyphs."
    new_glyph()
    final_vertical_shift = 0
    glyph_name = '%s%s%s%s' % (S_SCANDICUS, AMBITUS[i], AMBITUS[j], lique)
    if copy_existing_glyph(glyph_name):
        return
    # special case of i=j=1, we use glyph 1025 directly
    if i == 1 and j == 1 and lique == L_NOTHING:
        paste_glyph('Punctum')
        second_glyph = 'PesOneNothing'
        paste_glyph(second_glyph, get_width('Punctum'), FONT_CONFIG['base height'])
        set_width(get_width('PesOneNothing')+get_width('Punctum'))
        end_glyph(glyph_name)
        return
    if i == 1:
        paste_glyph('Punctum')
        length = get_width('Punctum')
        second_glyph = 'p2base'
        if lique == L_DEMINUTUS:
            second_glyph = 'mnbpdeminutus'
        if j == 1:
            final_vertical_shift = FONT_CONFIG['deminutus vertical shift']
    else:
        paste_glyph('PunctumLineTR')
        length = get_width('PunctumLineTR') - get_width('line2')
        draw_line(i, length, FONT_CONFIG['base height'])
        second_glyph = 'msdeminutus'
        if j == 1 and glyph_exists('msdeminutusam1'):
            second_glyph = 'msdeminutusam1'
        if j == 1:
            final_vertical_shift = FONT_CONFIG['deminutus vertical shift']
    if j == 1 and lique == L_NOTHING and glyph_exists('UpperPesOneNothing'):
        paste_glyph('UpperPesOneNothing', length, i * FONT_CONFIG['base height'])
        set_width(length + get_width('UpperPesOneNothing'))
        end_glyph(glyph_name)
        return
    paste_glyph(second_glyph, length, i*FONT_CONFIG['base height'])
    if (i == 1) and lique == L_NOTHING:
        length = length + get_width('Punctum')
    else:
        length = length + get_width(second_glyph)
    if j != 1:
        draw_line(j, length - get_width('line2'), (i+1) * FONT_CONFIG['base height'])
    if last_glyph == 'rdeminutus':
        paste_glyph('rdeminutus', length -
                       get_width('rdeminutus'), (i+j)*FONT_CONFIG['base height']+
                       final_vertical_shift)
    else:
        paste_glyph(last_glyph, length - get_width(last_glyph),
                       (i+j)*FONT_CONFIG['base height']+final_vertical_shift)
    set_width(length)
    end_glyph(glyph_name)

def ancus():
    "Creates the ancus."
    message("ancus")
    write_all_ancus('rvbase', S_ANCUS, 'short')
    write_all_ancus('rvbase', S_ANCUS_LONGQUEUE, 'long')

def write_all_ancus(first_glyph, glyph_type, qtype, i_range=ALL_AMBITUS,
        j_range=ALL_AMBITUS):
    for i in i_range:
        for j in j_range:
            write_ancus(i, j, first_glyph, glyph_type, qtype)

def write_ancus(i, j, first_glyph, glyph_type, qtype):
    "Writes the ancus glyphs."
    new_glyph()
    glyph_name = '%s%s%s%s' % (glyph_type, AMBITUS[i], AMBITUS[j], L_DEMINUTUS)
    if copy_existing_glyph(glyph_name):
        return
    draw_left_queue(i, qtype, S_ANCUS, L_NOTHING, j)
    if i == 1:
        first_glyph = 'rvirgabase'
        length = get_width(first_glyph)
    else:
        length = get_width(first_glyph) - get_width('line2')
    paste_glyph(first_glyph)
    if i != 1:
        draw_line(i, length, (-i+1)*FONT_CONFIG['base height'])
    width_dem = draw_deminutus(-i, j, length,
                                firstbar = 0 if i == 1 else 2)
    set_width(length+width_dem)
    end_glyph(glyph_name)

def leading():
    "Creates the leading fusion glyphs."
    message("leading fusion glyphs")
    write_all_leading('PunctumLineTR', S_LEADING_PUNCTUM)
    write_all_leading('idebilis', S_LEADING_PUNCTUM, L_INITIO_DEBILIS)
    write_all_leading('QuilismaLineTR', S_LEADING_QUILISMA)
    write_all_leading('AscendensOriscusLineTR', S_LEADING_ORISCUS)

# lique has a slightly different meaning here
def write_all_leading(first_glyph, glyph_type, lique='', i_range=ALL_AMBITUS):
    for i in i_range:
        write_leading(i, first_glyph, glyph_type, lique)

def write_leading(i, first_glyph, glyph_type, lique=''):
    "Writes the leading fusion glyphs."
    new_glyph()
    glyph_name = '%s%s%s' % (glyph_type, AMBITUS[i], lique)
    if copy_existing_glyph(glyph_name):
        return
    length = -get_width('line2')
    if i == 1 and first_glyph != 'idebilis':
        length = 0.1
        if first_glyph == 'PunctumLineTR':
            first_glyph = 'Punctum'
        elif first_glyph == 'QuilismaLineTR':
            first_glyph = 'Quilisma'
        elif first_glyph == 'AscendensOriscusLineTR':
            first_glyph = FLATTENED_ORISCUS['AscendensOriscusFlatTop']
    length = get_width(first_glyph) + length
    paste_glyph(first_glyph, 0, -i * FONT_CONFIG['base height'])
    if i != 1:
        draw_line(i, length, -(i-1) * FONT_CONFIG['base height'])
    simplify()
    set_width(length)
    end_glyph(glyph_name)

def fusion():
    "Creates the fusion glyphs."
    message("simple fusion glyphs")
    write_all_fusion_leading('PunctumLineTR', S_PUNCTUM, L_UP)
    write_all_fusion_leading('msdeminutus', S_UPPER_PUNCTUM, L_UP)
    write_all_fusion_leading('PunctumLineTLTR', S_LOWER_PUNCTUM, L_UP)
    write_all_fusion_leading('idebilis', S_PUNCTUM, L_INITIO_DEBILIS_UP)
    write_all_fusion_leading('QuilismaLineTR', S_QUILISMA, L_UP)
    write_all_fusion_leading('AscendensOriscusLineTR', S_ASCENDENS_ORISCUS, L_UP)
    write_all_fusion_leading('DescendensOriscusLineTR', S_DESCENDENS_ORISCUS,
            L_UP)
    write_all_fusion_leading('AscendensOriscusLineBLTR',
            S_UPPER_ASCENDENS_ORISCUS, L_UP)
    write_all_fusion_leading(FLATTENED_ORISCUS['AscendensOriscusLineTRFlatBottom'],
            S_UPPER_OBLATUS_ASCENDENS_ORISCUS, L_UP)
    write_all_fusion_leading('DescendensOriscusLineBLTR',
            S_UPPER_DESCENDENS_ORISCUS, L_UP)
    write_all_fusion_leading('AscendensOriscusLineTLTR',
            S_LOWER_ASCENDENS_ORISCUS, L_UP)
    write_all_fusion_leading('DescendensOriscusLineTLTR',
            S_LOWER_DESCENDENS_ORISCUS, L_UP)
    write_all_fusion_leading(FLATTENED_ORISCUS['DescendensOriscusLineTRFlatTop'],
            S_LOWER_OBLATUS_DESCENDENS_ORISCUS, L_UP)
    write_all_fusion_leading('AscendensOriscusLineBLTR',
            S_ASCENDENS_ORISCUS_SCAPUS, L_UP, qtype='short',
            stemshape=S_FLEXUS_ORISCUS_SCAPUS)
    write_all_fusion_leading('AscendensOriscusLineBLTR',
            S_ASCENDENS_ORISCUS_SCAPUS_LONGQUEUE, L_UP, qtype='long',
            stemshape=S_FLEXUS_ORISCUS_SCAPUS)
    write_fusion_leading(1, 'AscendensOriscusLineBLTR',
            S_ASCENDENS_ORISCUS_SCAPUS_OPENQUEUE, L_UP, qtype='open',
            stemshape=S_FLEXUS_ORISCUS_SCAPUS)
    write_all_fusion_leading('DescendensOriscusLineBLTR',
            S_DESCENDENS_ORISCUS_SCAPUS, L_UP, qtype='short',
            stemshape=S_FLEXUS_ORISCUS_SCAPUS)
    write_all_fusion_leading('DescendensOriscusLineBLTR',
            S_DESCENDENS_ORISCUS_SCAPUS_LONGQUEUE, L_UP, qtype='long',
            stemshape=S_FLEXUS_ORISCUS_SCAPUS)
    write_fusion_leading(1, 'DescendensOriscusLineBLTR',
            S_DESCENDENS_ORISCUS_SCAPUS_OPENQUEUE, L_UP, qtype='open',
            stemshape=S_FLEXUS_ORISCUS_SCAPUS)
    write_all_fusion_leading('AscendensOriscusLineBR', S_ASCENDENS_ORISCUS,
            L_DOWN)
    write_all_fusion_leading('DescendensOriscusLineBR', S_DESCENDENS_ORISCUS,
            L_DOWN)
    write_all_fusion_leading('AscendensOriscusLineBLBR',
            S_UPPER_ASCENDENS_ORISCUS, L_DOWN)
    write_all_fusion_leading(FLATTENED_ORISCUS['AscendensOriscusLineBRFlatBottom'],
            S_UPPER_OBLATUS_ASCENDENS_ORISCUS, L_DOWN)
    write_all_fusion_leading('DescendensOriscusLineBLBR',
            S_UPPER_DESCENDENS_ORISCUS, L_DOWN)
    write_all_fusion_leading('AscendensOriscusLineTLBR',
            S_LOWER_ASCENDENS_ORISCUS, L_DOWN)
    write_all_fusion_leading('DescendensOriscusLineTLBR',
            S_LOWER_DESCENDENS_ORISCUS, L_DOWN)
    write_all_fusion_leading(FLATTENED_ORISCUS['DescendensOriscusLineBRFlatTop'],
            S_LOWER_OBLATUS_DESCENDENS_ORISCUS, L_DOWN)
    write_all_fusion_leading("AscendensOriscusLineBLBR",
            S_ASCENDENS_ORISCUS_SCAPUS, L_DOWN, qtype='short',
            stemshape=S_FLEXUS_ORISCUS_SCAPUS)
    write_all_fusion_leading("AscendensOriscusLineBLBR",
            S_ASCENDENS_ORISCUS_SCAPUS_LONGQUEUE, L_DOWN, qtype='long',
            stemshape=S_FLEXUS_ORISCUS_SCAPUS)
    write_fusion_leading(1, "AscendensOriscusLineBLBR",
            S_ASCENDENS_ORISCUS_SCAPUS_OPENQUEUE, L_DOWN, qtype='open',
            stemshape=S_FLEXUS_ORISCUS_SCAPUS)
    write_all_fusion_leading("DescendensOriscusLineBLBR",
            S_DESCENDENS_ORISCUS_SCAPUS, L_DOWN, qtype='short',
            stemshape=S_FLEXUS_ORISCUS_SCAPUS)
    write_all_fusion_leading("DescendensOriscusLineBLBR",
            S_DESCENDENS_ORISCUS_SCAPUS_LONGQUEUE, L_DOWN, qtype='long',
            stemshape=S_FLEXUS_ORISCUS_SCAPUS)
    write_fusion_leading(1, "DescendensOriscusLineBLBR",
            S_DESCENDENS_ORISCUS_SCAPUS_OPENQUEUE, L_DOWN, qtype='open',
            stemshape=S_FLEXUS_ORISCUS_SCAPUS)
    write_all_fusion_leading('PunctumLineBR', S_PUNCTUM, L_DOWN)
    write_all_fusion_leading('PunctumLineBLBR', S_UPPER_PUNCTUM, L_DOWN)
    write_all_fusion_leading('mademinutus', S_LOWER_PUNCTUM, L_DOWN)
    write_all_fusion_leading('VirgaBaseLineBL', S_VIRGA_REVERSA, L_DOWN,
            qtype='short', stemshape=S_FLEXUS)
    write_all_fusion_leading('VirgaBaseLineBL', S_VIRGA_REVERSA_LONGQUEUE,
            L_DOWN, qtype='long', stemshape=S_FLEXUS)
    write_fusion_leading(1, 'VirgaBaseLineBL', S_VIRGA_REVERSA_OPENQUEUE,
            L_DOWN, qtype='open', stemshape=S_FLEXUS)

# lique is only for initio debilis here
def write_all_fusion_leading(first_glyph, glyph_type, lique, qtype=None,
        stemshape=None, i_range=ALL_AMBITUS):
    for i in i_range:
        write_fusion_leading(i, first_glyph, glyph_type, lique, qtype,
                stemshape)

def write_fusion_leading(i, first_glyph, glyph_type, lique, qtype=None,
        stemshape=None):
    "Writes the fusion glyphs."
    new_glyph()
    glyph_name = '%s%s%s' % (glyph_type, AMBITUS[i], lique)
    if copy_existing_glyph(glyph_name):
        return
    length = -get_width('line2')
    if i == 1 and first_glyph != 'idebilis':
        length = 0.1
        if first_glyph == 'PunctumLineTR' or first_glyph == 'PunctumLineBR':
            first_glyph = 'Punctum'
        elif first_glyph == 'QuilismaLineTR':
            first_glyph = 'Quilisma'
        elif 'Oriscus' in first_glyph and 'Line' in first_glyph:
            if 'Flat' in first_glyph:
                if first_glyph.endswith('LineBRFlatTop'):
                    first_glyph = FLATTENED_ORISCUS[first_glyph[:-13] + 'Flattened']
                elif first_glyph.endswith('LineTRFlatBottom'):
                    first_glyph = FLATTENED_ORISCUS[first_glyph[:-16] + 'Flattened']
                else:
                    first_glyph = FLATTENED_ORISCUS[
                            first_glyph.replace('LineTR','').replace('LineBR','')]
            elif first_glyph.endswith('LineTR') or first_glyph.endswith('LineBR'):
                first_glyph = first_glyph[:-6]
                if first_glyph.startswith('Ascendens') and lique == L_UP:
                    first_glyph = FLATTENED_ORISCUS[first_glyph + 'FlatTop']
                elif first_glyph.startswith('Descendens') and lique == L_DOWN:
                    first_glyph = FLATTENED_ORISCUS[first_glyph + 'FlatBottom']
            else:
                first_glyph = first_glyph[:-2]
                if first_glyph.startswith('Ascendens') and lique == L_UP:
                    first_glyph = FLATTENED_ORISCUS[first_glyph + 'FlatTop']
                elif first_glyph.startswith('Descendens') and lique == L_DOWN:
                    first_glyph = FLATTENED_ORISCUS[first_glyph + 'FlatBottom']
        elif first_glyph == 'msdeminutus' or first_glyph == 'PunctumLineBLBR':
            first_glyph = 'PunctumLineBL'
        elif first_glyph == 'mademinutus' or first_glyph == 'PunctumLineTLTR':
            first_glyph = 'PunctumLineTL'
        elif first_glyph == 'VirgaBaseLineBL':
            first_glyph = 'rvirgabase'
    length = get_width(first_glyph) + length
    if qtype:
        draw_left_queue(i, qtype, stemshape, lique)
    paste_glyph(first_glyph)
    if i != 1:
        if lique == L_UP or lique == L_INITIO_DEBILIS_UP:
            draw_line(i, length, FONT_CONFIG['base height'])
        elif lique == L_DOWN:
            draw_line(i, length, -(i - 1) * FONT_CONFIG['base height'])
    simplify()
    set_width(length)
    end_glyph(glyph_name)

def brackets():
    for i in range(0, 15):
        write_bracket(i, 'Left')
    for i in range(0, 15):
        write_bracket(i, 'Left', 'Short')
    for i in range(0, 15, 2):
        write_bracket(i, 'Left', 'Long')
    for i in range(0, 15):
        write_bracket(i, 'Right')
    for i in range(0, 15):
        write_bracket(i, 'Right', 'Short')
    for i in range(0, 15, 2):
        write_bracket(i, 'Right', 'Long')

def write_bracket(i, direction, size = ''):
    "Writes a bracket glyph"
    new_glyph()
    if not cavum:
        glyph_name = 'Bracket%s%s%s' % (direction, size, AMBITUS[i])
        line = 'Bracket%sLine' % direction
        base_height = FONT_CONFIG['base height']
        if copy_existing_glyph(glyph_name):
            return
        paste_glyph('Bracket' + direction + 'Bottom')
        for j in range(0, i+1):
            paste_glyph(line, 0, j*base_height)
        if size == 'Short':
            paste_glyph('Bracket' + direction + 'Top', 0,
                    i*base_height-FONT_CONFIG['bracket shift'])
        elif size == 'Long':
            paste_glyph('Bracket' + direction + 'Top', 0,
                    i*base_height+FONT_CONFIG['bracket shift'])
        else:
            paste_glyph('Bracket' + direction + 'Top', 0, i*base_height)
        simplify()
        set_width(get_width(line))
        end_glyph(glyph_name)

if __name__ == "__main__":
    main()
