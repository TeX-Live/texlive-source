#!/usr/bin/env python3

# texliveonfly.py (formerly lualatexonfly.py) - "Downloading on the fly"
#     (similar to miktex) for texlive.
#
# Given a .tex file, runs lualatex (by default) repeatedly, using error messages
#     to install missing packages.
#
#
# September 19, 2011 Release
#
# Written on Ubuntu 10.04 with TexLive 2011
# Other systems may have not been tested.
#
# Copyright (C) 2011 Saitulaa Naranong
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/copyleft/gpl.html>.

import re, subprocess, os, time,  optparse, sys

#sets up temp directory and paths
tempDirectory =  os.path.join(os.getenv("HOME"), ".texliveonfly")
lockfilePath = os.path.join(tempDirectory,  "newterminal_lock")

#makes sure the temp directory exists
try:
    os.mkdir(tempDirectory)
except OSError:
    print("Our temp directory " + tempDirectory +  " already exists; good.")

checkedForUpdates = False   #have we checked for updates yet?

#NOTE: double-escaping \\ is neccessary for a slash to appear in the bash command
def spawnInNewTerminal(bashCommand):
    #creates lock file
    lockfile = open(lockfilePath, 'w')
    lockfile.write("texliveonfly currently performing task in separate terminal.")
    lockfile.close()

    #adds line to remove lock at end of command
    bashCommand += '; rm \\"' + lockfilePath + '\\"'

    #runs the bash command in a new terminal
    process = subprocess.Popen (
        ['x-terminal-emulator', '-e',  'sh -c "{0}"'.format(bashCommand) ]
            , stdout=subprocess.PIPE )
    process.wait()

    #doesn't let us proceed until the lock file has been removed by the bash command
    while os.path.exists(lockfilePath):
        time.sleep(0.1)

def updateTLMGR():
    global checkedForUpdates
    if not checkedForUpdates:
        spawnInNewTerminal('''echo \\"Updating tlmgr prior to installing packages\n(this is necessary to avoid complaints from itself).\\n\\" ; sudo tlmgr update --self''')
        checkedForUpdates = True

#strictmatch requires an entire /file match in the search results
def getSearchResults(preamble, term, strictMatch):
    output = subprocess.getoutput("tlmgr search --global --file " + term)
    outList = output.split("\n")

    results = ["latex"]    #latex entry for removal later

    for line in outList:
        line = line.strip()
        if line.startswith(preamble) and (not strictMatch or line.endswith("/" + term)):
            #filters out the package in:
            #   texmf-dist/.../package/file
            #and adds it to packages
            results.append(line.split("/")[-2].strip())
            results.append(line.split("/")[-3].strip()) #occasionally the package is one more slash before

    results = list(set(results))    #removes duplicates
    results.remove("latex")     #removes most common fake result
    return results

def getFilePackage(file):
    return " ".join( getSearchResults("texmf-dist/", file, True) )

def getFontPackage(font):
    font = re.sub(r"\((.*)\)", "", font)    #gets rid of parentheses
    results = getSearchResults("texmf-dist/fonts/", font , False)

    #allow for possibility of lowercase
    if len(results) == 0:
        return "" if font.islower() else getFontPackage(font.lower())
    else:
        return " ".join(results)

#string can contain more than one package
def installPackages(packagesString):
    updateTLMGR()  #avoids complaints about tlmgr not being updated

    #New terminal is required: we're not guaranteed user can input sudo password into editor
    print("Attempting to install LaTex package(s): " + packagesString )
    print("A new terminal will open and you may be prompted for your sudo password.")

    #bash command to download and remove lock
    bashCommand='''echo \\"Attempting to install LaTeX package(s): {0} \\"
echo \\"(Some of them might not be real.)\\n\\"
sudo tlmgr install {0}'''.format(packagesString)

    spawnInNewTerminal(bashCommand)

### MAIN PROGRAM ###
licenseinfo = """texliveonfly.py Copyright (C) 2011 Saitulaa Naranong
This program comes with ABSOLUTELY NO WARRANTY;
See the GNU General Public License v3 for more info."""

defaultArgs = "-synctex=1 -interaction=nonstopmode"

if __name__ == '__main__':
    # Parse command line
    parser = optparse.OptionParser(
        usage="\n\n\t%prog [options] file.tex\n\nUse option --help for more info.\n\n" + licenseinfo ,
        version='2011.20.9',
        conflict_handler='resolve'
    )

    parser.add_option('-h', '--help',
        action='help', help='print this help text and exit')
    parser.add_option('-e', '--engine',
        dest='engine', metavar='ENGINE', help='your LaTeX compiler; defaults to lualatex', default="lualatex")
    parser.add_option('-a', '--arguments',
        dest='arguments', metavar='ARGS', help='arguments to send to engine; default is: "{0}"'.format(defaultArgs) , default=defaultArgs)
    parser.add_option('-f', '--fail_silently', action = "store_true" ,
        dest='fail_silently', help="If tlmgr cannot be found, compile document anyway.", default=False)

    (options, args) = parser.parse_args()

    if len(args) == 0:
        parser.error("You must specify a .tex file to compile.")

    latexDocName = args[0]

    if "not found" in subprocess.getoutput("tlmgr"):
        if options.fail_silently:
            subprocess.getoutput( options.engine + ' ' + options.arguments + ' "' + latexDocName + '"')
            sys.exit(0)
        else:
            parser.error("It appears tlmgr is not installed.  Are you sure you have TeX Live 2010 or later?")

    #loop constraints
    done = False
    previousFile = ""
    previousFontFile = ""
    previousFont =""

    #keeps running until all missing font/file errors are gone, or the same ones persist in all categories
    while not done:
        output = subprocess.getoutput( options.engine + ' ' + options.arguments + ' "' + latexDocName + '"')

        #most reliable: searches for missing file
        filesSearch = re.findall(r"! LaTeX Error: File `([^`']*)' not found" , output) + re.findall(r"! I can't find file `([^`']*)'." , output)
        #next most reliable: infers filename from font error
        fontsFileSearch = [ name + ".tfm" for name in re.findall(r"! Font \\[^=]*=([^\s]*)\s", output) ]
        #brute force search for font name in files
        fontsSearch =  re.findall(r"! Font [^\n]*file\:([^\:\n]*)\:", output) + re.findall(r"! Font \\[^/]*/([^/]*)/", output)

        if len(filesSearch) > 0 and filesSearch[0] != previousFile:
            installPackages(getFilePackage(filesSearch[0]))
            previousFile = filesSearch[0]
        elif len(fontsFileSearch) > 0 and fontsFileSearch[0] != previousFontFile:
            installPackages(getFilePackage(fontsFileSearch[0]))
            previousFontFile = fontsFileSearch[0]
        elif len(fontsSearch) > 0 and fontsSearch[0] != previousFont:
            installPackages(getFontPackage(fontsSearch[0]))
            previousFont = fontsSearch[0]
        else:
            done = True
