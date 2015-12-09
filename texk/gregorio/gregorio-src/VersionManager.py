#! /usr/bin/env python2

"""
    A script that manages the VERSION of gregorio.

    See VersionUpdate.py -h for help

    Copyright (C) 2015 The Gregorio Project (see CONTRIBUTORS.md)

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
from datetime import date

from distutils.util import strtobool

locale.setlocale(locale.LC_TIME, 'C')

os.chdir(sys.path[0])

VERSION_FILE = '.gregorio-version'
GREGORIO_FILES = ["configure.ac",
                  "windows/gregorio-resources.rc",
                  "macosx/Gregorio.pkgproj",
                  "windows/gregorio.iss",
                  "doc/GregorioRef.tex",
                  "tex/gregoriotex.sty",
                  "tex/gregoriotex.lua",
                  "tex/gregoriotex.tex",
                  "tex/gregoriotex-chars.tex",
                  "tex/gregoriotex-signs.tex",
                  "tex/gregoriotex-spaces.tex",
                  "tex/gregoriotex-symbols.tex",
                  "tex/gregoriotex-syllable.tex",
                  "tex/gregoriotex-main.tex",
                  "tex/gregoriosyms.sty",
                  "fonts/squarize.py",
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
        self.version = self.read_version()
        self.filename_version = self.filename_version_from_version(self.version)
        self.short_tag = None
        self.date = None
        self.binary_version = self.binary_version_from_version(self.version)

    def filename_version_from_version(self, version):
        "Return filename-compatible version"
        return version.replace('.', '_')

    def binary_version_from_version(self, version):
        "Return binary version number for Windows FILEVERSION"
        bin = version.replace('.',',')
        if '-' in bin:
            bin = bin.replace('-beta',',1')
            bin = bin.replace('-rc',',2')
        else:
            bin += ',30'
        return bin

    def read_version(self):
        "Return version for instance variable"
        with open(self.versionfile, 'r') as verfile:
            self.grever = verfile.readline()
        return self.grever.strip('\n')

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
            verfile.write(self.version)
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
    with open('CHANGELOG.md', 'r') as infile:
        result = []
        for line in infile:
            if '[Unreleased][unreleased]' in line:
                result.append(line)
                result.append('\n')
                result.append('\n')
                newline = '## [' + newver + '] - ' + today.strftime("%Y-%m-%d") + '\n'
                result.append(newline)
            else:
                result.append(line)
    with open('CHANGELOG.md','w') as outfile:
        outfile.write(''.join(result))
    sys.exit(0)

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
    version_obj.update_version(newversion)
    replace_version(version_obj)

def bump_major(version_obj, not_interactive):
    "Changed the major version number: x.y.z --> x+1.0.0-beta1"
    oldversion = version_obj.version
    nums = re.search(r'(\d+)(\.\d+)(\.\d+)', oldversion)
    newversion = str(int(nums.group(1)) +1) + '.0.0-beta1'
    if (not not_interactive):
        confirm_replace(oldversion, newversion)
    version_obj.update_version(newversion)
    replace_version(version_obj)

def bump_minor(version_obj, not_interactive):
    "Changed the minor version number: x.y.z --> x.y+1.0-beta1"
    oldversion = version_obj.version
    nums = re.search(r'(\d+\.)(\d+)(\.\d+)', oldversion)
    newversion = nums.group(1) + str(int(nums.group(2)) +1) + '.0-beta1'
    if (not not_interactive):
        confirm_replace(oldversion, newversion)
    version_obj.update_version(newversion)
    replace_version(version_obj)

def bump_patch(version_obj, not_interactive):
    "Changed the patch version number: x.y.z --> x.y.z+1"
    oldversion = version_obj.version
    nums = re.search(r'(\d+\.\d+\.)(\d+)', oldversion)
    newversion = nums.group(1) + str(int(nums.group(2)) +1)
    if (not not_interactive):
        confirm_replace(oldversion, newversion)
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
    version_obj.update_version(newversion)
    replace_version(version_obj)

def do_release(version_obj, not_interactive):
    "Strips extra characters from the version leaving x.y.z"
    oldversion = version_obj.version
    newversion = re.sub(r'([\d.]+)-?.*', r'\1', oldversion)
    if (not not_interactive):
        confirm_replace(oldversion, newversion)
    version_obj.update_version(newversion)
    replace_version(version_obj)

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
    elif args.beta:
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
