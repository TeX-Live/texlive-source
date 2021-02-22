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

import os, sys,  getopt,  datetime,  subprocess, argparse, lilyglyphs_common as lg



# ###############
# string to be printed before the actual command
lily_src_prefix = """\\version "2.16.2"

#(set-global-staff-size 14)

\paper {
  indent = 0
}
\header {
  tagline = ""
}

"""

# string to be printed after the actual command definition
lily_src_score = """
  \\score {
  \\new Staff \\with {
    \\remove "Staff_symbol_engraver"
    \\remove "Clef_engraver"
    \\remove "Time_signature_engraver"
  }
"""

def main():
    """Do the actual work of the script"""
    print('')
    print('buildglyphimages.py,')
    print('Part of lilyglyphs.')
    print('')
    
    # set CWD and ensure the necessary subdirs are present
    check_paths()
    print('')

    # load and parse input file
    lg.read_input_file(in_file)
    read_entries()
    print('')

    # generate LilyPond source files for each command
    # and compile them
    write_lily_src_files()
    print('')
    lg.compile_lily_files()
    print('')
    
    # remove intermediate files and move pdfs to pdf directory
    lg.cleanup_lily_files()
    print('')
    
    # generate latex commands and example code
    # and write them to the output file
    lg.generate_latex_commands()
    print('')
    write_latex_file()

    
def check_paths():
    """Sets CWD to 'glyphimages' subdir or root of lilyglyphs_private
       and makes sure that the necessary subdirectories are present"""
    
    # one level above 'definitions' is 'glyphimages'
    os.chdir(os.path.join(in_path, '..'))

    # check the presence of the necessary subdirectories
    # and create them if necessary
    # (otherwise we'll get errors when trying to write in them)
    ls = os.listdir('.')
    if not lg.dir_lysrc in ls:
        os.mkdir(lg.dir_lysrc)
    if not lg.dir_pdfs in ls:
        os.mkdir(lg.dir_pdfs)
    if not lg.dir_cmd in ls:
        os.mkdir(lg.dir_cmd)

def cmd_filename(cmd_name):
    if cmd_name.startswith('lily'):
        return cmd_name[:4] + '-' + cmd_name[4:]
    elif cmd_name.startswith('lily-'):
        return cmd_name
    else:
        return 'lily-' + cmd_name

# set default scale and raise arguments to empty
scale = ''
rais = ''

def read_entries():
    """Parses the input source file and extracts glyph entries"""
    print('Read entries of LilyPond commands:')
    for i in range(len(lg.definitions_file)):
        if '%%lilyglyphs' in lg.definitions_file[i]:
            i = read_entry(i)
    print(lg.lily_files)

def read_entry(i):
    """Reads a single glyph entry from the input file and stores it
    in the global dictionary lg.in_cmds"""
    global scale,  rais
    # read comment line(s)
    i += 1
    is_protected = False
    comment = []
    # loop over the comment line(s)
    while i < len(lg.definitions_file):
        line = lg.definitions_file[i].strip()
        # check for 'protected' entries that shouldn't be processed newly
        if not line[0] == '%':
            break
        elif '%%protected' in line:
            is_protected = True
        elif 'scale=' in line:
            dummy, scale = line.split('=')
        elif 'raise=' in line:
            dummy,  rais = line.split('=')
        else:
            line = line[1:].strip()
            comment.append(line)
        i += 1

    # remove any empty lines
    while len(lg.definitions_file[i].strip()) == 0:
        i += 1

    # read command name
    line = lg.definitions_file[i].strip()
    cmd_name = line[: line.find('=') - 1]
    print('- ' + cmd_name, end=' ')
    if is_protected:
        print('(protected and skipped)')
    else:
        print('') #(for line break only)

    # read actual command until we find a line the begins with a closing curly bracket
    i += 1
    lilySrc = []
    while lg.definitions_file[i][0] != '}':
        lilySrc.append(lg.definitions_file[i])
        i += 1
    if not is_protected:
        lg.in_cmds[cmd_name] = {}
        lg.in_cmds[cmd_name]['comment'] = comment
        lg.in_cmds[cmd_name]['lilySrc'] = lilySrc
        lg.in_cmds[cmd_name]['element'] = cmd_filename(cmd_name)
        lg.in_cmds[cmd_name]['type'] = 'image'
        if scale:
            lg.in_cmds[cmd_name]['scale'] = scale
        if rais:
            lg.in_cmds[cmd_name]['raise'] = rais

        lg.lily_files.append(cmd_filename(cmd_name))
    return i


def usage():
    print("""buildglyphimages. Part of the lilyglyphs package.
    Parses a template file, creates
    single .ly files from it, uses LilyPond to create single glyph
    pdf files and set up template files to be used in LaTeX.
    The input file has to be located in the glyphimages folder
    of either the lilyglyph package itself or of a copy of 
    the lilyglyphs_private directory structure contained in the
    distribution.
    For detailed instructions refer to the manual.
    Usage:
    buildglyphimages.py in-file-name.
    """)

def write_file_info(name, fout):
    """Formats file specific information for the lilyPond source file"""
    long_line = '% This file defines a single glyph to be created with LilyPond: %\n'
    width = len(long_line) - 1
    header = '%' * width + '\n'
    spacer = '%' + ' ' * (width - 2) + '%\n'
    padding = width - len(name) - 8
    fout.write(header)
    fout.write(spacer)
    fout.write(long_line)
    fout.write(spacer)
    fout.write('%   ' + name + '.ly' + ' ' * padding + '%\n')
    fout.write(spacer)
    fout.write(header)
    fout.write(lg.signature())
    fout.write('\n\n')

def write_latex_file():
    """Composes LaTeX file and writes it to disk"""
    print('Generate LaTeX file')
    print(lg.dir_cmd, in_basename)
    lg.write_latex_file(os.path.join(os.getcwd(), lg.dir_cmd,  in_basename + '.tex'))

def write_lily_src_files():
    """Generates one .ly file for each found new command"""
    skip_cmds = []
    print('Write .ly files for each entry:')
    for cmd_name in lg.in_cmds:
        print('- ' + cmd_name)
        gen_src_name = os.path.join(lg.dir_lysrc, cmd_filename(cmd_name) + '.ly')
        # handle existing commands
        if os.path.exists(gen_src_name):
            action = ''
            while not (action == 'Y' or action == 'N'):
                action = input('already present. Overwrite (y/n)? ')
                action = action.upper()
            if action == 'N':
                skip_cmds.append(cmd_name)
                continue
        
        # open a single lily src file for write access
        fout = open(gen_src_name,  'w')

        #output the license information
        fout.write(lg.lilyglyphs_copyright_string)
        fout.write('\n')

        #output information on the actual file
        write_file_info(cmd_name, fout)

        #write the default LilyPond stuff
        fout.write(lily_src_prefix)

        # write the comment for the command
        fout.write('%{\n')
        for line in lg.in_cmds[cmd_name]['comment']:
            fout.write(line + '\n')
        fout.write('%}\n\n')

        # write the actual command
        fout.write(cmd_name + ' = {\n')
        for line in lg.in_cmds[cmd_name]['lilySrc']:
            fout.write(line + '\n')
        fout.write('}\n')

        # write the score definition
        fout.write(lily_src_score)

        # finish the LilyPond file
        fout.write('  \\' + cmd_name + '\n')
        fout.write('}\n\n')

        fout.close()
    
    # remove skipped commands from in_cmds
    print(skip_cmds)
    for cmd_name in skip_cmds:
        del lg.in_cmds[cmd_name]
        lg.lily_files.remove(cmd_filename(cmd_name))

# ####################################
# Finally launch the program
if __name__ == "__main__":
    
    # parse command line arguments
    parser = argparse.ArgumentParser(
                      description='Process templates to LilyPond input files,' +
                      'compile these and generate LaTeX commands.', 
                      parents=[lg.common_arguments])
    parser.add_argument('i', 
                        type=lg.is_file, 
                        metavar='INPUT', 
                        help='File with command templates.')
    args = parser.parse_args()
    
    # if we have exactly one existing file
    # join its components and run the program

    # process filename argument, providing absolute path
    script_path, script_name = os.path.split(sys.argv[0])

    in_file = os.path.join(os.getcwd(), vars(args)['i'])
    # check if the input file is in the right place
    # this has to be the definitions subdir of
    # the package directory or the lilyglyphs_private dir
    in_path, in_filename = os.path.split(in_file)
    in_path = os.path.normpath(in_path)
    if not (('lilyglyphs' in in_path) and (in_path.endswith('definitions'))):
        print('File in the wrong location: ' + in_path)
        usage()
        sys.exit(2)
    in_basename, in_ext = os.path.splitext(in_filename)
    main()
