#! /usr/bin/python3

""" Searches through a directory and creates an index page for it
"""

__author__ = "Guido U. Draheim"

import logging
import os.path
import re
import xml.etree.ElementTree as ET

logg = logging.getLogger("dir2index")

def esc(text):
    text = text.replace(".", "\\&.")
    text = text.replace("-", "\\-")
    return text
def unescape(text):
    text = text.replace('&lt;', '<')
    text = text.replace('&gt;', '>')
    text = text.replace('&quot;', '"')
    text = text.replace('&amp;', '&')
    return text
def htm(text):
    text = text.replace('&', '&amp;')
    text = text.replace('<', '&lt;')
    text = text.replace('>', '&gt;')
    text = text.replace('"', '&quot;')
    return text
def splitname(filename):
    base = os.path.basename(filename)
    name, ext = os.path.splitext(base)
    if name.endswith(".3"): name = name[:-2]
    return name

def parse_html(filename):
    tree = ET.parse(filename)
    return tree.getroot()

def zzip_sorted(filenames):
    for name in filenames:
        if "zziplib" in name:
            yield name
    for name in filenames:
        if "zziplib" not in name:
            yield name

def dir2(man, dirs, into):
    text = "<html><body>" + "\n"
    file2name = {}
    file2text = {}
    for dirname in dirs:
        for filename in os.listdir(dirname):
            filepath = os.path.join(dirname, filename)
            file2name[filename] = splitname(filename)
            file2text[filename] = open(filepath).read()
    # find the overview filenames and generate the pages order
    overviews = []
    for filename in file2text:
        if " overview</title>" in file2text[filename]:
            overviews.append(filename)
    logg.warning("overviews = %s", overviews)
    logg.warning("overviews = %s", [file2name[f] for f in overviews])
    file2item = {}
    pages = []
    for overview in zzip_sorted(overviews):
        if overview not in pages:
            pages.append(overview)
        for line in file2text[overview].split("\n"):
            m = re.match('<li><a href="([^"]*)".*</li>', line)
            if m:
                filename = m.group(1)
                if filename not in file2item:
                    file2item[filename] = line
                if filename not in pages:
                    pages.append(filename)
    for filename in sorted(file2name):
        if filename not in pages:
            pages.append(filename)
    text += "<ul>"
    for page in pages:
        if page in file2item:
            text += file2item[page]
        elif page in overviews:
            name = file2name[page]
            logg.warning("page %s = %s", page, name)
            text += '<li><a href="%s"><h4>%s</h4></a></li>' % (page, name)
        else:
            name = file2name[page]
            text += '<li><a href="%s">%s</a></li>' % (page, name)
        text += "\n"
    text += "</ul>"
    text += "</body></html>" + "\n"
    writefile("%s/index.html" % into, text)

def writefile(filename, manpagetext):
    dirname = os.path.dirname(filename)
    if not os.path.isdir(dirname):
        logg.debug("mkdir %s", dirname)
        os.makedirs(dirname)
    with open(filename, "w") as f:
        f.write(manpagetext)
    logg.debug("written %s [%s]", filename, manpagetext.split("\n", 1)[0])

if __name__ == "__main__":
    from optparse import OptionParser
    _o = OptionParser("%prog [options] directories...")
    _o.add_option("-o","--into", metavar="DIR", default=".",
        help="specify base directory for output [%default]")
    _o.add_option("-t","--make", metavar="DIR", default="man",
        help="make 'man'/'html' output pages [%default]")
    _o.add_option("-v","--verbose", action="count", default=0,
        help="increase logging level [%default]")
    opt, args = _o.parse_args()
    logging.basicConfig(level = max(0, logging.WARNING - 10 * opt.verbose))
    # ensure commandline is compatible with "xmlto -o DIR TYPE INPUTFILE"
    make = opt.make
    dir2(make == 'man', args, opt.into)
