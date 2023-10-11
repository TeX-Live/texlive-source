#!/usr/bin/env python3

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

__version__ = '2023/10/10 v1.0.0'

import argparse, re, sys, os, pdfrw, subprocess, itertools
from pathlib import Path

parser = argparse.ArgumentParser(
    description = "Extract extern pages out of the document PDF.",
    epilog = "For details, see the man page or the Memoize documentation.",
    prog = 'memoize-extract.py',
)
parser.add_argument('--pdf', '-P', help = 'extract from file PDF')
parser.add_argument('--prune', '-p', action = 'store_true',
    help = 'remove the extern pages after extraction')
parser.add_argument('--keep', '-k', action = 'store_true',
    help = 'do not mark externs as extracted')
parser.add_argument('--force', '-f', action = 'store_true',
    help = 'extract even if the size-check fails')
parser.add_argument('--log', '-l', default = os.devnull, help = 'the log file')
parser.add_argument('--warning-template', '-w', default = '\warningtext',
    help = '\warningtext in the template will be replaced by the warning message')
parser.add_argument('--quiet', '-q', action = 'store_true',
    help = "describe what's happening")
parser.add_argument('--embedded', '-e', action = 'store_true',
    help = "prefix all messages to the standard output with the script name")
parser.add_argument('--mkdir', '-m', action = 'store_true',
    help = 'create a directory (and exit)')
parser.add_argument('mmz',
    help = 'the record file produced by Memoize: doc.mmz when compiling doc.tex')
parser.add_argument('--version', '-V', action = 'version',
                    version = f"%(prog)s of Memoize " + __version__)

args = parser.parse_args()

message_prefix = parser.prog + ': ' if args.embedded else ''
if args.embedded:
    print()

# Even a bit more paranoid than required:
# (a) disallowing TEXMFOUTPUT=/ (while allowing C:\ on Windows)
# (b) waiting for access to '-output-directory'.
output_paths = [Path.cwd()]
if texmfoutput := subprocess.run(
        ['kpsewhich', '--var-value=TEXMFOUTPUT'],
        capture_output = True).stdout.decode().strip():
    texmfoutput_dir = Path(texmfoutput).resolve()
    if len(texmfoutput_dir.parts) != 1 or texmfoutput_dir.drive:
        output_paths.append(texmfoutput_dir)

def paranoia(f):
    p = Path(f).resolve()
    if p.stem.startswith('.'):
        raise RuntimeError(f"{message_prefix}Cannot create a hidden file or dir: {f}")
    if not any(p.is_relative_to(op) for op in output_paths):
        raise RuntimeError(f"{message_prefix}Cannot write outside the current working or output directory tree: {f}")

mmz_file = Path(args.mmz)
    
if args.mkdir: # Here, argument "mmz" is interpreted as the directory to create.
    # We cannot simply say
    # paranoia(mmz_file)
    # mmz_file.mkdir(parents = True, exist_ok = True)
    # because have be paranoid about the intermediate directories as well:
    # memoize-extract.py -m a/../../BAD/../<cwd-name>/b
    # Note that paranoia might kick in only after a part of the given path was
    # already created.  This is in accord to how "mkdir" behaves wrt existing
    # files.
    for folder in itertools.chain(reversed(mmz_file.parents), (mmz_file,)):
        if not folder.is_dir():
            paranoia(folder)
            folder.mkdir(exist_ok = True)
            if not args.quiet:
                print(f"{message_prefix}Created directory {folder}")
    sys.exit()
elif mmz_file.suffix != '.mmz':
    raise RuntimeError(f"{message_prefix}The 'mmz' argument should be a file with suffix '.mmz', not '{mmz_file}'")

mmz_dir = mmz_file.parent
pdf_file = Path(args.pdf) if args.pdf else mmz_file.with_suffix('.pdf')
paranoia(pdf_file)
pdf = None
extern_pages = []
new_mmz = []
args.log is os.devnull or paranoia(Path(args.log))
re_newextern = re.compile(r'\\mmzNewExtern *{(?P<extern_fn>(?P<prefix>.*?)(?P<code_md5sum>[0-9A-F]{32})-(?P<context_md5sum>[0-9A-F]{32})(?:-[0-9]+)?.pdf)}{(?P<page_n>[0-9]+)}{(?P<expected_width>[0-9.]*)pt}{(?P<expected_height>[0-9.]*)pt}')
tolerance = 0.01
done_message = f"{message_prefix}Done (there was nothing to extract)."

# Complication: I want to use 'with', but don't have to open stderr.
with open(args.log, 'w') as log:
    log = sys.stderr if args.log is os.devnull else log
    try:
        mmz = mmz_file.open()
    except FileNotFoundError:
        print(f'''{message_prefix}File "{mmz_file}" does not exist, assuming there's nothing to do.''',
              file = sys.stderr)
    else:
        if not args.quiet:
            print(f"{message_prefix}Extracting externs from {pdf_file}")
        for line in mmz:
            if m := re_newextern.match(line):
                extern_file = mmz_dir / m['extern_fn']
                paranoia(extern_file)
                page_n = int(m['page_n'])-1
                c_memo = mmz_dir / (m['prefix'] + m['code_md5sum'] + '.memo')
                cc_memo = mmz_dir / (m['prefix'] + m['code_md5sum'] + '-' + m['context_md5sum'] + '.memo')
                if not (c_memo.exists() and cc_memo.exists()):
                    print(args.warning_template.replace('\warningtext', f'Not extracting page {page_n} into extern {extern_file}, because the associated (c)c-memo does not exist'), file = log)
                    continue
                if not pdf:
                    try:
                        pdf = pdfrw.PdfReader(pdf_file)
                    except pdfrw.errors.PdfParseError:
                        print(f'{message_prefix}File "{pdf_file}" cannot be read, bailing out.', file = sys.stderr)
                        print(args.warning_template.replace('\warningtext', f'Cannot read file "{pdf_file}". Perhaps you have to load Memoize earlier in the preamble?'), file = log)
                        args.keep = True
                        break
                extern = pdfrw.PdfWriter(extern_file)
                page = pdf.pages[page_n]
                expected_width_pt, expected_height_pt = float(m['expected_width']), float(m['expected_height'])
                mb = page['/MediaBox']
                width_bp, height_bp = float(mb[2]) - float(mb[0]), float(mb[3]) - float(mb[1])
                width_pt = width_bp / 72 * 72.27
                height_pt = height_bp / 72 * 72.27
                warning = None
                if abs(width_pt - expected_width_pt) > tolerance \
                   or abs(height_pt - expected_height_pt) > tolerance:
                    warning = (
                        f'I refuse to extract page {page_n+1} from "{pdf_file}", '
                        f'because its size ({width_pt}pt x {height_pt}pt) is not '
                        f'what I expected ({expected_width_pt}pt x {expected_height_pt}pt)')
                    print(args.warning_template.replace('\warningtext', warning), file = log)
                if warning and not args.force:
                    extern_file.unlink(missing_ok = True)
                else:
                    extern.addpage(page)
                    if not args.quiet:
                        print(f"{message_prefix}  Page {page_n+1} --> {extern_file}", file = sys.__stdout__)
                    extern.write()
                    done_message = f"{message_prefix}Done."
                    if args.prune:
                        extern_pages.append(page_n)
                    if not args.keep:
                        line = '%' + line
            if not args.keep:
                new_mmz.append(line)
        mmz.close()
        if not args.quiet:
            print(done_message)
        if not args.keep:
            paranoia(mmz_file)
            with open(mmz_file, 'w') as mmz:
                for line in new_mmz:
                    print(line, file = mmz, end = '')
        if args.prune and extern_pages:
            pruned_pdf = pdfrw.PdfWriter(pdf_file)
            pruned_pdf.addpages(
                page for n, page in enumerate(pdf.pages) if n not in extern_pages)
            pruned_pdf.write()
            if not args.quiet:
                print(f"{message_prefix}The following extern pages were pruned out of the PDF:",
                      ",".join(str(page+1) for page in extern_pages))
    if args.log is not os.devnull:
        print(r'\endinput', file = log)
