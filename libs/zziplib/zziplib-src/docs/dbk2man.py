#! /usr/bin/python
""" Converts an xml-file with docbook elements into troff manual pages.
    The conversion uses etree expecting <refentry> elements in the input.
    The output goes to a multiple files in manX/* subdirectories.
"""

__author__ = "Guido U. Draheim"

import logging
import os.path
import re
import xml.etree.ElementTree as ET

logg = logging.getLogger("dbk2man")

def parse_docbook(filename):
    tree = ET.parse(filename)
    return tree.getroot()

def dbk2man(filename, subdirectory = "."):
    root = parse_docbook(filename)
    return docbook2man(root, subdirectory)

def docbook2man(root, subdirectory = "."):
    if root.tag != "reference":
        logg.warning("no <reference> found, not a docbook file?")
        logg.warning("found <%s> instead", root.tag)
    title = ""
    for refentry in root:
        if refentry.tag == 'title':
            title = refentry.text
            continue
        if refentry.tag != 'refentry':
            logg.warning("no <refentry> list found, not a docbook file?")
            logg.warning("found <%s> instead", refentry.tag)
            continue
        refentry2man(refentry, subdirectory, title)

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


def refentryinfo2(refentry, title):
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
    if header:
        text = '.TH ' + " ".join([ '"%s"' % esc(part) for part in header])
        return text + "\n"
    logg.warning("no <refmeta> found")
    return ""

def refentrytitle2(refentry, title = ""):
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
    if refentrytitle:
        text = '.SH "NAME"' + "\n"
        text += "" + esc(", ".join(refentries))
        text += " " + esc("-")
        text += " " + esc(refpurpose)
        return text + "\n"
    logg.warning("no <refentrytitle> found")
    return ""


def refsynopsisdiv2(refentry, title = ""):
    section = refentry.find("refsynopsisdiv")
    if section is not None:
        text = '.SH "SYNOPSIS"' + "\n"
        text += ".sp\n"
        text += ".nf\n"
        for funcsynopsis in section.findall("funcsynopsis"):
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
    logg.warning("no <resynopsidiv> found")
    return ""

def refsections2(refentry, title = ""):
    text = ""
    for refsect in refentry.findall("refsect1"):
        head = refsect.find("title")
        if head is not None:
           text += '.SH "%s"' % (esc(head.text.upper()))
           text += "\n"
        for para in list(refsect):
            if para.tag == 'title':
                continue
            if para.tag == 'para':
                text += cleanpara(para) + "\n"
                text += ".sp\n"
                continue
            if para.tag == 'itemizedlist':
                for item in list(para):
                    text += cleanpara(item) + "\n"
                    text += ".sp\n"
                continue
            logg.warning("unknown para <%s>", para.tag)
        text += ".sp\n"
    return text

def cleanpara(para):
   item = unescape(ET.tostring(para))
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

def styleinfo():
   styles = []
   styles += [ ".ie \\n(.g .ds Aq \\(aq" ]
   styles += [ ".el        .ds Aq " ] # http://bugs.debian.org/507673
   styles += [ ".nh" ] # disable hyphenation
   styles += [ ".ad l" ] # align left, no justification
   return "".join([ "%s\n" % part for part in styles ])

def refentry2man(refentry, subdirectory = ".", title = ""):
    if refentry.tag != "refentry":
        logg.warning("no <refentry> found, not a docbook file?")
        logg.warning("found <%s> instead", refentry.tag)
    text = ""
    text += refentryinfo2(refentry, title)
    text += styleinfo()
    text += refentrytitle2(refentry, title)
    text += refsynopsisdiv2(refentry)
    text += refsections2(refentry)

    ### write the files
    refentrytitle = ""
    manvolnum = "3"
    section = refentry.find("refmeta")
    if section is not None:
        found = section.find("refentrytitle")
        if found is not None: refentrytitle = found.text
        found = section.find("manvolnum")
        if found is not None: manvolnum = found.text
    written = 0
    section = refentry.find("refnamediv")
    for refname in section.findall("refname"):
        if not refentrytitle:
            refentrytitle = refname.text
        manpage = refname.text
        filename = "%s/man%s/%s.%s" % (subdirectory, manvolnum, manpage, manvolnum)
        if manpage != refentrytitle:
            manpagetext = ".so %s.%s\n" % (refentrytitle, manvolnum)
            writefile(filename, manpagetext)
        else:
            manpagetext = text
            writefile(filename, manpagetext)
            written += 1
    if not written:
        manpage = refentrytitle
        filename = "%s/man%s/%s.%s" % (subdirectory, manvolnum, manpage, manvolnum)
        writefile(filename, manpagetext)

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
    _o.add_option("-d","--into", metavar="DIR", default=".",
        help="specify base directory for output [%default]")
    _o.add_option("-v","--verbose", action="count", default=0,
        help="increase logging level [%default]")
    opt, args = _o.parse_args()
    logging.basicConfig(level = max(0, logging.WARNING - 10 * opt.verbose))
    for arg in args:
        dbk2man(arg, opt.into)
