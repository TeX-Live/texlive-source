#! /usr/bin/env python3

"""
    A script to check the syllabation of a gabc file

    See checkSyllabation.py -h for help

    Copyright (C) 2016-2019 Elie Roux

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in
    the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished to do
    so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    Depends on pyphen: http://pyphen.org/
    You also need the hyph_la_liturgical.dic file. To get it, get the
      hyphen-la project on https://github.com/gregorio-project/hyphen-la
      and run "make" in the "patterns" directory.

"""

import sys
import re
import argparse
import pyphen
import os
import glob

DEFAULT_OUTFILE = False
if os.name == 'nt':
    DEFAULT_OUTFILE = 'check-syllabation.log'

def get_parser():
    "Return command line parser"
    parser = argparse.ArgumentParser(
        description='A script to check the syllabation of gabc files')
    parser.add_argument('-p', '--pat-path',
                        help='hyphenation pattern file',
                        action='store', default="hyph_la_liturgical.dic")
    parser.add_argument('-o', '--outfile',
                        help='The file that will contain the report',
                        action='store', default=DEFAULT_OUTFILE)
    parser.add_argument('-v', '--verbose',
                        help='Report also files without error',
                        action='store_true', default=False)
    parser.add_argument('path',
                        help='Path to a gabc file or a directory containing gabc files',
                        action='store', default=".")
    return parser

def deacc(accstr):
  return accstr.replace('á', 'a').replace('é', 'e').replace('í', 'i').replace('ó', 'o').replace('ú', 'u').replace('ý', 'y').replace('́', '').replace('ǽ', 'æ')

def checkwords(words_list, hyphenator):
    errors = []
    for word in words_list:
        initialword = deacc(word.lower())
        if initialword.find('-') == -1:
            # no need for noise
            continue
        correctword = hyphenator.inserted(initialword.replace('-',''))
        if correctword != initialword:
            errors.append( (initialword,correctword) )
    return errors

def get_words_list(gabc_content):
    gabc_content = gabc_content.split('%%\n', 1)[1] # no headers
    gabc_content = re.sub(r'%.*\n', '', gabc_content)
    gabc_content = gabc_content.replace('\n', ' ').replace('\r', ' ').replace('{','')
    gabc_content = gabc_content.replace('}','').replace('<sp>\'ae</sp>', 'ǽ')
    gabc_content = gabc_content.replace('<sp>oe</sp>', 'œ').replace('<sp>\'oe</sp>', 'œ')
    gabc_content = gabc_content.replace('<sp>ae</sp>', 'æ').replace('<sp>\'æ</sp>', 'ǽ')
    gabc_content = gabc_content.replace('<sp>\'œ</sp>', 'œ')
    gabc_content = re.sub(r'\([^\)]*\)', '-', gabc_content)
    gabc_content = re.sub(r'<\/?[ibuec]>', '', gabc_content)
    gabc_content = re.sub(r'<\/?sc>', '', gabc_content)
    gabc_content = re.sub(r'<\/?eu>', '', gabc_content)
    gabc_content = re.sub(r'<v>[^<]*<\/v>', '', gabc_content)
    gabc_content = re.sub(r'<v>[^>]*<\/v>', '', gabc_content)
    gabc_content = re.sub(r'<sp>[^>]*<\/sp>', '', gabc_content)
    gabc_content = re.sub(r'<alt>[^>]*<\/alt>', '', gabc_content)
    gabc_content = re.sub(r'\[[^\]]*\]', '', gabc_content)
    gabc_content = re.sub(r'-+', '-', gabc_content)
    gabc_content = re.sub(r'-?(\s+|$)', ' ', gabc_content)
    gabc_content = re.sub(r'[^a-záéíóæúýœǽ\u0301 -]', '', gabc_content, flags=re.IGNORECASE)
    gabc_content = re.sub(r'(^|\s+)-', ' ', gabc_content)
    return gabc_content.split()

def get_file_list(path):
    if os.path.isfile(path):
        return [path]
    elif os.path.isdir(path):
        files = glob.glob(os.path.join(path, '**/*.gabc'), recursive=True)
        files = sorted(files)
        return files
    else:
        print('Error! Cannot find '+path, file=sys.stderr)
        sys.exit(1)

def check_file(filepath, hyphenator, outfd, report_no_error=False):
    words_list = []
    with open(filepath, 'r', encoding='utf8') as inputf:
        words_list = get_words_list(inputf.read())
    errors = checkwords(words_list, hyphenator)
    nb_errors = len(errors)
    if nb_errors > 0 or report_no_error :
        outfd.write('analyzing '+filepath+':\n')
        if nb_errors == 0:
            outfd.write('  no error!\n')
        else:
            for t in errors:
                outfd.write('  '+t[0]+' should be '+t[1]+'\n')
        outfd.write('\n')
    return nb_errors

def main():
    "Main function"
    parser = get_parser()
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(1)
    args = parser.parse_args()
    hyphenator = pyphen.Pyphen(filename=args.pat_path,left=1,right=1)
    outfd = sys.stdout
    if args.outfile != False:
        outfd = open(args.outfile, 'w', encoding='utf8')
    file_list = get_file_list(args.path)
    nb_errors = 0
    for f in file_list:
        nb_errors += check_file(f, hyphenator, outfd, args.verbose)
    if len(file_list) > 1 and nb_errors > 0:
        outfd.write('Total errors: '+str(nb_errors)+'\n')
    elif nb_errors == 0 and not args.verbose:
        outfd.write('No error!\n')
    if outfd is not sys.stdout:
        outfd.close()
    if nb_errors == 0:
        sys.exit(0)
    else:
        sys.exit(2)

if __name__ == "__main__":
    main()
