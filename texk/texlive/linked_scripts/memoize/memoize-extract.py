#!/usr/bin/env python3
#
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

import argparse, re, sys, os, subprocess, itertools, traceback, platform
from pathlib import Path, PurePath
# We will only try to import the PDF processing library once we set up the
# error log.

# \paragraph{Messages}

# The messages are written both to the extraction log and the terminal (we
# output to stdout rather than stderr so that messages on the TeX terminal and
# document |.log| appear in chronological order).  Messages are automatically
# adapted to the TeX |--format|.

# The format of the messages.  It depends on the given |--format|; the last
# entry is for t the terminal output.

ERROR = {
    'latex':   r'\PackageError{{{package_name}}}{{{short}}}{{{long}}}',
    'plain':   r'\errhelp{{{long}}}\errmessage{{{package_name}: {short}}}',
    'context': r'\errhelp{{{long}}}\errmessage{{{package_name}: {short}}}',
    None:      '{header}{short}.\n{long}',
}

WARNING = {
    'latex':   r'\PackageWarning{{{package_name}}}{{{texindent}{text}}}',
    'plain':   r'\message{{{package_name}: {texindent}{text}}}',
    'context': r'\message{{{package_name}: {texindent}{text}}}',
    None:      r'{header}{indent}{text}.',
}

INFO = {
    'latex':   r'\PackageInfo{{{package_name}}}{{{texindent}{text}}}',
    'plain':   r'\message{{{package_name}: {texindent}{text}}}',
    'context': r'\message{{{package_name}: {texindent}{text}}}',
    None:      r'{header}{indent}{text}.',    
}

# Some variables used in the message routines; note that |header| will be
# redefined once we parse the arguments.

package_name = 'memoize (python-based extraction)'
exit_code = 0
log = None
header = ''
indent = ''
texindent = ''

# The message routines.

def error(short, long):
    if not args.quiet:
        print(ERROR[None].format(short = short, long = long, header = header))
    if log:
        long = long.replace('\\', '\\string\\')
        print(
            ERROR[args.format].format(
                short = short, long = long, package_name = package_name),
            file = log)
    global exit_code
    exit_code = 11
    endinput()
    
def warning(text):
    if log:
        print(
            WARNING[args.format].format(
                text = text, texindent = texindent, package_name = package_name),
            file = log)
    if not args.quiet:
        print(WARNING[None].format(text = text, header = header, indent = indent))
    global exit_code
    exit_code = 10
    
def info(text):
    if text and not args.quiet:
        print(INFO[None].format(text = text, header = header, indent = indent))
        if log:
            print(
                INFO[args.format].format(
                    text = text, texindent = texindent, package_name = package_name),
                file = log)
            
# Mark the log as complete and exit.
def endinput():
    if log:
        print(r'\endinput', file = log)
        log.close()
    sys.exit(exit_code)

# \paragraph{Permission-related functions}

# |paranoia_in|/|out| should work exactly as
# |kpsewhich -safe-in-name|/|-safe-out-name|.
def paranoia_in(f, remark = ''):
    if f and not _paranoia(f, openin_any):
        error(f"I'm not allowed to read from '{f}' (openin_any = {openin_any})",
              remark)

def paranoia_out(f, remark = ''):
    if f and not _paranoia(f, openout_any):
        error(f"I'm not allowed to write to '{f}' (openout_any = {openout_any})",
              remark)

def _paranoia(f, mode):
    # |mode| is the value of |openin_any| or |openout_any|.  |f| is a
    # |pathlib.Path| object.
    return (
        # In mode `any' (|a|, |y| or |1|), we may access any file.
        mode in 'ay1'
        or (
        # Otherwise, we are at least in the restricted mode, so we should not
        # open dot files on Unix-like systems (except file called |.tex|).
            not (os.name == 'posix' and f.stem.startswith('.') and f.stem != '.tex')
            and (
                # If we are precisely in the restricted mode (|r|, |n|, |0|),
                # then there are no further restrictions.
                mode in 'rn0'
                # Otherwise, we are in the paranoid mode (officially |p|, but
                # any other value is interpreted as |p| as well).  There are
                # two further restrictions in the paranoid mode.
                or (
                    # We're not allowed to go to a parent directory.
                    '..' not in f.parts
                    and
                    # If the given path is absolute, is should be a descendant
                    # of either |TEXMF_OUTPUT_DIRECTORY| or |TEXMFOUTPUT|.
                    (not f.is_absolute()
                     or
                     is_ancestor(texmf_output_directory, f)
                     or
                     is_ancestor(texmfoutput, f)
                     )))))

# On Windows, we disallow ``semi-absolute'' paths, i.e.\ paths starting with
# the |\| but lacking the drive.  On Windows, |pathlib|'s |is_absolute| returns
# |True| only for paths starting with |\| and containing the drive.
def sanitize_filename(f):
    if f and platform.system() == 'Windows' and not (f.is_absolute() or not f.drive):
        error(f"\"Semi-absolute\" paths are disallowed: '{f}'", r"The path must "
              r"either contain both the drive letter and start with '\', "
              r"or none of these; paths like 'C:foo' and '\foo' are disallowed")

def access_in(f):
    return os.access(f, os.R_OK)

# This function can fail on Windows, reporting a non-writable file or dir as
# writable, because |os.access| does not work with Windows' |icacls|
# permissions.  Consequence: we might try to write to a read-only current or
# output directory instead of switching to the temporary directory.  Paranoia
# is unaffected, as it doesn't use |access_*| functions.
def access_out(f):
    try:
        exists = f.exists()
    # Presumably, we get this error when the parent directory is not
    # executable.
    except PermissionError:
        return
    if exists:
        # An existing file should be writable, and if it's a directory, it
        # should also be executable.
        return os.access(f, os.W_OK) and (not f.is_dir() or os.access(f, os.X_OK))
    else:
        # For a non-existing file, the parent directory should be writable.
        # (This is the only place where function |pathlib.parent| is used, so
        # it's ok that it returns the logical parent.)
        return os.access(f.parent, os.W_OK)

# This function finds the location for an input file, respecting
# |TEXMF_OUTPUT_DIRECTORY| and |TEXMFOUTPUT|, and the permissions in the
# filesystem.  It returns an absolute file as-is.  For a relative file, it
# tries |TEXMF_OUTPUT_DIRECTORY| (if defined), the current directory (always),
# and |TEXMFOUTPUT| directory (if defined), in this order.  The first readable
# file found is returned; if no readable file is found, the file in the current
# directory is returned.
def find_in(f):
    sanitize_filename(f)
    if f.is_absolute():
        return f
    for df in (texmf_output_directory / f if texmf_output_directory else None,
               f,
               texmfoutput / f if texmfoutput else None):
        if df and access_in(df):
            return df
    return f

# This function finds the location for an output file, respecting
# |TEXMF_OUTPUT_DIRECTORY| and |TEXMFOUTPUT|, and the permissions in the
# filesystem.  It returns an absolute file as-is.  For a relative file, it
# tries |TEXMF_OUTPUT_DIRECTORY| (if defined), the current directory (unless
# |TEXMF_OUTPUT_DIRECTORY| is defined), and |TEXMFOUTPUT| directory (if
# defined), in this order.  The first writable file found is returned; if no
# writable file is found, the file in either the current or the output
# directory is returned.
def find_out(f):
    sanitize_filename(f)
    if f.is_absolute():
        return f
    for df in (texmf_output_directory / f if texmf_output_directory else None,
               f if not texmf_output_directory else None,
               texmfoutput / f if texmfoutput else None):
        if df and access_out(df):
            return df
    return texmf_output_directory / f if texmf_output_directory else f

# This function assumes that both paths are absolute; ancestor may be |None|,
# signaling a non-path.
def is_ancestor(ancestor, descendant):
    if not ancestor:
        return
    a = ancestor.parts
    d = descendant.parts
    return len(a) < len(d) and a == d[0:len(a)]

# A paranoid |Path.mkdir|.  The given folder is preprocessed by |find_out|.
def mkdir(folder):
    folder = find_out(Path(folder))
    if not folder.exists():
        paranoia_out(folder)
        # Using |folder.mkdir| is fine because we know that
        # |TEXMF_OUTPUT_DIRECTORY|/|TEXMFOUTPUT|, if given, exists, and that
        # ``folder'' contains no |..|.
        folder.mkdir(parents = True, exist_ok = True)
        # This does not get logged when the function is invoked via |--mkdir|,
        # as it is not clear what the log name should be.
        info(f"Created directory {folder}")

_re_unquote = re.compile(r'"(.*?)"')
def unquote(fn):
    return _re_unquote.sub(r'\1', fn)
        
# \paragraph{Kpathsea}

# Get the values of |openin_any|, |openout_any|, |TEXMFOUTPUT| and
# |TEXMF_OUTPUT_DIRECTORY|.

kpsewhich_output = subprocess.run(['kpsewhich',
                                   f'-expand-var='
                                   f'openin_any=$openin_any,'
                                   f'openout_any=$openout_any,'
                                   f'TEXMFOUTPUT=$TEXMFOUTPUT'],
                                  capture_output = True
                                  ).stdout.decode().strip()
if not kpsewhich_output:
    # No TeX? (Note that |kpsewhich| should exist in MiKTeX as well.)  In
    # absence of |kpathsea| information, we get very paranoid, but still try to
    # get |TEXMFOUTPUT| from an environment variable.
    openin_any, openout_any = 'p', 'p'
    texmfoutput, texmf_output_directory = None, None
    # Unfortunately, this warning can't make it into the log.  But then again,
    # the chances of a missing |kpsewhich| are very slim, and its absence would
    # show all over the place anyway.
    warning('I failed to execute "kpsewhich"; , is there no TeX system installed? '
            'Assuming openin_any = openout_any = "p" '
            '(i.e. restricting all file operations to non-hidden files '
            'in the current directory of its subdirectories).')
else:
    m = re.fullmatch(r'openin_any=(.*),openout_any=(.*),TEXMFOUTPUT=(.*)',
                     kpsewhich_output)
    openin_any, openout_any, texmfoutput = m.groups()
    texmf_output_directory = os.environ.get('TEXMF_OUTPUT_DIRECTORY', None)
    if openin_any == '$openin_any':
        # When the |open*_any| variables are not expanded, we assume we're
        # running MiKTeX. The two config settings below correspond to TeXLive's
        # |openin_any| and |openout_any|; afaik, there is no analogue to
        # |TEXMFOUTPUT|.
        initexmf_output = subprocess.run(
            ['initexmf', '--show-config-value=[Core]AllowUnsafeInputFiles',
             '--show-config-value=[Core]AllowUnsafeOutputFiles'],
            capture_output = True).stdout.decode().strip()
        openin_any, openout_any = initexmf_output.split()
        openin_any = 'a' if openin_any == 'true' else 'p'
        openout_any = 'a' if openout_any == 'true' else 'p'
        texmfoutput = None
        texmf_output_directory = None

# An output directory should exist, and may not point to the root on Linux. On
# Windows, it may point to the root, because we only allow absolute filenames
# containing the drive, e.g.\ |F:\|; see |is_absolute|.
def sanitize_output_dir(d_str):
    d = Path(d_str) if d_str else None
    sanitize_filename(d)
    return d if d and d.is_dir() and \
        (not d.is_absolute() or len(d.parts) != 1 or d.drive) else None

texmfoutput = sanitize_output_dir(texmfoutput)
texmf_output_directory = sanitize_output_dir(texmf_output_directory)

class NotExtracted(UserWarning):
    pass

# We don't delve into the real script when loaded from the testing code.
if __name__ == '__main__':

    # \paragraph{Arguments}

    parser = argparse.ArgumentParser(
        description = "Extract extern pages produced by package Memoize "
                      "out of the document PDF.",
        epilog = "For details, see the man page or the Memoize documentation.",
        prog = 'memoize-extract.py',
    )
    parser.add_argument('-P', '--pdf', help = 'extract from file PDF')
    parser.add_argument('-p', '--prune', action = 'store_true',
        help = 'remove the extern pages after extraction')
    parser.add_argument('-k', '--keep', action = 'store_true',
        help = 'do not mark externs as extracted')
    parser.add_argument('-F', '--format', choices = ['latex', 'plain', 'context'],
        help = 'the format of the TeX document invoking extraction')
    parser.add_argument('-f', '--force', action = 'store_true',
        help = 'extract even if the size-check fails')
    parser.add_argument('-q', '--quiet', action = 'store_true',
        help = "describe what's happening")
    parser.add_argument('-m', '--mkdir', action = 'store_true',
        help = 'create a directory (and exit); '
               'mmz argument is interpreted as directory name')
    parser.add_argument('-V', '--version', action = 'version',
        version = f"%(prog)s of Memoize " + __version__)
    parser.add_argument('mmz', help = 'the record file produced by Memoize: '
                                      'doc.mmz when compiling doc.tex '
                                      '(doc and doc.tex are accepted as well)')

    args = parser.parse_args()

    header = parser.prog + ': ' if args.format else ''
    
    # Start a new line in the TeX terminal output.
    if args.format:
        print()

    # \paragraph{Initialization}

    # With |--mkdir|, argument |mmz| is interpreted as the directory to create.
    if args.mkdir:
        mkdir(args.mmz)
        sys.exit()

    # Normalize the |mmz| argument into a |.mmz| filename.
    mmz_file = Path(args.mmz)
    if mmz_file.suffix == '.tex':
        mmz_file = mmz_file.with_suffix('.mmz')
    elif mmz_file.suffix != '.mmz':
        mmz_file = mmz_file.with_name(mmz_file.name + '.mmz')

    # Once we have the |.mmz| filename, we can open the log.
    if args.format:
        log_file = find_out(mmz_file.with_suffix('.mmz.log'))
        paranoia_out(log_file)
        info(f"Logging to '{log_file}'");
        log = open(log_file, 'w')

    # Now that we have opened the log file, we can try loading the PDF
    # processing library.
    try:
        import pdfrw
    except ModuleNotFoundError:
        error("Python module 'pdfrw' was not found",
              'Have you followed the instructions is section 1.1 of the manual?')

    # Catch any errors in the script and output them to the log.
    try:
        
        # Find the |.mmz| file we will read, but retain the original filename
        # in |given_mmz_file|, as we will still need it.
        given_mmz_file = mmz_file
        mmz_file = find_in(mmz_file)
        paranoia_in(mmz_file)
        if not args.keep:
            paranoia_out(mmz_file,
                remark = 'This file is rewritten unless option --keep is given.')
        try:
            mmz = open(mmz_file)
        except FileNotFoundError:
            info(f"File '{given_mmz_file}' does not exist, "
                 f"assuming there's nothing to do")
            endinput()

        # Determine the PDF filename: it is either given via |--pdf|, or
        # constructed from the |.mmz| filename.
        pdf_file = find_in(Path(args.pdf)
                           if args.pdf else given_mmz_file.with_suffix('.pdf'))
        paranoia_in(pdf_file)
        if args.prune:
            paranoia_out(pdf_file,
                remark = 'I would have to rewrite this file '
                         'because option --prune was given.')

        # Various initializations.
        
        re_prefix = re.compile(r'\\mmzPrefix *{(?P<prefix>.*?)}')
        re_split_prefix = re.compile(r'(?P<dir_prefix>.*/)?(?P<name_prefix>.*?)')
        re_newextern = re.compile(
            r'\\mmzNewExtern *{(?P<extern_path>.*?)}{(?P<page_n>[0-9]+)}'
            r'{(?P<expected_width>[0-9.]*)pt}{(?P<expected_height>[0-9.]*)pt}')
        re_extern_path = re.compile(
            r'(?P<dir_prefix>.*/)?(?P<name_prefix>.*?)'
            r'(?P<code_md5sum>[0-9A-F]{32})-'
            r'(?P<context_md5sum>[0-9A-F]{32})(?:-[0-9]+)?.pdf')
        pdf = None
        extern_pages = []
        new_mmz = []
        tolerance = 0.01
        dir_to_make = None
        info(f"Extracting new externs listed in '{mmz_file}' from '{pdf_file}'")
        done_message = "Done (there was nothing to extract)"
        indent = '  '
        texindent = '\space\space '

        # \paragraph{Process \texttt{.mmz}}

        for line in mmz:
            try:
                if m_p := re_prefix.match(line):
                    # Found |\mmzPrefix|: create the extern directory, but only
                    # later, if an extern file is actually produced.  We parse
                    # the prefix in two steps because we have to unquote the
                    # entire prefix.
                    prefix = unquote(m_p['prefix'])
                    if not (m_sp := re_split_prefix.match(prefix)):
                        warning(f"Cannot parse line {line.strip()}")
                    dir_to_make = m_sp['dir_prefix']
                elif m_ne := re_newextern.match(line):
                    # Found |\mmzNewExtern|: extract the extern page into an
                    # extern file.
                    done_message = "Done"
                    # The extern filename, as specified in |.mmz|:
                    unquoted_extern_path = unquote(m_ne['extern_path'])
                    extern_file = Path(unquoted_extern_path)
                    # We parse the extern filename in a separate step because
                    # we have to unquote the entire path.
                    if not (m_ep := re_extern_path.match(unquoted_extern_path)):
                        warning(f"Cannot parse line {line.strip()}")
                    # The actual extern filename:
                    extern_file_out = find_out(extern_file)
                    paranoia_out(extern_file_out)
                    page_n = int(m_ne['page_n'])-1
                    # Check whether c-memo and cc-memo exist (in any input
                    # directory).
                    c_memo = extern_file.with_name(
                        m_ep['name_prefix'] + m_ep['code_md5sum'] + '.memo')
                    cc_memo = extern_file.with_name(
                        m_ep['name_prefix'] + m_ep['code_md5sum']
                        + '-' + m_ep['context_md5sum'] + '.memo')
                    c_memo_in = find_in(c_memo)
                    cc_memo_in = find_in(cc_memo)
                    if not (access_in(c_memo_in) and access_in(cc_memo_in)) \
                       and not args.force:
                        warning(f"I refuse to extract page {page_n+1} into extern "
                                f"'{extern_file}', because the associated c-memo "
                                f"'{c_memo}' and/or cc-memo '{cc_memo}' "
                                f"does not exist")
                        raise NotExtracted()
                    # Load the PDF.  We only do this now so that we don't load
                    # it if there is nothing to extract.
                    if not pdf:
                        if not access_in(pdf_file):
                            warning(f"Cannot open '{pdf_file}'")
                            endinput()
                        try:
                            # All safe, |paranoia_in| was already called above.
                            pdf = pdfrw.PdfReader(pdf_file)
                        except pdfrw.errors.PdfParseError as err:
                            error(rf"File '{pdf_file}' seems corrupted. Perhaps you "
                                  rf"have to load Memoize earlier in the preamble",
                                  f"In particular, Memoize must be loaded before "
                                  f"TikZ library 'fadings' and any package "
                                  f"deploying it, and in Beamer, load Memoize "
                                  f"by writing \RequirePackage{{memoize}} before "
                                  f"\documentclass{{beamer}}. "
                                  f"This was the error thrown by Python: \n{err}")
                    # Does the page exist?
                    if page_n >= len(pdf.pages):
                        error(rf"I cannot extract page {page_n} from '{pdf_file}', "
                              rf"as it contains only {len(pdf.pages)} page" +
                              ('s' if len(pdf.pages) > 1 else ''), '')
                    # Check whether the page size matches the |.mmz|
                    # expectations.
                    page = pdf.pages[page_n]
                    expected_width_pt = float(m_ne['expected_width'])
                    expected_height_pt = float(m_ne['expected_height'])
                    mb = page['/MediaBox']
                    width_bp = float(mb[2]) - float(mb[0])
                    height_bp = float(mb[3]) - float(mb[1])
                    width_pt = width_bp / 72 * 72.27
                    height_pt = height_bp / 72 * 72.27
                    if (abs(width_pt - expected_width_pt) > tolerance
                            or abs(height_pt - expected_height_pt) > tolerance) \
                            and not args.force:
                        warning(
                            f"I refuse to extract page {page_n+1} from '{pdf_file}' "
                            f"because its size ({width_pt}pt x {height_pt}pt) "
                            f"is not what I expected "
                            f"({expected_width_pt}pt x {expected_height_pt}pt)")
                        raise NotExtracted()
                    # All tests were successful, let's create the extern file.
                    # First, the containing directory, if necessary.
                    if dir_to_make:
                        mkdir(dir_to_make)
                        dir_to_make = None
                    # Now the extern file.  Note that |paranoia_out| was
                    # already called above.
                    info(f"Page {page_n+1} --> {extern_file_out}")
                    extern = pdfrw.PdfWriter(extern_file_out)
                    extern.addpage(page)
                    extern.write()
                    # This page will get pruned.
                    if args.prune:
                        extern_pages.append(page_n)
                    # Comment out this |\mmzNewExtern|.
                    if not args.keep:
                        line = '%' + line
            except NotExtracted:
                pass
            finally:
                if not args.keep:
                    new_mmz.append(line)
        mmz.close()
        indent = ''
        texindent = ''
        info(done_message)

        # Write out the |.mmz| file with |\mmzNewExtern| lines commented
        # out. (All safe, |paranoia_out| was already called above.)
        if not args.keep:
            with open(mmz_file, 'w') as mmz:
                for line in new_mmz:
                    print(line, file = mmz, end = '')

        # Remove the extracted pages from the original PDF. (All safe,
        # |paranoia_out| was already called above.)
        if args.prune and extern_pages:
            pruned_pdf = pdfrw.PdfWriter(pdf_file)
            pruned_pdf.addpages(
                page for n, page in enumerate(pdf.pages) if n not in extern_pages)
            pruned_pdf.write()
            info(f"The following extern pages were pruned out of the PDF: " +
                 ",".join(str(page+1) for page in extern_pages))

        # Report that extraction was successful.
        endinput()

    # Catch any errors in the script and output them to the log.
    except Exception as err:
        error(f'Python error: {err}', traceback.format_exc())
        
# Local Variables:
# fill-column: 79
# after-save-hook: py2dtx
# End:
