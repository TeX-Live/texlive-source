#! /usr/bin/python3

""" Converts an xml-file with docbook elements into troff manual pages.
    The conversion uses etree expecting <refentry> elements in the input.
    The output goes to a multiple files in manX/* subdirectories.
"""

__author__ = "Guido U. Draheim"

import logging
import os.path
import re
import collections
import xml.etree.ElementTree as ET

logg = logging.getLogger("dbk2man")

def esc(text):
    text = str(text)
    text = text.replace(".", "\\&.")
    text = text.replace("-", "\\-")
    return text
def unescape(text):
    text = str(text)
    text = text.replace('&lt;', '<')
    text = text.replace('&gt;', '>')
    text = text.replace('&quot;', '"')
    text = text.replace('&amp;', '&')
    return text
def htm(text):
    text = str(text)
    text = text.replace('&', '&amp;')
    text = text.replace('<', '&lt;')
    text = text.replace('>', '&gt;')
    text = text.replace('"', '&quot;')
    return text
def mailhref(text):
    return re.sub("<([^<>]*@[^<>]*)>", 
        lambda x: '&lt;<a href="mailto:%s">%s</a>&gt;' % (x.group(1), x.group(1)), 
        text)

OverviewEntry = collections.namedtuple("OverviewEntry", ["manpage", "manvolnum", "refpurpose"])

def parse_docbook(filename):
    tree = ET.parse(filename)
    return tree.getroot()

def dbk2(man, filenames, subdirectory = "."):
    for filename in filenames:
        root = parse_docbook(filename)
        overview = docbook2(man, root, subdirectory)
        overview2(man, overview, subdirectory, filename)

def docbook2(man, root, subdirectory = "."):
    if root.tag != "reference":
        logg.warning("no <reference> found, not a docbook file?")
        logg.warning("found <%s> instead", root.tag)
    overview = {}
    title = ""
    for refentry in root:
        if refentry.tag == 'title':
            title = refentry.text
            continue
        if refentry.tag != 'refentry':
            logg.warning("no <refentry> list found, not a docbook file?")
            logg.warning("found <%s> instead", refentry.tag)
            continue
        overviewref = refentry2(man, refentry, subdirectory, title)
        for filename, overviewentry in overviewref.items():
            overview[filename] = overviewentry
    return overview

def refentryinfo2(man, refentry, title):
    date, productname, manvolnum, refentrytitle = "", "", "", ""
    section = refentry.find("refentryinfo")
    if section is not None:
        found = section.find("date")
        if found is not None: date = found.text
        found = section.find("productname")
        if found is not None: productname = found.text
    section = refentry.find("refmeta")
    if section is not None:
        found = section.find("refentrytitle")
        if found is not None: refentrytitle = found.text
        found = section.find("manvolnum")
        if found is not None: manvolnum = found.text
    if man:
        header = []
        if refentrytitle:
            header += [ refentrytitle ]
            if manvolnum:
                header += [ manvolnum ]
                if date:
                    header += [ date ]
                    if productname:
                        header += [ productname ]
                    if title:
                        header += [ title ]
        if not header:
            logg.warning("no <refmeta> found")
            return ""
        text = '.TH ' + " ".join([ '"%s"' % esc(part) for part in header])
        return text + "\n"
    else:
        text = "<html><head><title>"
        if productname or title: 
            text += "%s: " % htm(productname or title)
        text += htm(refentrytitle)
        if manvolnum: 
            text += "(%s)" % htm(manvolnum)
        text += "</title>"
        text += "\n" + '<meta name="product" content="%s" />' % htm(productname or title)
        text += "\n" + '<meta name="refentry" content="%s" />' % htm(refentrytitle)
        text += "\n" + '<meta name="manvolnum" content="%s" />' % htm(manvolnum)
        text += "\n" + '<meta name="date" content="%s" />' % htm(date)
        return text + "\n</head><body>\n"

def refentrytitle2(man, refentry, title = ""):
    refentrytitle = ""
    section = refentry.find("refmeta")
    if section is not None:
        found = section.find("refentrytitle")
        if found is not None: refentrytitle = found.text
    refentries = [ refentrytitle ]
    refpurpose = ""
    section = refentry.find("refnamediv")
    if section is not None:
        found = section.find("refpurpose")
        if found is not None: refpurpose = found.text
        for found in section.findall("refname"):
             refname = found.text
             if refname not in refentries:
                 refentries.append(refname)
    if not refentrytitle:
        logg.warning("no <refentrytitle> found")
        return ""
    elif man:
        text = '.SH "NAME"' + "\n"
        text += "" + esc(", ".join(refentries))
        text += " " + esc("-")
        text += " " + esc(refpurpose)
        return text + "\n"
    else:
        text = '<h3>Name</h3>' + "\n"
        text += "<p>" + htm(", ".join(refentries))
        text += " " + htm("-")
        text += " " + htm(refpurpose)
        text += "</p>"
        return text + "\n"

def refsynopsisdiv2(man, refentry, title = ""):
    refsynopsisdiv = refentry.find("refsynopsisdiv")
    if refsynopsisdiv is None:
        logg.warning("no <resynopsisdiv> found")
        return ""
    if man:
        return refsynopsisdiv2man(refsynopsisdiv, title)
    else:
        return refsynopsisdiv2htm(refsynopsisdiv, title)

def refsynopsisdiv2man(refsynopsisdiv, title = ""):
    text = '.SH "SYNOPSIS"' + "\n"
    text += ".sp\n"
    text += ".nf\n"
    for funcsynopsis in refsynopsisdiv.findall("funcsynopsis"):
        funcsynopsisinfo = ""
        found = funcsynopsis.find("funcsynopsisinfo")
        if found is not None: funcsynopsisinfo = found.text
        if funcsynopsisinfo:
            for info in funcsynopsisinfo.split("\n"):
                text += '.B "%s"' % esc(info)
                text += "\n"
            text += ".sp" + "\n"
        else:
            logg.debug("no <funcsynopsisinfo> found")
            logg.debug("\n%s", ET.tostring(funcsynopsis))
        funcs = 0
        for funcprototype in funcsynopsis.findall("funcprototype"):
            item = ET.tostring(funcprototype)
            item = str(item)
            item = item.replace("<funcprototype>","")
            item = item.replace("</funcprototype>","")
            if False:
                item = item.replace("\n", " ")
                item = item.replace("<funcdef>","")
                item = item.replace("</funcdef>","")
                item = item.replace("<paramdef>",'<def>')
                item = item.replace("</paramdef>",'<def>')
                items = item.split("<def>")
                text += '.BI %s' % " ".join(['"%s"' % part for part in items if part])
            else:
                item = item.replace("<funcdef>","")
                item = re.sub(r"([_\w]+)</funcdef>", lambda x: "\\fI%s\\fR" % x.group(1), item)
                item = item.replace("<paramdef>",'')
                item = item.replace("</paramdef>",'')
                text += item 
            text += "\n"
            funcs += 1
        if not funcs:
            logg.warning("no <funcprototype> found")
            logg.warning("\n%s", ET.tostring(funcsynopsis))
        text += ".fi" + "\n"
        text += ".sp" + "\n"
    return text

def refsynopsisdiv2htm(refsynopsisdiv, title = ""):
    text = '<h3>Synopsis</h3>' + "\n"
    text += '<pre>' + "\n"
    for funcsynopsis in refsynopsisdiv.findall("funcsynopsis"):
        funcsynopsisinfo = ""
        found = funcsynopsis.find("funcsynopsisinfo")
        if found is not None: funcsynopsisinfo = found.text
        if funcsynopsisinfo:
            for info in funcsynopsisinfo.split("\n"):
                text += '<b>%s</b>' % htm(info)
                text += "\n"
            text += "\n"
        else:
            logg.debug("no <funcsynopsisinfo> found")
            logg.debug("\n%s", ET.tostring(funcsynopsis))
        funcs = 0
        for funcprototype in funcsynopsis.findall("funcprototype"):
            item = ET.tostring(funcprototype)
            item = str(item)
            item = item.replace("<funcprototype>","")
            item = item.replace("</funcprototype>","")
            item = item.replace("<funcdef>","")
            item = re.sub(r"([_\w]+)</funcdef>", lambda x: "<b>%s</b>" % x.group(1), item)
            item = item.replace("<paramdef>",'')
            item = item.replace("</paramdef>",'')
            text += item 
            text += "\n"
            funcs += 1
        if not funcs:
            logg.warning("no <funcprototype> found")
            logg.warning("\n%s", ET.tostring(funcsynopsis))
        text += "</pre>" + "\n"
    return text

def refsections2(man, refentry, title = ""):
    text = ""
    for refsect in refentry.findall("refsect1"):
        if man:
            text += refsect2man(refsect, title)
            text += ".sp\n"
        else:
            text += refsect2htm(refsect, title)
    return text


def refsect2man(refsect, title = ""):
    text = ""
    head = refsect.find("title")
    if head is not None:
       text += '.SH "%s"' % (esc(head.text.upper()))
       text += "\n"
    for para in list(refsect):
        if para.tag == 'title':
            continue
        if para.tag == 'para':
            text += para2man(para) + "\n"
            text += ".sp\n"
            continue
        if para.tag == 'itemizedlist':
            for item in list(para):
                text += para2man(item) + "\n"
                text += ".sp\n"
            continue
        logg.warning("unknown para <%s>", para.tag)
    return text

def refsect2htm(refsect, title = ""):
    text = ""
    head = refsect.find("title")
    if head is not None:
       text += '<h3>%s</h3>' % htm(head.text)
       text += "\n"
    for para in list(refsect):
        if para.tag == 'title':
            continue
        if para.tag == 'para':
            text += "<p>" + para2htm(para)
            text += "</p>" + "\n"
            continue
        if para.tag == 'itemizedlist':
            text += "<ul>" + "\n"
            for item in list(para):
                text += "<li><p>" + para2htm(item)
                text += "</p></li>" + "\n"
            text += "</ul>" + "\n"
            continue
        logg.warning("unknown para <%s>", para.tag)
    return text

def para2man(para):
   item = unescape(ET.tostring(para))
   item = str(item)
   item = item.replace("\n", " ")
   item = item.replace("  ", " ")
   item = item.replace("  ", " ")
   item = item.replace("  ", " ")
   item = item.replace("  ", " ")
   item = item.replace("<listitem>", "")
   item = item.replace("</listitem>", "")
   item = item.replace("<para>", "")
   item = item.replace("</para>", "")
   item = item.replace("<function>", "\\fI")
   item = item.replace("</function>", "\\fP")
   item = item.replace("<literal>", "\\fI")
   item = item.replace("</literal>", "\\fP")
   return item

def para2htm(para):
   item = unescape(ET.tostring(para))
   item = str(item)
   item = item.replace("\n", " ")
   item = item.replace("  ", " ")
   item = item.replace("  ", " ")
   item = item.replace("  ", " ")
   item = item.replace("  ", " ")
   item = item.replace("<listitem>", "")
   item = item.replace("</listitem>", "")
   item = item.replace("<para>", "")
   item = item.replace("</para>", "")
   item = item.replace("<function>", "<em><code>")
   item = item.replace("</function>", "</code></em>")
   item = item.replace("<literal>", "<code>")
   item = item.replace("</literal>", "</code>")
   item = mailhref(item)
   return item

def styleinfo2(man):
    if man:
        styles = []
        styles += [ ".ie \\n(.g .ds Aq \\(aq" ]
        styles += [ ".el        .ds Aq " ] # http://bugs.debian.org/507673
        styles += [ ".nh" ] # disable hyphenation
        styles += [ ".ad l" ] # align left, no justification
        return "".join([ "%s\n" % part for part in styles ])
    else:
        return ""

def refends2(man):
    if man:
        return ""
    else:
        return "</body></html>" + "\n"

def refentry2(man, refentry, subdirectory = ".", title = ""):
    if refentry.tag != "refentry":
        logg.warning("no <refentry> found, not a docbook file?")
        logg.warning("found <%s> instead", refentry.tag)
    text = ""
    text += refentryinfo2(man, refentry, title)
    text += styleinfo2(man)
    text += refentrytitle2(man, refentry, title)
    text += refsynopsisdiv2(man, refentry)
    text += refsections2(man, refentry)
    text += refends2(man)

    ### write the files
    refentrytitle = ""
    manvolnum = "3"
    section = refentry.find("refmeta")
    if section is not None:
        found = section.find("refentrytitle")
        if found is not None: refentrytitle = found.text
        found = section.find("manvolnum")
        if found is not None: manvolnum = found.text
    #
    refpurpose = ""
    section = refentry.find("refnamediv")
    if section is None:
        logg.warning("no <refnamediv> found in <refentry> for '%s', bad docbook?", refentrytitle)
        if not refentrytitle: raise Exception("not even a refentrytitle")
        manpages = [ refentrytitle ]
    else:
        manpages = [ refname.text for refname in section.findall("refname") ]
        found = section.find("refpurpose")
        if found is not None: refpurpose = found.text
    #
    overview = {}
    if man:
        written = 0
        for manpage in manpages:
            if not refentrytitle:
                refentrytitle = manpage
            filename = "%s/man%s/%s.%s" % (subdirectory, manvolnum, manpage, manvolnum)
            if manpage != refentrytitle:
                manpagetext = ".so %s.%s\n" % (refentrytitle, manvolnum)
                writefile(filename, manpagetext)
            else:
                manpagetext = text
                writefile(filename, manpagetext)
                written += 1
            overview[filename] = OverviewEntry(manpage, manvolnum, refpurpose)
        if not written:
            manpage = refentrytitle
            filename = "%s/man%s/%s.%s" % (subdirectory, manvolnum, manpage, manvolnum)
            writefile(filename, manpagetext)
            overview[filename] = OverviewEntry(manpage, manvolnum, refpurpose)
    else:
        manpage = refentrytitle
        filename = "%s/%s.%s.%s" % (subdirectory, manpage, manvolnum, "html")
        writefile(filename, text)
        overview[filename] = OverviewEntry(manpage, manvolnum, refpurpose)
    #
    return overview

def splitname(filename):
    base = os.path.basename(filename)
    name, ext = os.path.splitext(base)
    return name

def overview2(man, overview, subdirectory, docbook_filename):
    if not man:
        overview2htm(overview, subdirectory, docbook_filename)

def overview2htm(overview, subdirectory, docbook_filename):
    basename = splitname(docbook_filename)
    text = "<html><head><title>%s %s</title>\n" % (htm(basename), htm("overview"))
    text += "</head><body>\n"
    text += "<h3>%s %s</h3>\n" % (htm(basename), htm("overview"))
    text += "<ul>\n"
    for filename in sorted(overview):
        entry = overview[filename]
        subdir_filename = os.path.basename(filename)
        text += '<li><a href="%s">%s</a> - %s</li>' % (subdir_filename, entry.manpage, htm(entry.refpurpose))
        text += "\n"
    text += "</ul>\n"
    text += "</body></html>\n"
    filename = "%s/%s.%s" % (subdirectory, basename, "html")
    writefile(filename, text)

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
    _o = OptionParser("%prog [options] docbookfiles...")
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
    if args and args[0] in ("man", "html"):
       make = args[0]
       args = args[1:]
    dbk2(make == 'man', args, opt.into)
