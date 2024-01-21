#!/usr/bin/env python

# This file is a part of Memoize, a TeX package for externalization of
# graphics and memoization of compilation results in general, available at
# https://ctan.org/pkg/memoize and https://github.com/sasozivanovic/memoize.
#
# Copyright (c) 2020- Saso Zivanovic <saso.zivanovic@guest.arnes.si>
#
# This work may be distributed and/or modified under the conditions of the
# LaTeX Project Public License, either version 1.3c of this license or (at
# your option) any later version.  The latest version of this license is in
# https://www.latex-project.org/lppl.txt and version 1.3c or later is part of
# all distributions of LaTeX version 2008 or later.
#
# This work has the LPPL maintenance status `maintained'.
# The Current Maintainer of this work is Saso Zivanovic.
# 
# The files belonging to this work and covered by LPPL are listed in
# <texmf>/doc/generic/memoize/FILES.

__version__ = '2024/01/21 v1.1.2'

import argparse, re, sys, pathlib, os

parser = argparse.ArgumentParser(
    description="Remove (stale) memo and extern files.",
    epilog = "For details, see the man page or the Memoize documentation "
             "(https://ctan.org/pkg/memoize)."
)
parser.add_argument('--yes', '-y', action = 'store_true',
                    help = 'Do not ask for confirmation.')
parser.add_argument('--all', '-a', action = 'store_true',
                    help = 'Remove *all* memos and externs.')
parser.add_argument('--quiet', '-q', action = 'store_true')
parser.add_argument('--prefix', '-p', action = 'append', default = [],
    help = 'A path prefix to clean; this option can be specified multiple times.')
parser.add_argument('mmz', nargs= '*', help='.mmz record files')
parser.add_argument('--version', '-V', action = 'version',
                    version = f"%(prog)s of Memoize " + __version__)
args = parser.parse_args()

re_prefix = re.compile(r'\\mmzPrefix *{(.*?)}')
re_memo = re.compile(r'%? *\\mmz(?:New|Used)(?:CC?Memo|Extern) *{(.*?)}')
re_endinput = re.compile(r' *\\endinput *$')

prefixes = set(pathlib.Path(prefix).resolve() for prefix in args.prefix)
keep = set()

# We loop through the given .mmz files, adding prefixes to whatever manually
# specified by the user, and collecting the files to keep.
for mmz_fn in args.mmz:
    mmz = pathlib.Path(mmz_fn)
    mmz_parent = mmz.parent.resolve()
    try:
        with open(mmz) as mmz_fh:
            prefix = ''
            endinput = False
            empty = None
            for line in mmz_fh:
                line = line.strip()
                
                if not line:
                    pass
                
                elif endinput:
                    raise RuntimeError(
                        rf'Bailing out, '
                        rf'\endinput is not the last line of file {mmz_fn}.')
                
                elif m := re_prefix.match(line):
                    prefix = m[1]
                    prefixes.add( (mmz_parent/prefix).resolve() )
                    if empty is None:
                        empty = True

                elif m := re_memo.match(line):
                    if not prefix:
                        raise RuntimeError(
                            f'Bailing out, no prefix announced before file "{m[1]}".')
                    if not m[1].startswith(prefix):
                        raise RuntimeError(
                            f'Bailing out, prefix of file "{m[1]}" does not match '
                            f'the last announced prefix ({prefix}).')
                    keep.add((mmz_parent / m[1]))
                    empty = False

                elif re_endinput.match(line):
                    endinput = True
                    continue

                else:
                    raise RuntimeError(fr"Bailing out, "
                        fr"file {mmz_fn} contains an unrecognized line: {line}")
            
        if empty and not args.all:
            raise RuntimeError(fr'Bailing out, file {mmz_fn} is empty.')

        if not endinput and empty is not None and not args.all:
            raise RuntimeError(
                fr'Bailing out, file {mmz_fn} does not end with \endinput; '
                fr'this could mean that the compilation did not finish properly. '
                fr'You can only clean with --all.'
            )
        
    # It is not an error if the file doesn't exist.
    # Otherwise, cleaning from scripts would be cumbersome.
    except FileNotFoundError:
        pass

tbdeleted = []
def populate_tbdeleted(folder, basename_prefix):
    re_aux = re.compile(
        re.escape(basename_prefix) + 
        '[0-9A-F]{32}(?:-[0-9A-F]{32})?'
        '(?:-[0-9]+)?(?:\.memo|(?:-[0-9]+)?\.pdf|\.log)$')
    try:
        for f in folder.iterdir():
            if re_aux.match(f.name) and (args.all or f not in keep):
                tbdeleted.append(f)
    except FileNotFoundError:
        pass

for prefix in prefixes:
    # "prefix" is interpreted both as a directory (if it exists) and a basename prefix.
    if prefix.is_dir():
        populate_tbdeleted(prefix, '')
    populate_tbdeleted(prefix.parent, prefix.name)

allowed_dirs = [pathlib.Path().absolute()] # todo: output directory
deletion_not_allowed = [f for f in tbdeleted if not f.is_relative_to(*allowed_dirs)]
if deletion_not_allowed:
    raise RuntimeError("Bailing out, "
        "I was asked to delete these files outside the current directory:\n" +
        "\n".join(str(f) for f in deletion_not_allowed))
        
_cwd_absolute = pathlib.Path().absolute()
def relativize(path):
    try:
        return path.relative_to(_cwd_absolute)
    except ValueError:
        return path

if tbdeleted:
    tbdeleted.sort()
    if not args.yes:
        print('I will delete the following files:')
        for f in tbdeleted:
            print(relativize(f))
        print("Proceed (y/n)? ")
        a = input()
    if args.yes or a == 'y' or a == 'yes':
        for f in tbdeleted:
            if not args.quiet:
                print("Deleting", relativize(f))
            try:
                f.unlink()
            except FileNotFoundError:
                print(f"Cannot delete {f}")
    else:
        print("Bailing out.")
elif not args.quiet:
    print('Nothing to do, the directory seems clean.')

# Local Variables:
# fill-column: 79
# after-save-hook: py2dtx
# End:
