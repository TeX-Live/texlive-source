#! /usr/bin/env python2

"""
    A script that manages the VERSION of gregorio.

    See VersionUpdate.py -h for help

    Copyright (C) 2015-2019 The Gregorio Project (see CONTRIBUTORS.md)

    This file is part of Gregorio.

    Gregorio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Gregorio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Gregorio.  If not, see <http://www.gnu.org/licenses/>.
"""

from __future__ import print_function

import sys
import re
import argparse
import subprocess
import time
import os
import locale
import linecache
from datetime import date

from distutils.util import strtobool

locale.setlocale(locale.LC_TIME, 'C')

os.chdir(sys.path[0])

VERSION_FILE = '.gregorio-version'
CURRENTYEAR = str(date.today().year)
GREGORIO_FILES = ["configure.ac",
                  "ctan-o-mat.config",
                  "windows/gregorio-resources.rc",
                  "macosx/Gregorio.pkgproj",
                  "macosx/douninstall.sh",
                  "windows/gregorio.iss",
                  "doc/GregorioRef.tex",
                  "tex/gregoriotex.sty",
                  "tex/gregoriotex.lua",
                  "tex/gregoriotex.tex",
                  "tex/gregoriotex-chars.tex",
                  "tex/gregoriotex-signs.tex",
                  "tex/gregoriotex-signs.lua",
                  "tex/gregoriotex-spaces.tex",
                  "tex/gregoriotex-symbols.tex",
                  "tex/gregoriotex-symbols.lua",
                  "tex/gregoriotex-syllable.tex",
                  "tex/gregoriotex-main.tex",
                  "tex/gregoriotex-nabc.tex",
                  "tex/gregoriotex-nabc.lua",
                  "tex/gregoriosyms.sty",
                  "tex/gregoriotex-common.tex",
                  "fonts/squarize.py",
                 ]
COPYRIGHT_FILES = ["install-gtex.sh",
                   "build-ctan.sh",
                   "ctan_upload.sh",
                   "tex/gregoriotex-signs.tex",
                   "tex/gregorio-vowels.dat",
                   "tex/gsp-default.tex",
                   "tex/gregoriotex-nabc.lua",
                   "tex/gregoriotex-symbols.lua",
                   "tex/gregoriotex-chars.tex",
                   "tex/gregoriotex-main.tex",
                   "tex/gregoriotex-spaces.tex",
                   "tex/Makefile.am",
                   "tex/gregoriotex-common.tex",
                   "tex/gregoriotex-syllable.tex",
                   "tex/gregoriotex.lua",
                   "tex/gregoriotex.sty",
                   "tex/gregoriosyms.sty",
                   "tex/gregoriotex-nabc.tex",
                   "tex/gregoriotex.tex",
                   "tex/gregoriotex-signs.lua",
                   "tex/gregoriotex-symbols.tex",
                   "contrib/TeXShop/Makefile.am",
                   "contrib/900_gregorio.xml",
                   "contrib/Makefile.am",
                   "contrib/gabc.lang",
                   "contrib/gprocess",
                   "contrib/checkSyllabation.py",
                   "configure.ac",
                   "examples/Makefile.am",
                   "Makefile.am",
                   "install.sh",
                   "debian/copyright",
                   "debian/manpage.xml",
                   "doc/Command_Index_User.tex",
                   "doc/Makefile.am",
                   "doc/GregorioRef.tex",
                   "doc/Command_Index_gregorio.tex",
                   "doc/Command_Index_internal.tex",
                   "doc/GregorioNabcRef.tex",
                   "doc/Gabc.tex",
                   "doc/GregorioRef.lua",
                   "doc/Appendix_Font_Tables.tex",
                   "VersionManager.py",
                   "coverage.sh",
                   "COPYING.md",
                   "src/characters.h",
                   "src/plugins.h",
                   "src/gregoriotex/gregoriotex-write.c",
                   "src/gregoriotex/gregoriotex.h",
                   "src/gregoriotex/gregoriotex-position.c",
                   "src/unicode.c",
                   "src/unicode.h",
                   "src/messages.h",
                   "src/support.c",
                   "src/sha1.h",
                   "src/messages.c",
                   "src/support.h",
                   "src/sha1.c",
                   "src/struct.h",
                   "src/bool.h",
                   "src/struct_iter.h",
                   "src/Makefile.am",
                   "src/dump/dump.c",
                   "src/encode_utf8strings.c",
                   "src/enum_generator.h",
                   "src/gabc/gabc-score-determination.c",
                   "src/gabc/gabc-elements-determination.c",
                   "src/gabc/gabc-write.c",
                   "src/gabc/gabc-notes-determination.l",
                   "src/gabc/gabc.h",
                   "src/gabc/gabc-score-determination.l",
                   "src/gabc/gabc-score-determination.y",
                   "src/gabc/gabc-score-determination.h",
                   "src/gabc/gabc-glyphs-determination.c",
                   "src/utf8strings.h.in",
                   "src/config.h",
                   "src/characters.c",
                   "src/vowel/vowel-rules.y",
                   "src/vowel/vowel.h",
                   "src/vowel/vowel-rules.h",
                   "src/vowel/vowel-rules.l",
                   "src/vowel/vowel.c",
                   "src/gregorio-utils.c",
                   "src/struct.c",
                   "fonts/gregorio-base.sfd",
                   "fonts/granapadano-base.sfd",
                   "fonts/squarize.py",
                   "fonts/convertsfdtottf.py",
                   "fonts/Makefile.am",
                   "fonts/simplify.py",
                   "fonts/stemsschemas.py",
                   "fonts/greciliae-base.sfd",
                   "fonts/gregall.sfd",
                   "fonts/grelaon.sfd",
                   "fonts/gresgmodern.sfd",
                   "fonts/install_supp_fonts.lua",
                   "windows/gregorio.iss",
                   "windows/install.lua",
                   "windows/uninstall.lua",
                  ]

def get_parser():
    "Return command line parser"
    parser = argparse.ArgumentParser(
        description='A script to manage the VERSION of gregorio.')
    parser.add_argument('-ni', '--not-interactive',
                        help='Do not ask confirmation',
                        action='store_true', default=False)
    parser.add_argument('-c', '--get-current',
                        help='Prints the current gregorio version',
                        action='store_true', default=False)
    parser.add_argument('-d', '--get-debian-stable',
                        help='Prints the version for Debian stable package',
                        action='store_true', default=False)
    parser.add_argument('-dg', '--get-debian-git',
                        help='Prints the version for Debian git package',
                        action='store_true', default=False)

    modify = parser.add_mutually_exclusive_group()
    modify.add_argument('-b', '--beta',
                        help='Increment -betax+1',
                        action='store_true', default=False,
                        dest='beta')
    modify.add_argument('--manual=',
                        help='Manually set the version.',
                        action='store',
                        dest='manual_version')
    modify.add_argument('-m', '--major',
                        help='Increment the major version: x+1.0.0-beta1',
                        action='store_true', default=False,
                        dest='major')
    modify.add_argument('-e', '--enhancement',
                        help='Increment the minor version: x.y+1.0-beta1',
                        action='store_true', default=False,
                        dest='minor')
    modify.add_argument('-p', '--patch',
                        help='Increment the patch version: x.y.z+1',
                        action='store_true', default=False,
                        dest='patch')
    modify.add_argument('-rc', '--release-candidate',
                        help='Change version to a -rc1, or increment -rcx+1',
                        action='store_true', default=False,
                        dest='release_candidate')
    modify.add_argument('-r', '--release',
                        help='Make a release. Removes -rcx',
                        action='store_true', default=False,
                        dest='release')
    return parser

class Version(object):
    "Class for version manipulation."

    def __init__(self, versionfile):
        self.versionfile = versionfile
        self.version = linecache.getline(self.versionfile, 1).strip('\n')
        self.filename_version = self.filename_version_from_version(self.version)
        self.short_tag = None
        self.date = None
        self.binary_version = self.binary_version_from_version(self.version)

    def filename_version_from_version(self, version):
        "Return filename-compatible version"
        return version.replace('.', '_')

    def binary_version_from_version(self, version):
        "Return binary version number for Windows FILEVERSION"
        binary = version.replace('.', ',')
        if '-' in binary:
            binary = binary.replace('-beta', ',1')
            binary = binary.replace('-rc', ',2')
        else:
            binary += ',30'
        return binary

    def fetch_version(self):
        "Prints version"
        print(self.version)
        sys.exit(0)

    def fetch_version_debian_stable(self):
        "Prints version for Debian stable package and exits"
        print(self.version.replace('-', '~'))
        sys.exit(0)

    def fetch_version_debian_git(self):
        "Prints version for Debian git package and exits"
        self.short_tag = subprocess.check_output(
            ['git', 'rev-parse', '--short', 'HEAD'])
        self.short_tag = self.short_tag.strip('\n')
        self.date = time.strftime("%Y%m%d%H%M%S")
        print("{0}+git{1}+{2}".format(self.version.replace('-', '~'),
                                      self.date, self.short_tag))
        sys.exit(0)

    def update_version(self, newversion):
        "Update self.version and .gregorio-version with the new version."
        self.version = newversion
        self.filename_version = self.filename_version_from_version(newversion)
        self.binary_version = self.binary_version_from_version(newversion)
        print('Updating {0} with the new version: {1}\n'.format(
            self.versionfile, self.version))
        with open(self.versionfile, 'w') as verfile:
            verfile.write('{0}\n{1}'.format(self.version, CURRENTYEAR))
            verfile.write('\n\n*** Do not modify this file. ***\n')
            verfile.write('Use VersionManager.py to change the version.\n')

def replace_version(version_obj):
    "Change version in file according to heuristics."
    newver = version_obj.version
    newver_filename = version_obj.filename_version
    newbinver = version_obj.binary_version
    today = date.today()
    print('Updating source files to version {0}\n'.format(newver))
    for myfile in GREGORIO_FILES:
        result = []
        following_line_filename = False
        with open(myfile, 'r') as infile:
            for line in infile:
                if 'AC_INIT([' in line:
                    result.append(re.sub(r'(\d+\.\d+\.\d+(?:[-+~]\w+)*)', newver, line, 1))
                elif 'AppVersion' in line:
                    result.append(re.sub(r'(\d+\.\d+\.\d+(?:[-+~]\w+)*)', newver, line, 1))
                elif 'FILENAME_VERSION' in line:
                    result.append(re.sub(r'(\d+\_\d+\_\d+(?:[-+~]\w+)*)', newver_filename, line, 1))
                elif 'FileVersion' in line:
                    result.append(re.sub(r'(\d+\.\d+\.\d+(?:[-+~]\w+)*)', newver, line, 1))
                elif 'ProductVersion' in line:
                    result.append(re.sub(r'(\d+\.\d+\.\d+(?:[-+~]\w+)*)', newver, line, 1))
                elif 'GREGORIO_VERSION' in line:
                    result.append(re.sub(r'(\d+\.\d+\.\d+(?:[-+~]\w+)*)', newver, line, 1))
                elif 'GREGORIO_DATE_LTX' in line:
                    result.append(re.sub(r'(\d+\/\d+/\d+)', today.strftime("%Y/%m/%d"), line, 1))
                elif 'PARSE_VERSION_DATE_LTX' in line:
                    newline = re.sub(r'(\d+\.\d+\.\d+(?:[-+~]\w+)*)', newver, line, 1)
                    result.append(re.sub(r'(\d+\/\d+/\d+)', today.strftime("%Y/%m/%d"), newline, 1))
                elif 'PARSE_VERSION_DATE' in line:
                    newline = re.sub(r'(\d+\.\d+\.\d+(?:[-+~]\w+)*)', newver, line, 1)
                    result.append(re.sub(r'(\d{1,2} [A-Z][a-z]+ \d{4})', today.strftime("%-d %B %Y"), newline, 1))
                elif 'FILEVERSION' in line:
                    result.append(re.sub(r'\d+,\d+,\d+,\d+', newbinver, line, 1))
                elif 'PRODUCTVERSION' in line:
                    result.append(re.sub(r'\d+,\d+,\d+,\d+', newbinver, line, 1))
                elif 'PARSE_VERSION_FILE_NEXTLINE' in line:
                    result.append(line)
                    following_line_filename = True
                elif following_line_filename:
                    result.append(re.sub(r'(\d+\_\d+\_\d+(?:[-+~]\w+)*)', newver_filename, line, 1))
                    following_line_filename = False
                else:
                    result.append(line)
        with open(myfile, 'w') as outfile:
            outfile.write(''.join(result))
    sys.exit(0)

def update_changelog(newver,upgradetype):
    today = date.today()
    with open('CHANGELOG.md', 'r') as infile:
        result = []
        develop = False
        for line in infile:
            if upgradetype == "patch":
                if '[Unreleased][develop]' in line:
                    print("Found an unreleased develop section.")
                    print("Patch releases should be based on ctan branch.")
                    sys.exit(1)
                if '[Unreleased][CTAN]' in line:
                    result.append(line)
                    result.append('\n')
                    result.append('\n')
                    newline = '## [' + newver + '] - ' + today.strftime("%Y-%m-%d") + '\n'
                    result.append(newline)
                else:
                    result.append(line)
            else:
                if '[Unreleased][develop]' in line:
                    develop = True
                    result.append(line)
                    result.append('\n')
                    result.append('\n')
                    result.append('## [Unreleased][CTAN]\n')
                    result.append('\n')
                    result.append('\n')
                    newline = '## [' + newver + '] - ' + today.strftime("%Y-%m-%d") + '\n'
                    result.append(newline)
                elif '[Unreleased][CTAN]' in line and develop:
                    continue
                else:
                    result.append(line)
        if not develop and upgradetype != "patch":
            print("I didn't find a unreleased develop section.")
            print("Non-patch releases should be based on develop branch.")
            sys.exit(1)
    with open('CHANGELOG.md', 'w') as outfile:
        outfile.write(''.join(result))

def confirm_replace(oldver, newver):
    "Query the user to confirm action"
    query = 'Update version from {0} --> {1} [y/n]?'.format(oldver, newver)
    print(query)
    consent = None
    while True:
        try:
            consent = strtobool(raw_input().lower())
            break
        except ValueError:
            print('Answer with y or n.')
    if not consent:
        print('Aborting.')
        sys.exit(1)

def release_candidate(version_obj, not_interactive):
    "Changes x.y.z-beta to x.y.z-rc1 OR increments x.y.z-rcx+1"
    oldversion = version_obj.version
    if '-rc' in oldversion:
        newversion = re.sub(r'\d+$', lambda x: str(int(x.group()) +1),
                            oldversion)
    elif '-' in oldversion:
        newversion = re.sub(r'-.*', '-rc1', oldversion)
    if (not not_interactive):
        confirm_replace(oldversion, newversion)
    update_changelog(newversion,"releasecandidate")
    version_obj.update_version(newversion)
    replace_version(version_obj)

def beta(version_obj, not_interactive):
    "Increments x.y.z-betax+1"
    oldversion = version_obj.version
    if '-beta' in oldversion:
        newversion = re.sub(r'\d+$', lambda x: str(int(x.group()) +1),
                            oldversion)
    else:
        print('Version number not in beta stage')
        print('Use --major (-m) or --enhancement (-e) to start a new beta')
        sys.exit(1)
    if (not not_interactive):
        confirm_replace(oldversion, newversion)
    update_changelog(newversion,"beta")
    version_obj.update_version(newversion)
    replace_version(version_obj)

def bump_major(version_obj, not_interactive):
    "Changed the major version number: x.y.z --> x+1.0.0-beta1"
    oldversion = version_obj.version
    nums = re.search(r'(\d+)(\.\d+)(\.\d+)', oldversion)
    newversion = str(int(nums.group(1)) +1) + '.0.0-beta1'
    if (not not_interactive):
        confirm_replace(oldversion, newversion)
    update_changelog(newversion,"major")
    version_obj.update_version(newversion)
    replace_version(version_obj)

def bump_minor(version_obj, not_interactive):
    "Changed the minor version number: x.y.z --> x.y+1.0-beta1"
    oldversion = version_obj.version
    nums = re.search(r'(\d+\.)(\d+)(\.\d+)', oldversion)
    newversion = nums.group(1) + str(int(nums.group(2)) +1) + '.0-beta1'
    if (not not_interactive):
        confirm_replace(oldversion, newversion)
    update_changelog(newversion,"minor")
    version_obj.update_version(newversion)
    replace_version(version_obj)

def bump_patch(version_obj, not_interactive):
    "Changed the patch version number: x.y.z --> x.y.z+1"
    oldversion = version_obj.version
    nums = re.search(r'(\d+\.\d+\.)(\d+)', oldversion)
    newversion = nums.group(1) + str(int(nums.group(2)) +1)
    if (not not_interactive):
        confirm_replace(oldversion, newversion)
    update_changelog(newversion,"patch")
    version_obj.update_version(newversion)
    replace_version(version_obj)

def set_manual_version(version_obj, user_version, not_interactive):
    "Changed the version number to a user supplied value"
    oldversion = version_obj.version
    if not re.match(r'(\d+\.\d+\.\d+(?:[-+~]\w+)*)$', user_version):
        print('Bad version string. Use this style: x.y.z or x.y.z-betax')
        sys.exit(1)
    newversion = user_version
    if (not not_interactive):
        confirm_replace(oldversion, newversion)
    update_changelog(newversion,"manual")
    version_obj.update_version(newversion)
    replace_version(version_obj)

def do_release(version_obj, not_interactive):
    "Strips extra characters from the version leaving x.y.z"
    oldversion = version_obj.version
    newversion = re.sub(r'([\d.]+)-?.*', r'\1', oldversion)
    if (not not_interactive):
        confirm_replace(oldversion, newversion)
    update_changelog(newversion,"release")
    version_obj.update_version(newversion)
    replace_version(version_obj)

def copyright_year():
    "Check and update copyright year as needed"
    fileyear = linecache.getline(VERSION_FILE, 2).strip()
    def year_range(matchobj):
        "Check and add a year range to the copyright"
        if matchobj.group(1) is not None:
            return re.sub(fileyear, CURRENTYEAR, matchobj.group(0))
        return re.sub(fileyear, fileyear+'-'+CURRENTYEAR, matchobj.group(0))

    if int(fileyear) != int(CURRENTYEAR):
        print('Updating copyright year.')
        for myfile in COPYRIGHT_FILES:
            result = []
            with open(myfile, 'r') as infile:
                for line in infile:
                    if re.search(r'[C|c]opyright.*Gregorio Project', line):
                        result.append(re.sub(r'(\d{4}-)?(\d{4})', year_range, line))
                    elif re.search(r'[C|c]opyright.*Elie Roux', line):
                        result.append(re.sub(r'(\d{4}-)?(\d{4})', year_range, line))
                    elif re.search(r'[C|c]opyright.*Richard Chonak', line):
                        result.append(re.sub(r'(\d{4}-)?(\d{4})', year_range, line))
                    elif re.search(r'[C|c]opyright.*Jakub Jelinek', line):
                        result.append(re.sub(r'(\d{4}-)?(\d{4})', year_range, line))
                    else:
                        result.append(line)
            with open(myfile, 'w') as outfile:
                outfile.write(''.join(result))

def main():
    "Main function"
    parser = get_parser()
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(1)
    args = parser.parse_args()
    gregorio_version = Version(VERSION_FILE)
    not_interactive = False
    if args.not_interactive:
        not_interactive = True
    if args.get_current:
        gregorio_version.fetch_version()
    elif args.get_debian_stable:
        gregorio_version.fetch_version_debian_stable()
    elif args.get_debian_git:
        gregorio_version.fetch_version_debian_git()
    copyright_year()
    if args.beta:
        beta(gregorio_version, not_interactive)
    elif args.major:
        bump_major(gregorio_version, not_interactive)
    elif args.minor:
        bump_minor(gregorio_version, not_interactive)
    elif args.patch:
        bump_patch(gregorio_version, not_interactive)
    elif args.release_candidate:
        release_candidate(gregorio_version, not_interactive)
    elif args.release:
        do_release(gregorio_version, not_interactive)
    elif args.manual_version:
        set_manual_version(gregorio_version, args.manual_version, not_interactive)

if __name__ == "__main__":
    main()
