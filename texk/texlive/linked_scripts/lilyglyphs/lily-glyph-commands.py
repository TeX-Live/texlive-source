#!/usr/bin/env python

# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#                                                                        %
#      This file is part of the 'lilyglyphs' LaTeX package.              %
#                                ==========                              %
#                                                                        %
#              https://github.com/openlilylib/lilyglyphs                 %
#               http://www.openlilylib.org/lilyglyphs                    %
#                                                                        %
#  Copyright 2012-2020 Urs Liska and others, ul@openlilylib.org          %
#                                                                        %
#  'lilyglyphs' is free software: you can redistribute it and/or modify  %
#  it under the terms of the LaTeX Project Public License, either        %
#  version 1.3 of this license or (at your option) any later version.    %
#  You may find the latest version of this license at                    %
#               http://www.latex-project.org/lppl.txt                    %
#  more information on                                                   %
#               http://latex-project.org/lppl/                           %
#  and version 1.3 or later is part of all distributions of LaTeX        %
#  version 2005/12/01 or later.                                          %
#                                                                        %
#  This work has the LPPL maintenance status 'maintained'.               %
#  The Current Maintainer of this work is Urs Liska (see above).         %
#                                                                        %
#  This work consists of the files listed in the file 'manifest.txt'     %
#  which can be found in the 'license' directory.                        %
#                                                                        %
#  This program is distributed in the hope that it will be useful,       %
#  but WITHOUT ANY WARRANTY; without even the implied warranty of        %
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  %
#                                                                        %
# %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

# ########################################################################
#                                                                        #
# genGlyphCommands.py                                                    #
#                                                                        #
# generate commands based on Emmentaler glyphs                           #
#                                                                        #
# ########################################################################

import os, sys, argparse, lilyglyphs_common as lg

def main(input_file):
    """Do the main work of the script"""
    
    in_dir, in_file = os.path.split(input_file)
    os.chdir(in_dir)
    
    # load and parse input file
    lg.read_input_file(in_file)
    read_entries()
    
    # generate LaTeX commands and save them to output file
    # (will be a .tex sibling of the input file)
    lg.generate_latex_commands()
    out_file, out_ext = os.path.splitext(input_file)
    lg.write_latex_file(os.path.join(os.getcwd(), out_file + '.tex'))
    
def read_entries():
    """Parse the input file and fill a dictionary"""
    entry = {}
    def reset_entry():
        """Set all members of the dict empty,
           because not all are present in each entry."""
        entry['cmd'] = ''
        entry['element'] = ''
        entry['type'] = 'glyphname'
        entry['comment'] = []
        entry['raise'] = ''
        entry['scale'] = ''
    
    reset_entry()
    scale = ''
    rais = ''
    for line in lg.definitions_file:
        line = line.strip()
        # empty line = end of entry
        if not len(line):
            # skip if cmd and glyph haven't been filled both
            if not (entry['cmd'] and entry['element']):
                print('Skip malformed entry \'' + entry['cmd'] + '\'. Please check input file')
                reset_entry()
            else:
                print('Read entry \'' + entry['cmd'] + '\'')
                lg.in_cmds[entry['cmd']] = {}
                lg.in_cmds[entry['cmd']]['element'] = entry['element']
                lg.in_cmds[entry['cmd']]['type'] = entry['type']
                lg.in_cmds[entry['cmd']]['comment'] = entry['comment']
                if scale:
                    lg.in_cmds[entry['cmd']]['scale'] = scale
                if rais:
                    lg.in_cmds[entry['cmd']]['raise'] = rais
                reset_entry()
        # ignore Python or LaTeX style comments
        elif line[0] in '#%':
            continue
        else:
            # add element to the entry
            keyval = line.split('=')
            if keyval[0] == 'scale':
                scale = keyval[1]
            elif keyval[0] == 'raise':
                rais = keyval[1]
            if keyval[0] == 'comment':
                entry['comment'] = [keyval[1]]
            else:
                entry[keyval[0].strip()] = keyval[1].strip()

    
def usage():
    print('genGlyphCommands.py')
    print('is part of the lilyglyphs package')
    print('')
    print('Usage:')
    print('Pass the name (without path) of an input definitions file')
    print('(this has to be located in the /stash_new_commands directory.')
    print('Please refer to the manual (documentation/lilyglyphs.pdf).')

# ####################################
# Finally launch the program
if __name__ == "__main__":
    # parse command line arguments
    parser = argparse.ArgumentParser(
                      description='Generate LaTeX commands using Emmentaler glyphs', 
                      parents=[lg.common_arguments])
    parser.add_argument('i', 
                        type=lg.is_file, 
                        metavar='INPUT', 
                        help='File with command templates.')
    args = parser.parse_args()
    
    # if we have exactly one existing file
    # join its components and run the program
    main(os.path.join(os.getcwd(), vars(args)['i']))
