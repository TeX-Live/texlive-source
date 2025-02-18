# This is a companion to squarize.py
# coding=utf-8

"""
    Companion to squarize.py, building the stem length config
    according to various schemas.

    Copyright (C) 2016-2025 The Gregorio Project (see CONTRIBUTORS.md)

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

    The idea here is to return a "stem schema" describing the lengths
    of all possible stems.

    Common names of variables:
      - font_config(dict): the font configuration (ex: content of
            greciliae.json)
      - add_suppl(boolean): if we must make queues slightly longer
            than their reference height. The height difference is
            font_config[font_config['bottom-add']]
      - bmu(str): "bottom" or "middle" or "top"
      - base (int): font_config['base height']
"""

def get_default_porrectus(font_config, add_suppl, one_bottom):
    """ Common subfunction, gets porrectus length.
    """
    suppl = font_config['bottom-add'] if add_suppl else 0
    return {
        "Nothing": {
            "1": {
                "short": font_config['bottom-porrectus-1'] + suppl,
                "long": one_bottom
            },
            "2": {
                "short": font_config['bottom-porrectus-2'] + suppl,
                "long": font_config['bottom-porrectus-2'] + suppl
            },
            "3": {
                "short": font_config['bottom-porrectus-3'] + suppl,
                "long": font_config['bottom-porrectus-3'] + suppl
            },
            "4": {
                "short": font_config['bottom-porrectus-4'] + suppl,
                "long": font_config['bottom-porrectus-4'] + suppl
            },
            "5": {
                "short": font_config['bottom-porrectus-5'] + suppl,
                "long": font_config['bottom-porrectus-5'] + suppl
            }
        }
    }

def get_conf(font_config, bmu, suffix, add_suppl, second_suffix=''):
    """ Gets a value from font_config, with fallbacks if not present.
        - suffix can be 'deminutus', 'oriscus', etc.
        - second_suffix can be 'lower' or 'upper', currently handling
            quilisma only.
    """
    suffixed = bmu+'-'+suffix
    second_suffixed = bmu+'-'+suffix+'-'+second_suffix
    base = font_config[bmu]
    if second_suffixed in font_config:
        base = font_config[second_suffixed]
    elif suffixed in font_config:
        base = font_config[suffixed]
    if add_suppl and bmu == 'bottom':
        base += font_config['bottom-add']
    return base

def get_stem_schema_default(font_config):
    """ This returns Gregorio's default schema. This is adapted from
        1934 Antiphonale Monasticum (AM), with a few changes:
          - values for ancus and salicus (not present in AM)
          - more consistent short virga stems. AM has a short and
              longer form. See line 6 page 400 for both figures side
              to side. The longer form is used.
          - ih~ and similar have a slightly shorter queue (AM 400 l. 3)
          - small changes for ambitus generally, queues are slightly shorter
          - treat similar figures with consistency (gv and ge for instance)

        List of some figures in the AM:
          - ih(ih) : AM 367, 3 (p. 367, l. 3)
          - hg(hg) : AM 367,4
          - dc(dc) : AM 366
          - ed(ed) : AM 1185,5
          - ig(ig) : AM 367,6
          - fd(fd) : AM 673,1
          - gd(gd) : AM 531,3
          - he(he) : AM 420,7
          - hd(hd) : AM 1183,1
          - ih~(ih~) : AM 396,1
          - hg~(hg~) : AM 347,7
          - dc~(dc~) : AM 397,4, AM 1182,4
          - ed~(ed~) : AM 402,5
          - ge~(ge~) : AM 239,4
          - gd~(gd~) : AM 239,3
          - gc~(gc~) : AM 397,5
    """
    base = font_config['base height']
    # these are used for both virga and flexus deminutus with ambitus one, for coherence
    virga_long = get_conf(font_config, 'bottom', '', False) - 2*base
    virga_short = get_conf(font_config, 'middle', '', False) - 2*base
    virga_open = get_conf(font_config, 'bottom', '', False) - base

    def get_basic(suffix, add_suppl=False, second_suffix=''):
        """ Common function for flexus, pes quadratum, pes quassus, etc.
        """
        bottom = get_conf(font_config, 'bottom', suffix, add_suppl, second_suffix)
        middle = get_conf(font_config, 'middle', suffix, add_suppl, second_suffix)
        top = get_conf(font_config, 'top', suffix, add_suppl, second_suffix)
        # using lower version for bottom of quilisma when second is on a line
        bottom_lower = get_conf(font_config, 'bottom', suffix, add_suppl, 'lower')
        return {
            "1": {
                # ignoring the suffix for coherence
                "short": virga_short,
                "long": virga_long,
                "open": virga_open
            },
            "2": {
                "short": virga_short,
                "long": virga_long
            },
            "3": {
                "short": middle - 3*base,
                "long": bottom_lower - 3*base
            },
            "4": {
                "short": top - 4*base,
                "long": top - 4*base
            },
            "5": {
                "short": top - 4*base,
                "long": top - 4*base
            }
        }

    return {
        "ignore j": True,
        "Virga": {
            "Nothing": {
                "short": virga_short,
                "open": virga_open,
                "long": virga_long
            }
        },
        "Flexus": {
            "Nothing": get_basic(''),
            "DeminutusFirst": get_basic('deminutus-first'),
            "Deminutus": {
                "1": {
                    "short": font_config['middle'] - 2*base,
                    "long": font_config['top'] - 3*base,
                    "open": font_config['bottom-deminutus'] - base
                },
                "2": {
                    "short": font_config['bottom'] -2*base,
                    "long": font_config['bottom'] - 2*base,
                },
                "3": {
                    "long": font_config['top'] - 3*base,
                    "short": font_config['top'] - 3*base
                },
                "4": {
                    "long": font_config['top'] - 4*base,
                    "short": font_config['top-deminutus'] - 4*base,
                },
                "5": {
                    "long": font_config['top'] - 4*base,
                    "short": font_config['top-deminutus'] - 4*base,
                }
            }
        },
        "PesQuilismaQuadratum": {
            "Nothing": get_basic('quilisma', False, 'upper')
        },
        "PesQuassus": {
            "Nothing": get_basic('oriscus', False)
        },
        "Porrectus": get_default_porrectus(font_config, False, virga_long)
    }

def get_stem_schema_solesmes(font_config):
    """ This stem schema has been provided directly by a contact
        at Abbey of Solesmes.
    """
    base = font_config['base height']

    def get_bottom(suffix, add_suppl=True, second_suffix=''):
        """ Shortcut for get_config with common options
        """
        return get_conf(font_config, 'bottom', suffix, add_suppl, second_suffix)

    def get_basic(suffix, add_suppl=True, second_suffix=''):
        """ Common function for flexus, pes quadratum, pes quassus, etc.
        """
        bottom = get_bottom(suffix, add_suppl, second_suffix)
        # for ambitus one, it must have the same height as the virga for coherece, so ignoing suffix
        bottom_one = get_bottom('', True, second_suffix)
        return {
            "1": {
                "short": bottom_one - base,
                "long": bottom_one - 2*base,
                "open": bottom_one - base
            },
            "2": {
                "short": bottom - 2*base,
                "long": bottom - 2*base
            },
            "3": {
                "short": bottom - 3*base,
                "long": bottom - 3*base
            },
            "4": {
                "short": bottom - 4*base,
                "long": bottom - 4*base
            },
            "5": {
                "short": bottom - 5*base,
                "long": bottom - 5*base
            }
        }
    bottom_virga = get_bottom('')
    return {
        "ignore j": True,
        "Virga": {
            "Nothing": {
                "short": bottom_virga - base,
                "open": bottom_virga - base,
                "long": bottom_virga - 2*base
            }
        },
        "Flexus": {
            "Nothing": get_basic(''),
            "Deminutus": get_basic(''),
            "DeminutusFirst": get_basic('deminutus-first')
        },
        "PesQuilismaQuadratum": {
            "Nothing": get_basic('quilisma', False, 'lower')
        },
        "PesQuassus": {
            "Nothing": get_basic('oriscus', False)
        },
        "Porrectus": get_default_porrectus(font_config, True, bottom_virga - 2*base)
    }

def get_stem_schema(schemaname, font_config):
    """ Function called by squarize.py, returns the stem schema.
    """
    if schemaname == 'default':
        return get_stem_schema_default(font_config)
    if schemaname == 'solesmes':
        return get_stem_schema_solesmes(font_config)
    print(f'impossible to find schema {schemaname}, quitting')
