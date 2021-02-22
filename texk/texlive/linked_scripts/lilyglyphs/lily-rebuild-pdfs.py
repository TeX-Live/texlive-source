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
#   rebuild-pdfs.py                                                      #
#                                                                        #
# This program looks in the directories where the generated .ly files    #
# and the generated pdf images are stored.                               #
# If it finds that there are missing pdfs it will recompile them         #
# using LilyPond.                                                        #
#                                                                        #
# The main purpose is to allow the creation of pdf images of glyphs      #
# that already have been processed to LaTeX commands but whose PDF       #
# files have been lost.                                                  #
# The other purpose is to update the set of PDF files to a new version   #
# of Emmentaler or LilyPond without touching the definitions.            #
#                                                                        #
# ########################################################################

import os, sys, subprocess, argparse, lilyglyphs_common as lg


def main():
    """Main walk through the program"""
    print('rebuild-pdfs.py')
    print('regenerate all pdf images that are not present (anymore)')
    print('')
    
    # Check if we are in a legal CWD and ensure a PDF subdir is present
    check_paths()

    # build a list of commands with missing PDF files
    src_files = check_missing_pdfs()

    # is there anything to be done at all?
    if len(src_files) == 0:
        print('')
        print('No image files missing, nothing to be done.')
        print('If you want to re-create pdfs, then delete them first')
        sys.exit(0)
    print('')
    print('Found ' + str(len(src_files)) + ' missing file(s).')
    for cmd in src_files:
        print('- ' + cmd)

    # compile all LilyPond files without matching pdf
    lg.lily_files = src_files
    lg.compile_lily_files()
    
    # clean up directories
    lg.cleanup_lily_files()

def check_missing_pdfs():
    """Compares the list of LilyPond source and resulting PDF files.
       Returns a list of LilyPond source file basenames 
       which don't have a corresponding PDF file"""
    print('Reading file lists, counting missing pdf files')
    
    # read existing .pdf files in lg.dir_pdfs
    img_files = []
    for entry in os.listdir(lg.dir_pdfs):
        entry = os.path.join(lg.dir_pdfs, entry)
        if os.path.isfile(entry):
            path, name =  os.path.split(entry)
            basename, ext = os.path.splitext(name)
            if ext == '.pdf':
                img_files.append(basename)

    # read existing .ly source files in in_dir
    # and add them to the sources list if the image is missing
    src_files = []
    for entry in os.listdir(lg.dir_lysrc):
        entry = os.path.join(lg.dir_lysrc, entry)
        if os.path.isfile(entry):
            path, name = os.path.split(entry)
            basename, ext = os.path.splitext(name)
            if ext == '.ly' and basename not in img_files:
                src_files.append(basename)
    return src_files

def check_paths():
    """Checks if we're in the right CWD
       and makes sure that there is a pdf output directory available"""

    print('Checking directories ...')
    
    # check the presence of the necessary subdirectories
    ls = os.listdir('.')
    if not 'generated_src' in ls:
        print('No LilyPond source files directory found.')
        print('Sorry, there is something wrong :-(')
        print('Current working directory is: ' + os.getcwd())
        print('Please consult the manual.')
        sys.exit(2)
    if not 'pdfs' in ls:
        os.mkdir('pdfs')
    
    print('... done')
    print('')


# ####################################
# Finally launch the program
if __name__ == "__main__":
    # parse command line arguments
    parser = argparse.ArgumentParser(
                      description='Rebuild all pdfs missing in the \'pdfs\' subdiredtory', 
                      parents=[lg.common_arguments])
    args = parser.parse_args()
    main()
