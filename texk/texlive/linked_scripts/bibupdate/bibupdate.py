#!/usr/bin/env python3

r"""
===============================================
bibupdate - update the entries of a bibtex file
===============================================

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

See https://github.com/AndrewMathas/BibUpdate for more details
about the bibupdate program.

Andrew Mathas andrew.mathas@gmail.com
Copyright (C) 2012-2026  # @bump@
"""

######################################################
import bdb
import difflib
import itertools
import json
import os
import re
import shutil
import sys
import tempfile
import textwrap
import threading
import time
import unicodedata
import urllib.error
import urllib.parse
import urllib.request
import xml.etree.ElementTree as ET

##########################################################
# Define bibupdate's meta data. This is used for global variables, to generate
# the doc-string and README files and in setup.py.

# bibupdate.ini is the single source of truth for the metadata below (it is what
# the release/bump script edits). The embedded default_metadata is only used as
# a fallback when bibupdate.ini cannot be found, so that bibupdate.py can run
# as a self-contained script.
default_metadata = {
    "author": "Andrew Mathas",
    "author_email": "andrew.mathas@gmail.com",
    "copyright": "2012-2026",  # @bump@
    "description": "Automatically update bibtex files",
    "keywords": "bibtex, mrlookup, MathSciNet, latex",
    "license": "GNU General Public License, Version 3, 29 June 2007",
    "name": "Andrew Mathas",
    "program": "bibupdate",
    "release date": "3 November 2026 CET",  # @bump@
    "repository": "https://github.com/AndrewMathas/BibUpdate",
    "url": "https://github.com/AndrewMathas/BibUpdate",
    "version": "2.1.0",  # @bump@
}


class Settings(dict):
    r"""
    A class for holding bibupdate's metadata and global variables. The metadata
    is read from `ini_file` (bibupdate.ini), falling back to `default_metadata`
    if the file cannot be found. Each metadata item is available both as a
    dictionary entry, `bibup[key]`, and as an attribute, `bibup.key`.
    """

    def __init__(self, ini_file):
        super().__init__()
        metadata = dict(default_metadata)
        try:
            with open(ini_file) as ini:
                for line in ini:
                    if "=" in line:
                        key, val = line.split("=", 1)
                        key = key.strip().lower()
                        if key != "":
                            metadata[key] = val.strip()
        except IOError:
            pass  # fall back to the embedded default_metadata
        for key, val in metadata.items():
            self[key] = val  # available as bibup[key]
            setattr(self, key, val)  # available as bibup.key


# Launchers may be symlinks in a bin directory; metadata belongs to the script.
ini_file_path = lambda f: os.path.join(os.path.dirname(os.path.realpath(__file__)), f)
bibup = Settings(ini_file_path("bibupdate.ini"))
bibup.debugging = False  # debugging off by default


##########################################################
# We trap system errors so that we can exit cleanly when the program is
# killed and so that we can automatically tap into a debugger when debugging.
def CleanExceptHook(type, value, tb):
    r"""
    Exit cleanly when program is killed, or jump into pdb if debugging
    """
    if type == KeyboardInterrupt:
        if "options" in globals():
            bib_error("program killed. Exiting...")
        else:  # interrupted before the command line was processed
            sys.stderr.write("%s error: program killed. Exiting...\n" % bibup.program)
            sys.exit(2)
    elif (
        not bibup.debugging
        or hasattr(sys, "ps1")
        or not sys.stdin.isatty()
        or not sys.stdout.isatty()
        or not sys.stderr.isatty()
        or issubclass(type, bdb.BdbQuit)
        or issubclass(type, SyntaxError)
    ):
        sys.__excepthook__(type, value, tb)
    else:
        import pdb
        import traceback

        # we are NOT in interactive mode, print the exception...
        traceback.print_exception(type, value, tb)
        print()
        # ...then start the debugger in post-mortem mode.
        pdb.pm()


# ...and a hook to grab the exception
sys.excepthook = CleanExceptHook

bibup.bibupdate_version = (
    "%(prog)s, version {0.version}: {0.description}\n{0.license}".format(bibup)
)

##########################################################
# Everything that bibupdate uses is in the python (3) standard library.
import argparse
from collections import OrderedDict, deque
from concurrent.futures import ThreadPoolExecutor

# We use fuzzy matching to decide whether two titles are "the same". This is
# done using difflib.SequenceMatcher.ratio(), which returns a number between 0 and 1
# measuring how similar the two strings are. We treat titles that are more than
# 90% similar as a match.
good_match_ratio = 0.9
DEFAULT_IGNORED_FIELDS = ("coden", "mrreviewer")


def good_match(one, two):
    r"""
    Return True or False depending on whether or not the lower cased strings
    `one` and `two` are a good (fuzzy) match for each other.
    """
    return (
        difflib.SequenceMatcher(None, one.lower(), two.lower()).ratio()
        > good_match_ratio
    )


lookup_log = threading.local()


def display_text(value):
    """Remove capitalization braces and normalize spacing for terminal output."""
    value = value.replace("{", "").replace("}", "").replace("~", " ")
    return " ".join(value.split())


def wrap_report(text, indent=""):
    """Keep report lines within 78 columns, with indented continuation lines."""
    return textwrap.fill(
        text,
        width=78,
        initial_indent=indent,
        subsequent_indent="  ",
        break_on_hyphens=False,
    )


def bib_print(*args):
    r"""
    Default printing mechanism. Defaults to sys.stdout but can be overridden
    by the command line options.
    """
    messages = getattr(lookup_log, "messages", None)
    if messages is not None:
        messages.extend(args)
    else:
        for a in args:
            # Color at the final write, after worker messages are replayed,
            # so redirected output and log files stay free of escape codes.
            if options.log.isatty():
                if a.startswith("! Didn't find "):
                    a = "\033[31m" + a + "\033[0m"
                elif a.startswith(
                    ("! Duplicate ", "! Possible duplicate ", "! Ambiguous citation ")
                ):
                    a = "\033[35m" + a + "\033[0m"
            options.log.write(a + "\n")


def bib_error(*args):
    r"""
    Print errors to sys.stderr and also to the log file if it is not sys.stdout.
    Then exit with an error code of 2.
    """
    if options.log != sys.stdout:
        bib_print("%s error:" % options.prog)
        for a in args:
            bib_print(a)

    sys.stderr.write("%s error: " % options.prog)
    for a in args:
        sys.stderr.write(a + "\n")
    sys.exit(2)


##########################################################
# The AMS provides its "free tools" (mrlookup, mref and lookup by MR number) as
# JSON web APIs served from mathscinet.ams.org. These endpoints sit behind
# Cloudflare, which returns a 403 to the default Python-urllib User-Agent, so we
# present our own User-Agent string when calling them.
ams_api = "https://mathscinet.ams.org/mathscinet/api"
user_agent = "bibupdate (https://github.com/AndrewMathas/BibUpdate)"
url_timeout = 30  # seconds to wait for a response before giving up on a request


def get_url(url):
    r"""
    Fetch `url`, sending a User-Agent (the default Python-urllib one is blocked
    by the AMS), and return the decoded response body as a string. Returns None,
    after printing a warning, if the request fails or times out. A single failed
    lookup should not abort a run over a whole bibliography.
    """
    try:
        bibup.debug("S %s" % url)
        request = urllib.request.Request(url, headers={"User-Agent": user_agent})
        lookup = urllib.request.urlopen(request, timeout=url_timeout)
        page = lookup.read().decode("utf-8", "replace")
        lookup.close()  # close the url feed
        return page
    except OSError as err:
        bibup.warning("! unable to connect to %s (%s)" % (url, err))
        return None


def get_json(url):
    r"""
    Fetch `url` and return the response parsed as JSON (a dict). Returns an empty
    dict if the request fails or the response is not valid JSON.
    """
    page = get_url(url)
    if page is None:
        return {}
    try:
        return json.loads(page)
    except ValueError:
        bibup.debug("MR non-JSON response")
        return {}


def article_bibtex(paper_id):
    r"""
    Return the bibtex string for the AMS article with the given `paper_id` (its
    MR number), or None if it cannot be retrieved.
    """
    data = get_json("%s/public/articles-with-formats/%s" % (ams_api, paper_id))
    return data.get("bibTexFormat")


def mrlookup_bibtex(data):
    r"""
    Extract the list of bibtex strings from an mrlookup JSON response. The
    response has the form {"all": {"results": [...]}, "dml": {"results": [...]}}
    where each result has a `bibTexFormat` field.
    """
    bibtex = []
    for section in ("all", "dml"):
        for result in data.get(section, {}).get("results", []):
            if result.get("bibTexFormat"):
                bibtex.append(result["bibTexFormat"])
    return bibtex


##########################################################
# The arXiv provides a public API that returns an Atom (XML) feed. We search on
# the significant words in the title and then fuzzily match the titles of the
# papers that are returned. The arXiv asks that successive API calls be spaced
# out by at least 3 seconds, so we throttle our requests accordingly.
arxiv_api = "https://export.arxiv.org/api/query"
arxiv_atom = {"atom": "http://www.w3.org/2005/Atom"}
arxiv_stopwords = set(
    "a an and are as at by for from in is of on or the to via with".split()
)
arxiv_delay = 3  # seconds requested by the arXiv between API calls
arxiv_last_call = [
    0.0
]  # time of the last arXiv API call (in a list so we can mutate it)


def arxiv_search(title):
    r"""
    Search the arXiv for `title` and return a list of `(arxiv_id, title)` pairs
    for the papers that are returned. The arxiv_id has any version suffix (such
    as v2) removed, so that it has the form 2211.07069 or math/0201001.
    """
    # search on the "significant" words in the title
    words = [
        w
        for w in re.findall(r"[A-Za-z]{3,}", clean_title(title))
        if w.lower() not in arxiv_stopwords
    ]
    if words == []:
        return []
    query = " AND ".join("ti:%s" % w for w in words)
    url = "%s?%s" % (
        arxiv_api,
        urllib.parse.urlencode({"search_query": query, "max_results": 10}),
    )

    # throttle: the arXiv asks for a delay between successive API calls
    wait = arxiv_delay - (time.time() - arxiv_last_call[0])
    if wait > 0:
        time.sleep(wait)
    page = get_url(url)
    arxiv_last_call[0] = time.time()
    if page is None:
        return []  # the request failed; get_url has already warned

    try:
        feed = ET.fromstring(page)
    except ET.ParseError:
        bibup.debug("arXiv: invalid XML response")
        return []

    results = []
    for entry in feed.findall("atom:entry", arxiv_atom):
        aid = entry.findtext("atom:id", "", arxiv_atom).strip()
        aid = re.sub(r"^https?://arxiv\.org/abs/", "", aid)  # strip the url prefix
        aid = re.sub(r"v\d+$", "", aid)  # strip the version suffix
        results.append((aid, entry.findtext("atom:title", "", arxiv_atom)))
    return results


##########################################################

# define a regular expression for extracting papers from BibTeX file
bibtex_entry = re.compile(r"(@\s*[A-Za-z]*\s*{[^@]*})\s*", re.DOTALL)

# regular expression for cleaning TeX from title etc
remove_tex = re.compile(r'[{}\'"_$]+')
remove_mathematics = re.compile(r"\$[^\$]+\$")  # assume no nesting
# savagely remove all maths from title
clean_title = lambda title: remove_tex.sub("", remove_mathematics.sub("", title))

# to help in checking syntax define recognised/valid types of entries in a bibtex database
bibtex_pub_types = [
    "article",
    "book",
    "booklet",
    "conference",
    "inbook",
    "incollection",
    "inproceedings",
    "manual",
    "mastersthesis",
    "misc",
    "phdthesis",
    "proceedings",
    "techreport",
    "unpublished",
]

# need to massage some of the font specifications returned by mrlookup to "standard" latex fonts.
fonts_to_replace = {"Bbb": "mathbb", "scr": "mathcal", "germ": "mathfrak"}
# a regular expression to replace expressions like \scr C and \scr{ Cat} in one hit
font_replacer = re.compile(
    r"\\(%s)\s*(?:(\w)|\{([\s\w]*)\})"
    % "|".join("%s" % f for f in fonts_to_replace.keys())
)


def replace_fonts(string):
    r"""
    Return a new version of `string` with various fonts commands replaced with
    their more standard LaTeX variants.

    Currently::

        - \Bbb X  and \Bbb {X*}  -->  \mathbb{X*}
        - \scr X  and \scr {X*}  -->  \mathcal{X*}
        - \germ X and \germ{X*}  -->  \mathfrak{X*}
    """
    return font_replacer.sub(
        lambda match: (
            r"\%s{%s}"
            % (fonts_to_replace[match.group(1)], match.group(2) or match.group(3))
        ),
        string,
    )


# overkill for "type checking" of the wrap length command line option
class NonnegativeIntegers(list):
    r"""
    A class that gives an easy test for non-negative integers::

        >>> 1 in NonnegativeIntegers()
        True
        >>> -1 in NonnegativeIntegers()
        False
    """

    def __str__(self):
        return "non-negative integers"

    def __contains__(self, x):
        r"""
        By implementing containment we can test if `x` is in `NonnegativeIntegers()`.
        """
        return isinstance(x, int) and x >= 0


class Bibtex(OrderedDict):
    r"""
    The bibtex class holds all of the data for a bibtex entry for a manuscript.
    It is called with a string <bib_string> that contains a bibtex entry and it
    returns a dictionary with whistles for the bibtex entry together with some
    methods for updating and printing the entry. As the bibtex file is a flat
    text file, to extract the data from it we use a collection of regular
    expressions which pull out the data key by key.

    The class contains mrlookup() and mathscinet() methods that attempt to
    update the bibtex entry by searching on the corresponding AMS databases for
    the paper. If successful this overwrites all of the existing bibtex fields,
    except for the cite key. Any entries not in MathSciNet are retained.

    Most of the hard work is done by a cunning use of regular expressions to
    extract the data from the bibtex file and from mrlookup and mathscinet.
    """

    # A regular expression to extract a bibtex entry from a string. We assume that
    # the pub_type is a word in [A-Za-z]* and allow the citation key and the
    # contents of the bibtex entry to be fairly arbitrary and of the form: {cite_key, *}.
    parse_bibtex_entry = re.compile(
        r"@(?P<pub_type>[A-Za-z]*)\s*\{\s*(?P<cite_key>\S*)\s*,\s*?(?P<keys_and_vals>.*\})[,\s]*\}",
        re.MULTILINE | re.DOTALL,
    )

    # To extract the keys and values we need to remove all spaces around equals
    # signs because otherwise we are unable to cope with = inside a bibtex field.
    keys_and_vals = re.compile(r"\s*=\s*")

    # A regular expression to extract pairs of keys and values from a bibtex
    # string. The syntax here is very lax: we assume that bibtex fields do not
    # contain the string '={'.  From the AMS the format of a bibtex entry is
    # much more rigid but we cannot assume that an arbitrary bibtex file will
    # respect the AMS conventions.  There is a small complication in that we
    # allow the value of each field to either be enclosed in braces or to be a
    # single word.
    bibtex_keys = re.compile(
        r"([A-Za-z]+)=(?:\{((?:[^=]|=(?!\{))*)\}|(\w+)),?\s*$", re.MULTILINE | re.DOTALL
    )

    # A regular expression for extracting page numbers: <first page>-*<last page>
    # or simply <page>.
    page_nums = re.compile("(?P<apage>[0-9]*)-+(?P<zpage>[0-9]*)")

    def __init__(self, bib_string):
        """
        Given a string <bib_string> that contains a bibtex entry return the corresponding Bibtex class.
        """
        super(Bibtex, self).__init__()  # initialise as an OrderedDict
        self.updated = False
        self.checked_fields = {}
        entry = self.parse_bibtex_entry.search(bib_string)
        if entry is None:
            self.cite_key = None
            self.bib_string = bib_string
        else:
            self.pub_type = entry.group("pub_type").strip().lower()
            self.cite_key = entry.group("cite_key").strip()
            keys_and_vals = self.keys_and_vals.sub(
                "=", entry.group("keys_and_vals")
            )  # remove spaces around =
            for key, val, word in self.bibtex_keys.findall(keys_and_vals):
                if val == "":
                    val = word  # val matches {value} whereas word matches word
                else:
                    val = " ".join(val.split())  # remove any internal space from val
                lkey = key.lower()  # keys always in lower case
                if lkey == "title":
                    self[lkey] = bibup.fix_fonts(
                        val
                    )  # only fix fonts in the title, others assumed OK
                else:
                    self[lkey] = val

            # guess whether this entry corresponds to a preprint
            self.is_preprint = (
                "pages" in self
                and self["pages"].lower() in ["preprint", "to appear", "in preparation"]
            ) or not ("pages" in self and "journal" in self)

    def __str__(self):
        r"""
        Return a string for printing the bibtex entry.
        """
        if hasattr(self, "pub_type"):
            return "@%s{%s,\n  %s\n}" % (
                self.pub_type.upper(),
                self.cite_key,
                ",\n  ".join(
                    "%s = {%s}" % (key, bibup.wrapped(self[key]))
                    for key in self.keys()
                    if key not in options.ignored_fields
                ),
            )
        else:
            return self.bib_string

    def __getitem__(self, key, default=""):
        """
        We override `__getitem__` so that `self[key]` returns `default` if `key`
        does not exist.
        """
        try:
            return OrderedDict.__getitem__(self, key)
        except KeyError:
            return default

    def has_valid_pub_type(self):
        r"""
        Return True if the entry has a valid pub_type, as determined by
        bibtex_pub_types, and False otherwise.
        """
        return hasattr(self, "pub_type") and (self.pub_type in bibtex_pub_types)

    def bibliography(self):
        """Return the key and a reference wrapped into an indented 78-column block."""

        def field(name):
            return self.checked_fields.get(name, self[name])

        authors = []
        for author in re.split(r"\s+and\s+", field("author")):
            parts = author.split(",")
            if len(parts) == 2:
                author = "%s %s" % (parts[1].strip(), parts[0].strip())
            if author.strip():
                authors.append(display_text(author))
        if len(authors) > 1:
            author_text = ", ".join(authors[:-1]) + " and " + authors[-1]
        else:
            author_text = "".join(authors)

        publication = display_text(field("journal") or field("booktitle"))
        if field("volume"):
            publication += " " + display_text(field("volume"))
        if field("number"):
            publication += "(%s)" % display_text(field("number"))
        if field("year"):
            publication += " (%s)" % display_text(field("year"))
        if field("pages"):
            publication += ", " + display_text(field("pages")).replace("--", "–")
        if field("publisher"):
            publication += (", " if publication.strip() else "") + display_text(
                field("publisher")
            )

        parts = [author_text, display_text(field("title")), publication.strip(" ,")]
        if field("eprint"):
            parts.append("%s:%s" % (field("archiveprefix") or "arXiv", field("eprint")))
        reference = " ".join(part.rstrip(".") + "." for part in parts if part)
        return (
            wrap_report("[%s]" % self.cite_key)
            + "\n"
            + wrap_report(reference, indent="  ")
        )

    def report_update(self):
        """Report once, after both AMS and optional arXiv updates have finished."""
        if options.check:
            if self.checked_fields:
                bib_print(self.bibliography())
            return
        if self.updated:
            if options.quieter == 2:
                bibup.verbose(self.bibliography())
            else:
                bibup.warning(
                    wrap_report(
                        "Updated %s: %s" % (self.cite_key, display_text(self["title"]))
                    )
                )

    def update_entry(self, bibtex_matches):
        """
        Given `bibtex_matches`, a list of candidate bibtex entries (as strings)
        returned by one of the AMS lookup tools, update `self` if there is a
        unique good match. If there is a good match then we overwrite all of our
        fields with those from the match (and keep any other fields).
        """
        # attempt to match self with the bibtex entries returned by the AMS
        matches = [Bibtex(b) for b in bibtex_matches if b is not None]
        matches = [
            mr for mr in matches if mr.cite_key is not None and mr.has_valid_pub_type()
        ]
        bibup.debug("MR number of matches=%d" % len(matches))
        clean_ti = clean_title(self["title"])
        bibup.debug(
            "MR ti=%s.%s"
            % (
                clean_ti,
                "".join("\nMR -->%s." % clean_title(mr["title"]) for mr in matches),
            )
        )
        if clean_ti != "":
            matches = [
                mr for mr in matches if good_match(clean_ti, clean_title(mr["title"]))
            ]
        bibup.debug("MR number of clean matches=%d" % len(matches))

        if len(matches) == 1:
            match = matches[0]
            differences = [
                key
                for key in match
                if key not in options.ignored_fields and self[key] != match[key]
            ]
            if differences:
                if options.check:
                    self.checked_fields.update((key, match[key]) for key in differences)
                else:
                    for key in differences:
                        self[key] = match[key]
                    self.updated = True
            else:
                bibup.debug("No change")

        else:  # no good match, or more than one good match
            bibup.warning(
                wrap_report(
                    "! Didn't find %s: %s"
                    % (self.cite_key, display_text(self["title"]) or "???")
                )
            )

    def mrlookup(self):
        """
        Use mrlookup to search for a more up-to-date version of this entry. To
        search with mrlookup we look for papers published by the first author in
        the given year with the right page numbers. Most of the work here is in
        finding the correct search parameters.
        """
        # only check mrlookup for books, articles, ... for which we don't already have an mrnumber field
        if not options.all and (
            "mrnumber" in self
            or self.pub_type not in ["book", "article", "inproceedings", "incollection"]
        ):
            return

        bibup.debug("=" * 30)
        # the mrlookup free tool accepts author, title, journal, firstPage,
        # lastPage and year fields
        search = {
            "author": "",
            "title": "",
            "journal": "",
            "firstPage": "",
            "lastPage": "",
            "year": "",
        }

        if "pages" in self and not self.is_preprint:
            pages = self.page_nums.search(self["pages"])
            if not pages is None:
                search["firstPage"] = pages.group("apage")  # first page
                search["lastPage"] = pages.group("zpage")  # last page
            elif self["pages"].isdigit():
                search["firstPage"] = self["pages"]  # first page

        # the year is reliable only if we also have page numbers
        if "year" in self and (self.pub_type == "book" or search["firstPage"] != ""):
            search["year"] = self["year"]

        # mrlookup's author field searches for one author. Joining surnames
        # with "and" gives no results for multi-author papers. Search using
        # only the first surname; update_entry still requires a unique title
        # match before accepting any result.
        if self["author"].strip():
            first_author = re.split(r"\s+and\s+|&", self["author"], maxsplit=1)[0]
            first_author = first_author.replace("~", " ").strip()
            if "," in first_author:
                search["author"] = first_author.split(",", 1)[0].strip()
            elif first_author:
                search["author"] = first_author.rsplit(None, 1)[-1]

        if (
            "title" in self
            and len(self["title"]) > 0
            and (search["firstPage"] == "" or self.is_preprint)
        ):
            search["title"] = clean_title(self["title"])

        url = "%s/freetools/mrlookup?%s" % (ams_api, urllib.parse.urlencode(search))
        self.update_entry(mrlookup_bibtex(get_json(url)))

    def mathscinet(self):
        """
        Use the paper's MR number to check/update the entry, if it exists.
        """
        if options.all or "mrnumber" in self:
            mrnumber = re.search(r"\d+", self["mrnumber"])
            if mrnumber is None:
                return  # nothing to search on
            self.update_entry([article_bibtex(mrnumber.group())])

    def mref(self):
        """
        Use MRef to check/update the entry.
        MRef takes a free-form reference so we give it the whole entry to play with.
        """
        # only check for books or articles for which we don't already have an mrnumber field
        if not options.all and (
            "mrnumber" in self
            or self.pub_type not in ["book", "article", "inproceedings", "incollection"]
        ):
            return
        ref = " ".join("%s" % self[key] for key in self.keys())
        url = "%s/freetools/mref?%s" % (ams_api, urllib.parse.urlencode({"ref": ref}))
        paper_id = get_json(url).get("paperId")
        self.update_entry([article_bibtex(paper_id)] if paper_id is not None else [])

    def arxiv(self):
        r"""
        Search the arXiv for this entry and, if there is a unique good (fuzzy)
        title match, add the `eprint` and `archiveprefix` fields. Unlike the AMS
        lookups, this only ever adds these two fields; it does not touch any of
        the other fields in the entry.
        """
        # we need a title to search on and, unless -a is given, we do not bother
        # looking up entries that already have an eprint field
        if self["title"] == "" or (not options.all and "eprint" in self):
            return

        clean_ti = clean_title(self["title"])
        if clean_ti == "":
            return
        matches = [
            aid
            for (aid, ti) in arxiv_search(self["title"])
            if good_match(clean_ti, clean_title(ti))
        ]
        bibup.debug("arXiv number of matches=%d" % len(matches))

        if len(matches) == 1:
            eprint = matches[0]
            # only report/update if something actually changes
            if self["eprint"] != eprint or self["archiveprefix"].lower() != "arxiv":
                if options.check:
                    self.checked_fields.update(eprint=eprint, archiveprefix="arXiv")
                else:
                    self["eprint"] = eprint
                    self["archiveprefix"] = "arXiv"
                    self.updated = True
        elif not self.is_preprint:
            bibup.debug("arXiv: no unique match for %s" % self.cite_key)


def parse_entries(papers):
    """Parse entries and retain their one-based source lines for diagnostics."""
    line, previous = 1, 0
    for match in bibtex_entry.finditer(papers):
        line += papers.count("\n", previous, match.start())
        previous = match.start()
        entry = Bibtex(match.group())
        entry.source_line = line
        yield entry


def normalized_text(value):
    """Normalize case, capitalization braces, spacing and punctuation."""
    value = unicodedata.normalize("NFKC", display_text(value)).casefold()
    return " ".join(re.findall(r"\w+", value))


def paper_signatures(entry):
    """Yield identifier matches and a conservative title/author fallback."""
    doi = urllib.parse.unquote(display_text(entry["doi"])).casefold()
    doi = re.sub(r"^(?:https?://(?:dx\.)?doi\.org/|doi:\s*)", "", doi)
    if re.fullmatch(r"10\.\d{4,9}/\S+", doi):
        yield "DOI", doi

    mrnumber = re.match(
        r"(?:mr)?(\d+)(?:\s|$)", display_text(entry["mrnumber"]).casefold()
    )
    if mrnumber and int(mrnumber.group(1)) > 0:
        yield "MR number", str(int(mrnumber.group(1)))

    if entry["archiveprefix"].casefold() in ("", "arxiv"):
        eprint = display_text(entry["eprint"]).casefold()
        eprint = re.sub(r"^https?://(?:www\.)?arxiv\.org/(?:abs|pdf)/", "", eprint)
        eprint = re.sub(r"(?:v\d+)?(?:\.pdf)?$", "", eprint)
        if re.fullmatch(r"\d{4}\.\d{4,5}|[a-z-]+(?:\.[a-z]{2})?/\d{7}", eprint):
            yield "arXiv ID", eprint

    # Compare all surnames and first initials, accepting both BibTeX name
    # orders and full given names versus initials. Never match by title alone.
    authors = []
    for author in re.split(r"\s+and\s+", entry["author"] or entry["editor"]):
        if not author.strip():
            continue
        if "," in author:
            surname, given = author.split(",", 1)
        else:
            names = author.rsplit(None, 1)
            given, surname = names if len(names) == 2 else ("", names[0])
        surname = normalized_text(surname)
        if surname:
            authors.append((surname, normalized_text(given)[:1]))
    title = normalized_text(entry["title"])
    if title and authors:
        yield "title and authors", (title, tuple(sorted(authors)))


def duplicate_groups(entries):
    """Return repeated-key groups and connected paper groups with match reasons."""
    entries = [
        entry
        for entry in entries
        if entry.cite_key is not None
        and entry.pub_type not in ("comment", "string", "preamble")
    ]
    keys, seen = {}, {}
    parents = list(range(len(entries)))
    reasons = [set() for entry in entries]

    def root(index):
        while parents[index] != index:
            parents[index] = parents[parents[index]]
            index = parents[index]
        return index

    for index, entry in enumerate(entries):
        keys.setdefault(entry.cite_key, []).append(entry)
        for signature in paper_signatures(entry):
            if signature in seen:
                first, second = root(seen[signature]), root(index)
                if first != second:
                    parents[second] = first
                    reasons[first].update(reasons[second])
                reasons[first].add(signature[0])
            else:
                seen[signature] = index

    groups = {}
    for index, entry in enumerate(entries):
        groups.setdefault(root(index), []).append(entry)
    return (
        {key: group for key, group in keys.items() if len(group) > 1},
        [(group, reasons[index]) for index, group in groups.items() if len(group) > 1],
    )


def duplicate_warnings(entries, keys=True, papers=True):
    """Describe selected duplicate groups without changing entries."""
    key_groups, paper_groups = duplicate_groups(entries)
    if keys:
        for key, group in key_groups.items():
            yield wrap_report(
                "! Duplicate citation key '%s': lines %s."
                % (key, ", ".join(str(e.source_line) for e in group))
            )
    if not papers:
        return
    for group, reasons in paper_groups:
        identifiers = reasons - {"title and authors"}
        if identifiers:
            label = "Duplicate paper (shared %s)" % ", ".join(sorted(identifiers))
        else:
            label = "Possible duplicate paper (same title and authors)"
        locations = ", ".join(
            "'%s' (line %d)" % (entry.cite_key, entry.source_line) for entry in group
        )
        yield wrap_report("! %s: %s." % (label, locations))


def source_boundaries(
    source, start, closing, quotes=True, key=False, content_ends=None, comments=True
):
    """Yield top-level commas and the closing delimiter, respecting nesting."""
    depth, quoted, escaped = 0, False, False
    index = content_end = start
    while index < len(source):
        char = source[index]
        if escaped:
            escaped = False
        elif char == "\\":
            escaped = True
        elif char == "%" and comments and depth == 0 and not quoted:
            newline = source.find("\n", index)
            index = len(source) if newline < 0 else newline
            continue
        elif char == closing and depth == 0 and not quoted:
            if content_ends is not None:
                content_ends.append(content_end)
            yield index
            return
        elif char == '"' and depth == 0 and quotes and not key:
            quoted = not quoted
        elif char == "{":
            depth += 1
        elif char == "}" and depth:
            depth -= 1
        elif char == "," and depth == 0 and not quoted:
            if content_ends is not None:
                content_ends.append(content_end)
            yield index
            key = False
        if not char.isspace():
            content_end = index + 1
        index += 1
    raise ValueError(
        "unclosed BibTeX entry or field near line %d"
        % (source.count("\n", 0, start) + 1)
    )


def source_entries(source):
    """Read literal duplicate metadata and source spans without rewriting fields.

    This small scanner also preserves quoted values, macros, comments, nesting
    and @ signs that the update-mode regular expressions cannot round-trip.
    Macro expressions are preserved but not expanded for duplicate matching.
    """
    header = re.compile(r"%[^\n]*|@\s*([A-Za-z]+)\s*([({])")
    position, line, previous = 0, 1, 0
    while True:
        match = header.search(source, position)
        if match is None:
            return
        position = match.end()
        if match.group(1) is None:  # a comment outside an entry
            continue
        kind = match.group(1).lower()
        content_ends = []
        boundaries = list(
            source_boundaries(
                source,
                position,
                "}" if match.group(2) == "{" else ")",
                quotes=kind != "comment",
                key=kind not in ("comment", "string", "preamble"),
                content_ends=content_ends,
            )
        )
        end = boundaries[-1]
        position = end + 1
        if kind in ("comment", "string", "preamble"):
            continue
        key_end = boundaries[0]
        key = source[match.end() : key_end].strip()
        if not key or re.search(r"[\s,{}()]", key):
            raise ValueError(
                "invalid citation key near line %d"
                % (source.count("\n", 0, match.start()) + 1)
            )
        entry = Bibtex("")
        entry.pub_type, entry.cite_key = kind, key
        entry.source_start, entry.source_end = match.start(), end + 1
        line += source.count("\n", previous, match.start())
        previous, entry.source_line = match.start(), line
        entry.key_start = (
            match.end()
            + len(source[match.end() : key_end])
            - len(source[match.end() : key_end].lstrip())
        )
        entry.key_end = entry.key_start + len(key)
        entry.field_spans, entry.field_sources = {}, {}
        entry.append_at = content_ends[-1]
        start = key_end + 1
        for boundary, value_end in zip(boundaries[1:], content_ends[1:]):
            field = re.fullmatch(
                r"(?:\s|%[^\n]*(?:\n|$))*([A-Za-z][\w-]*)\s*=\s*(.*?)\s*",
                source[start:value_end],
                re.DOTALL,
            )
            if field:
                name, value = field.group(1).lower(), field.group(2)
                entry.field_sources[name] = source[start + field.start(1) : value_end]
                offset = start + field.start(2)
                literal = None
                if value.startswith("{"):
                    if (
                        list(
                            source_boundaries(
                                value, 1, "}", quotes=False, comments=False
                            )
                        )[-1]
                        == len(value) - 1
                    ):
                        literal, offset = value[1:-1], offset + 1
                elif re.fullmatch(r'"(?:\\.|[^"\\])*"', value, re.DOTALL):
                    literal, offset = value[1:-1], offset + 1
                elif re.fullmatch(r"\d+", value):
                    literal = value
                if literal is not None:
                    entry[name] = literal
                    entry.field_spans[name] = (offset, offset + len(literal))
            start = boundary + 1
        yield entry


def author_key(entry):
    """Make a portable key from surnames (or editors) and an available year."""
    surnames = []
    for author in re.split(r"\s+and\s+", entry["author"] or entry["editor"]):
        words = author.split()
        name = (
            author.split(",", 1)[0] if "," in author else (words[-1] if words else "")
        )
        name = re.sub(r"\\(?:[A-Za-z]+|[^A-Za-z])\s*", "", name)
        name = unicodedata.normalize("NFKD", display_text(name))
        name = "".join(char for char in name if not unicodedata.combining(char))
        words = re.findall(r"[A-Za-z0-9]+", name)
        surnames.append("".join(word[:1].upper() + word[1:] for word in words))
    year = re.search(r"\d{4}", entry["year"])
    return ("".join(surnames) or "Anonymous") + (year.group() if year else "")


def entry_location(entry):
    """Describe an entry's original location, including its file when merging."""
    location = "line %d" % entry.source_line
    if hasattr(entry, "source_file"):
        location = "'%s', %s" % (entry.source_file, location)
    return location


def merge_sources(filenames, first_file):
    """Validate each input separately and combine source spans in file order."""
    source, entries = "", []
    for index, filename in enumerate(filenames):
        try:
            stream = first_file if index == 0 else open(filename, "r", newline="")
            with stream:
                if index == 0 and hasattr(stream, "reconfigure"):
                    stream.reconfigure(newline="")
                contents = stream.read()
            following = list(source_entries(contents))
        except (OSError, ValueError) as error:
            raise ValueError("%s: %s" % (filename, error)) from error
        if source and not source.endswith("\n"):
            source += "\n"
        offset = len(source)
        for entry in following:
            entry.source_file = filename
            for name in (
                "source_start",
                "source_end",
                "key_start",
                "key_end",
                "append_at",
            ):
                setattr(entry, name, getattr(entry, name) + offset)
            entry.field_spans = {
                name: (start + offset, end + offset)
                for name, (start, end) in entry.field_spans.items()
            }
        entries.extend(following)
        source += contents
    return source, entries


def validate_merge_output(filenames, output):
    """Prevent the output's backup from overwriting an input bibliography."""
    inputs = {os.path.realpath(filename) for filename in filenames}

    def is_input(path):
        return os.path.realpath(path) in inputs or (
            os.path.exists(path)
            and any(os.path.samefile(path, filename) for filename in filenames)
        )

    if is_input(output + ".bak"):
        raise ValueError(
            "output backup overlaps an input bibliography: %s.bak" % output
        )
    if is_input(output) and not options.overwrite and options.outputfile is None:
        raise ValueError(
            "default merge output is an input; choose --output or --overwrite"
        )


def merge_missing_fields(source, retained, later):
    """Copy absent field expressions, keeping existing values and source text."""
    fields = []
    for entry in later:
        for name, raw in entry.field_sources.items():
            if name in retained.field_sources:
                continue
            retained.field_sources[name] = raw
            if name in entry:
                retained[name] = entry[name]
            fields.append(name)
    if not fields:
        return [], []

    closing = retained.source_end - 1
    line_start = source.rfind("\n", retained.source_start, closing) + 1
    insertion = line_start if not source[line_start:closing].strip() else closing
    original = source[retained.source_start : retained.source_end]
    newline = "\r\n" if "\r\n" in original else "\n"
    indentation = re.search(r"\n([ \t]+)[A-Za-z][\w-]*\s*=", original)
    indent = indentation.group(1) if indentation else "  "
    prefix = "" if source[insertion - 1 : insertion] == "\n" else newline
    changes = []
    if source[retained.append_at - 1] != ",":
        if retained.append_at == insertion:
            prefix = "," + prefix
        else:
            changes.append((retained.append_at, retained.append_at, ","))
    addition = (
        prefix
        + newline.join(indent + retained.field_sources[name] + "," for name in fields)
        + newline
    )
    changes.append((insertion, insertion, addition))
    return changes, fields


def cleanup_duplicates(source, entries, papers=False, keys=False):
    """Edit only removed entries, repeated keys and unambiguous local references."""
    original_keys = {entry.source_start: entry.cite_key for entry in entries}
    removed, changes, messages, removed_pairs = set(), [], [], []
    filled = []
    paper_groups = duplicate_groups(entries)[1] if papers else []
    for group, reasons in paper_groups:
        retained = group[0]
        additions, fields = merge_missing_fields(source, retained, group[1:])
        changes.extend(additions)
        if fields:
            filled.append((retained, fields))
        for entry in group[1:]:
            removed.add(entry.source_start)
            changes.append((entry.source_start, entry.source_end, ""))
            removed_pairs.append((entry, retained))
    survivors = [entry for entry in entries if entry.source_start not in removed]
    reserved, seen = {entry.cite_key for entry in entries}, set()
    if keys:
        for entry in survivors:
            if entry.cite_key in seen:
                base = candidate = author_key(entry)
                suffix = 0
                while candidate in reserved:
                    suffix += 1
                    number, letters = suffix, ""
                    while number:
                        number, remainder = divmod(number - 1, 26)
                        letters = chr(97 + remainder) + letters
                    candidate = base + letters
                reserved.add(candidate)
                changes.append((entry.key_start, entry.key_end, candidate))
                messages.append(
                    "Renamed duplicate key '%s' (%s) to '%s'."
                    % (entry.cite_key, entry_location(entry), candidate)
                )
                entry.cite_key = candidate
            seen.add(entry.cite_key)

    # Follow every original occurrence to its final survivor. A shared old key
    # can refer to several different papers, so only one-target mappings are safe.
    retained_by_start = {entry.source_start: entry for entry in survivors}
    retained_by_start.update(
        (entry.source_start, retained) for entry, retained in removed_pairs
    )
    targets = {}
    for entry in entries:
        retained = retained_by_start[entry.source_start]
        targets.setdefault(original_keys[entry.source_start], {})[
            retained.source_start
        ] = retained.cite_key
    ambiguous = {
        key: sorted(set(values.values()))
        for key, values in targets.items()
        if len(values) > 1
    }
    aliases = {
        key: next(iter(values.values()))
        for key, values in targets.items()
        if len(values) == 1 and key not in values.values()
    }
    messages.extend(
        "Added missing fields to '%s': %s." % (entry.cite_key, ", ".join(fields))
        for entry, fields in filled
    )
    for start, end, replacement in sorted(changes, reverse=True):
        source = source[:start] + replacement + source[end:]
    # Re-read spans after insertion so imported references are remapped too.
    changes = []
    for entry in source_entries(source):
        for name in ("crossref", "xref", "related", "entryset", "xdata"):
            if name not in entry.field_spans:
                continue
            value = entry[name]
            updated = re.sub(
                r"[^\s,]+", lambda m: aliases.get(m.group(), m.group()), value
            )
            if value != updated:
                start, end = entry.field_spans[name]
                changes.append((start, end, updated))
                messages.append(
                    "Updated %s in '%s': %s -> %s."
                    % (name, entry.cite_key, value, updated)
                )
    messages = [
        "Removed duplicate paper '%s' (%s); kept '%s' (%s)."
        % (
            entry.cite_key,
            entry_location(entry),
            retained.cite_key,
            entry_location(retained),
        )
        for entry, retained in removed_pairs
    ] + messages
    for start, end, replacement in sorted(changes, reverse=True):
        source = source[:start] + replacement + source[end:]
    return source, messages, aliases, ambiguous


def tex_file_arguments(arguments, parser):
    """Keep .bib positionals out of argparse's variable-length -t lists."""
    result, index = [], 0
    while index < len(arguments):
        argument = arguments[index]
        index += 1
        flag, separator, attached = argument.partition("=")
        if flag not in ("-t", "--texfile"):
            result.append(argument)
            if argument == "--":
                result.extend(arguments[index:])
                break
            continue
        filenames = [attached] if separator else []
        while index < len(arguments) and not arguments[index].startswith("-"):
            filenames.append(arguments[index])
            index += 1
        count = 0
        for filename in filenames:
            if filename.lower().endswith(".tex"):
                result.append("-t=" + filename)
                count += 1
            elif filename.lower().endswith(".bib"):
                result.append(filename)
            else:
                parser.error(
                    "-t lists require .tex files and .bib bibliography filenames"
                )
        if not count:
            parser.error("-t requires at least one .tex filename")
    return result


def tex_skip_space(source, position):
    """Skip whitespace and TeX comments between a command and its arguments."""
    match = re.compile(r"(?:\s|%[^\n]*(?:\n|$))*").match(source, position)
    return match.end()


def tex_group_end(source, start):
    """Find an argument's end, respecting braces, escapes and comments."""
    opening = source[start : start + 1]
    if opening not in ("{", "[", "("):
        return None
    closing = {"{": "}", "[": "]", "(": ")"}[opening]
    depth, position = 0, start + 1
    while position < len(source):
        char = source[position]
        if char == "\\":
            position += 2
            continue
        if char == "%":
            newline = source.find("\n", position)
            position = len(source) if newline < 0 else newline + 1
            continue
        if char == "}" and depth:
            depth -= 1
        elif char == closing and depth == 0:
            return position + 1
        elif char == "{":
            depth += 1
        position += 1
    return None


def remap_tex_citations(source, aliases, ambiguous):
    """Replace literal keys in standard citation commands, preserving TeX text."""
    single = set(
        (
            "cite nocite citep citet citealp citealt citeauthor citeyear citeyearpar "
            "citenum citefullauthor citeyearnp citeonline "
            "parencite textcite autocite footcite footcitetext smartcite supercite "
            "fullcite footfullcite citeurl citetitle citedate citefield "
            "notecite pnotecite fnotecite"
        ).split()
    )
    multiple = set(
        (
            "cites parencites textcites autocites footcites footcitetexts smartcites "
            "supercites notecites pnotecites fnotecites"
        ).split()
    )
    verbatim = {
        "verbatim",
        "verbatim*",
        "Verbatim",
        "Verbatim*",
        "BVerbatim",
        "LVerbatim",
        "lstlisting",
        "minted",
        "comment",
        "filecontents",
        "filecontents*",
    }
    tokens = re.compile(r"%[^\n]*|\\([A-Za-z@]+)\*?|\\.")
    changes, cited_ambiguous, position = [], set(), 0
    while True:
        match = tokens.search(source, position)
        if match is None:
            break
        position = match.end()
        command = match.group(1)
        if command is None:
            continue
        cursor = tex_skip_space(source, position)
        if command in ("verb", "Verb", "lstinline", "mintinline"):
            if cursor < len(source):
                if command in ("lstinline", "mintinline") and source[cursor] == "[":
                    end = tex_group_end(source, cursor)
                    cursor = tex_skip_space(source, end) if end else len(source)
                if command == "mintinline" and source[cursor : cursor + 1] == "{":
                    end = tex_group_end(source, cursor)
                    cursor = tex_skip_space(source, end) if end else len(source)
                if cursor < len(source):
                    end = (
                        tex_group_end(source, cursor)
                        if source[cursor] == "{"
                        else source.find(source[cursor], cursor + 1) + 1
                    )
                    position = end or len(source)
            continue
        if command == "begin" and source[cursor : cursor + 1] == "{":
            end = tex_group_end(source, cursor)
            if end and source[cursor + 1 : end - 1] in verbatim:
                closing = "\\end{" + source[cursor + 1 : end - 1] + "}"
                end = source.find(closing, end)
                position = len(source) if end < 0 else end + len(closing)
            continue
        if (
            command in ("url", "path", "nolinkurl")
            and source[cursor : cursor + 1] == "{"
        ):
            position = tex_group_end(source, cursor) or len(source)
            continue
        name = command.lower()
        if name not in single and name not in multiple:
            continue
        if name in multiple:
            # biblatex multicites allow two global notes in parentheses.
            for unused in range(2):
                if source[cursor : cursor + 1] == "(":
                    end = tex_group_end(source, cursor)
                    cursor = tex_skip_space(source, end) if end else len(source)
        while True:
            for unused in range(2):
                if source[cursor : cursor + 1] == "[":
                    end = tex_group_end(source, cursor)
                    cursor = tex_skip_space(source, end) if end else len(source)
            if source[cursor : cursor + 1] != "{":
                break
            end = tex_group_end(source, cursor)
            if end is None:
                break
            value = source[cursor + 1 : end - 1]
            # Macro expressions and nested arguments cannot be resolved safely.
            literal = re.sub(r"%[^\n]*", "", value)
            if not re.search(r"[\\{}]", literal):
                for key in re.finditer(r"%[^\n]*|[^\s,%]+", value):
                    old = key.group()
                    if old in ambiguous:
                        cited_ambiguous.add(old)
                    if old in aliases:
                        start = cursor + 1 + key.start()
                        changes.append((start, start + len(old), aliases[old]))
            cursor = tex_skip_space(source, end)
            if name not in multiple:
                break
    parts, position = [], 0
    for start, end, replacement in sorted(changes):
        parts.extend((source[position:start], replacement))
        position = end
    parts.append(source[position:])
    return "".join(parts), len(changes), cited_ambiguous


def prepare_tex_updates(filenames, aliases, ambiguous, bibfile, outfile, bibfiles=()):
    """Read every TeX input before writing anything; reject overlapping paths."""
    plans, messages, seen = [], [], set()
    protected = {
        os.path.realpath(path)
        for path in (bibfile, outfile, outfile + ".bak", *bibfiles)
    }
    for filename in filenames:
        path = os.path.realpath(filename)
        if path in seen:
            continue
        seen.add(path)
        backup = os.path.abspath(filename + ".bak")
        if path in protected or os.path.realpath(backup) in protected:
            raise ValueError(
                "TeX input or backup overlaps a bibliography file: %s" % filename
            )
        with open(path, "rb") as stream:
            original = stream.read()
            mode = os.fstat(stream.fileno()).st_mode
        updated, count, unresolved = remap_tex_citations(
            original.decode("utf-8", errors="surrogateescape"), aliases, ambiguous
        )
        for key in sorted(unresolved):
            messages.append(
                "! Ambiguous citation '%s' in %s; left unchanged. Possible keys: %s; review manually."
                % (key, filename, ", ".join(ambiguous[key]))
            )
        if count:
            plans.append(
                dict(
                    path=path,
                    backup=backup,
                    original=original,
                    updated=updated.encode("utf-8", errors="surrogateescape"),
                    mode=mode,
                )
            )
            messages.append(
                "Updated citations in %s (%d replacements)." % (filename, count)
            )
    for plan in plans:
        if os.path.realpath(plan["backup"]) in seen:
            raise ValueError("TeX backup overlaps another input: %s" % plan["backup"])
    return plans, messages


def stage_file(path, contents, mode):
    """Prepare an atomic replacement beside its target, preserving permissions."""
    descriptor, temporary = tempfile.mkstemp(
        prefix=".bibupdate-", dir=os.path.dirname(path)
    )
    try:
        with os.fdopen(descriptor, "wb") as stream:
            stream.write(contents)
            os.fchmod(stream.fileno(), mode & 0o777)
        return temporary
    except BaseException:
        os.unlink(temporary)
        raise


def write_cleanup(bibliography, plans):
    """Prepare TeX replacements and backups before writing the bibliography."""
    staged = []
    try:
        for plan in plans:
            with open(plan["path"], "rb") as stream:
                if stream.read() != plan["original"]:
                    raise ValueError(
                        "TeX file changed during cleanup: %s" % plan["path"]
                    )
            for target, contents in (
                (plan["path"], plan["updated"]),
                (plan["backup"], plan["original"]),
            ):
                staged.append((stage_file(target, contents, plan["mode"]), target))
        for temporary, target in staged[1::2]:
            os.replace(temporary, target)
        with open_output() as output:
            output.write(bibliography)
        for temporary, target in staged[::2]:
            os.replace(temporary, target)
    finally:
        for temporary, target in staged:
            if os.path.exists(temporary):
                os.unlink(temporary)


def lookup_entry(entry):
    """Run one AMS lookup, buffering its messages for ordered output."""
    lookup_log.messages = []
    try:
        if entry.has_valid_pub_type():
            getattr(entry, options.lookup)()
        return entry, lookup_log.messages
    finally:
        del lookup_log.messages


def lookup_entries(entries):
    """Yield AMS results in input order, with at most --threads jobs queued."""
    if options.threads == 1:
        results = map(lookup_entry, entries)
        for entry, messages in results:
            bib_print(*messages)
            yield entry
        return

    entries = iter(entries)
    with ThreadPoolExecutor(max_workers=options.threads) as pool:
        pending = deque(
            pool.submit(lookup_entry, entry)
            for entry in itertools.islice(entries, options.threads)
        )
        try:
            while pending:
                entry, messages = pending.popleft().result()
                following = next(entries, None)
                if following is not None:
                    pending.append(pool.submit(lookup_entry, following))
                bib_print(*messages)
                yield entry
        finally:
            for future in pending:
                future.cancel()


def positive_integer(value):
    """argparse type for a strictly positive thread count."""
    try:
        number = int(value)
    except ValueError:
        raise argparse.ArgumentTypeError("must be a positive integer")
    if number < 1:
        raise argparse.ArgumentTypeError("must be a positive integer")
    return number


def process_options():
    r"""
    Set up and then parse the options to bibupdate using argparse.
    """
    global options, bibup

    parser = argparse.ArgumentParser(
        description="Update and validate BibTeX files",
        add_help=False,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    parser.add_argument(
        "files",
        nargs="*",
        metavar="FILE",
        help="input bibliography and optional output; with --merge: input bibliographies",
    )

    updating = parser.add_argument_group("Updating and checking")
    lookup = updating.add_mutually_exclusive_group()
    lookup.add_argument(
        "-m",
        "--mrlookup",
        action="store_const",
        const="mrlookup",
        dest="lookup",
        default="mrlookup",
        help="use mrlookup to update bibtex entries (default)",
    )

    lookup.add_argument(
        "-M",
        "--mathscinet",
        action="store_const",
        const="mathscinet",
        dest="lookup",
        help="use mathscinet to update bibtex entries (less flexible)",
    )

    lookup.add_argument(
        "-n",
        "--mref",
        action="store_const",
        const="mref",
        dest="lookup",
        help="use mref to update bibtex entries",
    )

    updating.add_argument(
        "-a",
        "--all",
        action="store_true",
        default=False,
        help="update or validate ALL BibTeX entries",
    )

    updating.add_argument(
        "-x",
        "--arxiv",
        action="store_true",
        default=False,
        help="also add arXiv eprint fields to entries",
    )

    updating.add_argument(
        "-c",
        "--check",
        dest="check",
        action="store_true",
        default=False,
        help="check all entries and report duplicates without writing a file",
    )

    updating.add_argument(
        "--threads",
        type=positive_integer,
        default=1,
        metavar="N",
        help="number of concurrent AMS lookups (default: 1; try 2)",
    )

    listing = parser.add_argument_group("Duplicate listing")
    cleanup = parser.add_argument_group("Duplicate cleanup")
    for group, flag, help_text in [
        (
            listing,
            "--list-duplicates",
            "list duplicate papers and keys without writing a file",
        ),
        (listing, "--list-papers", "list duplicate papers only"),
        (listing, "--list-keys", "list repeated citation keys only"),
        (
            cleanup,
            "--deduplicate",
            "remove duplicate papers and rename repeated keys",
        ),
        (
            cleanup,
            "--remove-duplicate-papers",
            "remove duplicate papers, keeping the first",
        ),
        (
            cleanup,
            "--rename-duplicate-keys",
            "rename repeated keys using authors and year",
        ),
    ]:
        group.add_argument(flag, action="store_true", default=False, help=help_text)

    cleanup.add_argument(
        "--merge",
        action="store_true",
        default=False,
        help="merge two or more .bib files and clean up duplicates",
    )
    cleanup.add_argument(
        "-t",
        "--texfile",
        dest="texfiles",
        nargs="+",
        action="extend",
        default=[],
        metavar="FILE.tex",
        help="update citations in listed TeX files during cleanup (with .bak backups)",
    )

    output = parser.add_argument_group("Output and formatting")
    output.add_argument(
        "--output",
        dest="named_output",
        metavar="FILE.bib",
        help="write the bibliography to this file (also usable with --merge)",
    )
    output.add_argument(
        "-o",
        "--overwrite",
        action="store_true",
        default=False,
        help="overwrite existing bibtex file",
    )

    output.add_argument(
        "-i",
        "--ignored-fields",
        type=str,
        default=None,
        metavar="FIELDS",
        action="append",
        help="space-separated list of bibtex fields to omit (default: %s)"
        % " ".join(DEFAULT_IGNORED_FIELDS),
    )

    output.add_argument(
        "-k",
        "--keep-fonts",
        action="store_true",
        default=False,
        help=r"do NOT replace fonts \Bbb, \germ and \scr in titles",
    )

    output.add_argument(
        "-w",
        "--wrap",
        type=int,
        default=0,
        action="store",
        choices=NonnegativeIntegers(),
        metavar="LEN",
        help="wrap bibtex fields to specified width",
    )

    reports = parser.add_argument_group("Reports and logging")
    reports.add_argument(
        "-q",
        "--quiet",
        dest="quieter",
        action="count",
        default=2,
        help="print brief updates and missed lookups; repeat for errors only",
    )

    reports.add_argument(
        "-l",
        "--log",
        default=sys.stdout,
        type=argparse.FileType("w"),
        help="log messages to specified file (defaults to stdout)",
    )

    help_options = parser.add_argument_group("Help and version")
    help_options.add_argument(
        "-h",
        "--help",
        action="help",
        help="show grouped options and exit",
    )
    help_options.add_argument(
        "-H",
        "--Help",
        action="store_true",
        default=False,
        help="print full program description",
    )

    help_options.add_argument(
        "-v",
        "--version",
        action="version",
        version=bibup.bibupdate_version,
        help="print the program version and exit",
    )

    # Keep the internal debugging switch out of user-facing help.
    parser.add_argument(
        "--debugging", action="count", default=0, help=argparse.SUPPRESS
    )

    # parse the options
    options = parser.parse_intermixed_args(tex_file_arguments(sys.argv[1:], parser))
    options.prog = parser.prog
    options.cleanup_papers = (
        options.merge or options.deduplicate or options.remove_duplicate_papers
    )
    options.cleanup_keys = (
        options.merge or options.deduplicate or options.rename_duplicate_keys
    )
    options.listing = (
        options.list_duplicates or options.list_papers or options.list_keys
    )
    if (options.cleanup_papers or options.cleanup_keys) and (
        options.listing or options.check
    ):
        parser.error(
            "cleanup options cannot be combined with listing options or --check"
        )

    if options.texfiles and not (options.cleanup_papers or options.cleanup_keys):
        parser.error(
            "-t/--texfile requires --merge, --deduplicate, "
            "--remove-duplicate-papers or --rename-duplicate-keys"
        )
    if any(not filename.lower().endswith(".tex") for filename in options.texfiles):
        parser.error("-t accepts only .tex filenames")

    if options.Help:
        # Keep Markdown link definitions for documentation builds, not terminal help.
        help_text = re.sub(r"(?:\n\[[^\]\n]+\]:[^\n]*)+\s*$", "", __doc__)
        bib_print(help_text.rstrip())
        sys.exit()
    if options.merge:
        if len(options.files) < 2:
            parser.error("--merge requires at least two .bib input files")
        if any(not filename.lower().endswith(".bib") for filename in options.files):
            parser.error("--merge accepts only .bib input files")
        if len({os.path.realpath(path) for path in options.files}) != len(
            options.files
        ):
            parser.error("--merge input files must be distinct")
        options.inputfiles = options.files
        options.outputfile = options.named_output
    else:
        if len(options.files) > 2:
            parser.error("use --merge to combine two or more input bibliographies")
        if len(options.files) == 2 and options.named_output is not None:
            parser.error("specify the output either positionally or with --output")
        options.inputfiles = options.files[:1]
        options.outputfile = options.named_output or (
            options.files[1] if len(options.files) == 2 else None
        )
    if options.named_output is not None and options.overwrite:
        parser.error("--output cannot be combined with --overwrite")
    if not options.inputfiles:
        bib_error("no bibtex file specified")
    try:
        options.bibtexfile = argparse.FileType("r")(options.inputfiles[0])
    except argparse.ArgumentTypeError as error:
        parser.error(str(error))

    if options.ignored_fields is None:
        options.ignored_fields = list(DEFAULT_IGNORED_FIELDS)
    else:
        # Explicit -i options replace the defaults; -i "" keeps every field.
        options.ignored_fields = [
            field for group in options.ignored_fields for field in group.lower().split()
        ]

    # if check==True then we want to check everything
    if options.check:
        options.all = True

    # define word wrapping when requested
    if options.wrap != 0:
        bibup.wrapped = lambda field: "\n".join(
            textwrap.wrap(field, options.wrap, subsequent_indent="\t")
        )
    else:
        bibup.wrapped = lambda field: field

    # define debugging, verbose and warning functions
    if options.debugging > 0:
        options.quieter = 2
        bibup.debugging = True
        bibup.debug = bib_print
        if options.debugging == 4:
            # drop into the debugger
            import pdb

            pdb.set_trace()
    else:
        bibup.debug = lambda *arg: None
    # 2 == compact references, 3 (-q) == brief updates and warnings,
    # 4 (-qq) == errors only. Check-mode references are always printed.
    bibup.verbose = bib_print if options.quieter == 2 else lambda *arg: None
    bibup.warning = bib_print if options.quieter <= 3 else lambda *arg: None

    # a shorthand for fixing the fonts (to avoid an if-statement when calling it)
    bibup.fix_fonts = replace_fonts if not options.keep_fonts else lambda title: title


def output_filename():
    """Return the selected bibliography output path."""
    if options.overwrite:
        newfile = options.bibtexfile.name
    elif options.outputfile is not None:
        newfile = options.outputfile
    else:
        directory, base = os.path.split(options.bibtexfile.name)
        prefix = "merged_" if options.merge else "updated_"
        newfile = os.path.join(directory, prefix + base)
    return newfile


def open_output():
    """Open the selected bibliography output after backing up an existing file."""
    newfile = output_filename()
    if os.path.isfile(newfile) and os.path.getsize(newfile) > 0:
        try:
            shutil.copyfile(newfile, newfile + ".bak")
        except IOError:
            bib_error("unable to create backup file for %s" % newfile)
    try:
        return open(newfile, "w", newline="")
    except IOError:
        bib_error("unable to open new bibtex file %s" % newfile)


def main():
    r"""
    Open the files and the delegate all of the hard work to the BibTeX class.
    """
    process_options()

    entries = None
    try:
        if options.merge:
            papers, entries = merge_sources(options.inputfiles, options.bibtexfile)
            validate_merge_output(options.inputfiles, output_filename())
        else:
            if (
                options.listing or options.cleanup_papers or options.cleanup_keys
            ) and hasattr(options.bibtexfile, "reconfigure"):
                options.bibtexfile.reconfigure(newline="")
            papers = options.bibtexfile.read()
            options.bibtexfile.close()
    except (ValueError, OSError) as error:
        bib_error(str(error))

    if "@" not in papers or (options.merge and not entries):
        bib_error("no bibtex entries found in %s" % ", ".join(options.inputfiles))
    first_entry = papers.index("@")

    if options.listing or options.cleanup_papers or options.cleanup_keys:
        try:
            if entries is None:
                entries = list(source_entries(papers))
            if options.listing:
                for warning in duplicate_warnings(
                    entries,
                    keys=options.list_duplicates or options.list_keys,
                    papers=options.list_duplicates or options.list_papers,
                ):
                    bibup.warning(warning)
            else:
                updated, messages, aliases, ambiguous = cleanup_duplicates(
                    papers,
                    entries,
                    papers=options.cleanup_papers,
                    keys=options.cleanup_keys,
                )
                plans, tex_messages = prepare_tex_updates(
                    options.texfiles,
                    aliases,
                    ambiguous,
                    options.bibtexfile.name,
                    output_filename(),
                    options.inputfiles,
                )
                write_cleanup(updated, plans)
                if options.merge:
                    messages.insert(
                        0,
                        "Merged %d bibliography files into '%s'."
                        % (len(options.inputfiles), output_filename()),
                    )
                for message in messages + tex_messages:
                    bibup.warning(wrap_report(message))
        except (ValueError, OSError) as error:
            bib_error(str(error))
        return

    entries = parse_entries(papers)
    if options.check:
        entries = list(entries)
        for warning in duplicate_warnings(entries):
            bibup.warning(warning)

    # if we are checking for errors then we check EVERYTHING but, in this case,
    # we don't need to create a new bibtex file
    if not options.check:
        newbibfile = open_output()
        newbibfile.write(papers[:first_entry])

    # Only AMS lookups run in workers. arXiv stays on this thread so its
    # requests remain sequential and respect the shared delay between calls.
    for bt in lookup_entries(entries):
        if bt.has_valid_pub_type():
            if options.arxiv:
                bt.arxiv()
            bt.report_update()

        # now write the new (hopefully) improved entry
        if not options.check:
            newbibfile.write("%s\n\n" % bt)

    if not options.check:
        newbibfile.close()


# The following __doc__ string is used both to print the documentation and by
# setup.py to automatically generate the README file. It is defined before
# main() runs so that the -H option can print the full documentation.
__doc__ = r"""# bibupdate

{0.description}

Usage:

    bibupdate [-h|-H] [-a] [-x] [-c] [-k] [-i FIELDS] [-l LOG]
              [-m | -M | -n] [-o] [-q] [--threads N] [-w LEN]
              [--deduplicate] [--remove-duplicate-papers] [--rename-duplicate-keys]
              [--list-duplicates] [--list-papers] [--list-keys]
              [-t FILE.tex [FILE.tex ...]]
              [--output FILE.bib] bibtexfile [outputfile]
    bibupdate --merge [options] FILE.bib FILE.bib [FILE.bib ...]

This is a command line tool for updating the entries in a [BibTeX] file using the
tools provided by the American Mathematical Society: [mrlookup], [MathSciNet]
and [MRef]. By default [bibupdate] uses [mrlookup] to update `article`, `book`,
`inproceedings` and `incollection` entries when they do not have an
`mrnumber` field. Entries are not updated if they already have an
`mrnumber` field, even an empty one, unless `--all` or `--check`
is used. The `-x` option makes [bibupdate] look for each paper in the
[arXiv] and add `eprint` and `archiveprefix` fields for all the papers
that it finds. In addition, [bibupdate] is able to merge bibliography
files, removing duplicate papers and keys in the process and,
optionally, updating citations to renamed duplicate keys in [LaTeX] files.

By default, [bibupdate] does not change your original file. Instead, it
writes the updated entries to a new file, with the filename given by 
adding an `updated_` prefix, so *file.bib* becomes *updated_file.bib*.
To update the bib file in place, use `--overwrite`, or give a output
filename. Existing output files are backed up with `.bak` extensions,
but `--overwrite` should still be used with care.

This should be used with care because you
can potentially lose data.

**Options by task**:

**Updating and checking**

    -m, --mrlookup        use mrlookup to update bibtex entries (default)
    -M, --mathscinet      use mathscinet to update bibtex entries (less flexible)
    -n, --mref            use mref to update bibtex entries
    -a, --all             update or validate ALL BibTeX entries
    -x, --arxiv           add arXiv eprint fields to entries
    -c, --check           check all entries and report duplicates without writing a file

**Duplicate listing**

    --list-duplicates     list duplicate papers and keys without writing a file
    --list-papers         list duplicate papers only
    --list-keys           list repeated citation keys only

**Duplicate cleanup**

    --deduplicate         remove duplicate papers and rename repeated keys
    --remove-duplicate-papers
                          remove duplicate papers, keeping the first
    --rename-duplicate-keys
                          rename repeated keys using authors and year
    --merge               merge two or more .bib files and clean up duplicates
    -t, --texfile FILE.tex [FILE.tex ...]
                          update citations in listed TeX files during cleanup

**Output and formatting**

    --output FILE.bib     write the bibliography to this file
    -o, --overwrite       update/overwrite the bibtex file (use with care)
    -i FIELDS, --ignored-fields FIELDS
                          space-separated list of bibtex fields to ignore
    -k, --keep-fonts      do NOT replace fonts \Bbb, \germ and \scr in titles
    -w LEN, --wrap LEN    wrap bibtex fields to specified width
    --threads N           number of concurrent AMS lookups (default: 1)

**Reports and logging**

    -q, --quiet           brief updates and missed lookups
    -qq                   only print errors
    -l LOG, --log LOG     log messages to specified file (defaults to stdout)

**Help and version**

    -h, --help            show grouped options and exit
    -H, --Help            print full program description

**Note:**
As described below, you should check the updated `.bib` file for errors
before deleting the original [BibTeX] file.

[BibTeX] is widely used by the [LaTeX] community to maintain publication databases.
This script attempts to add missing fields to the papers in a [BibTeX] database
file by querying [mrlookup] and getting the missing publication data from there.
This is not completely routine because an [mrlookup] search requires either the
names of the authors, or the title of the article, and both of these can have
non-standard representations. If the article is already published, then it is
also possible to use the publication year and its page numbers as part
of the search. To search on [mrlookup] we:

- use the first author's surname (accents and compound surnames can be problematic)
- use numeric page ranges, or a single numeric page, when available
- include the year for books, or when a numeric first page is available
- use the cleaned title when a numeric first page is not available

The author query uses only the first author's surname because [mrlookup] does not
accept multiple author names joined by 'and'. Braces used to protect capitals in
titles (for example `{{Kac-Moody}}`) are removed when searching and
matching, with the title stored in the [BibTeX] file retaining its braces.

If there is a unique match from [mrlookup], then [bibupdate] updates the entry in
the [BibTeX] file using the field values returned by an [mrlookup] search, except for
the citation key and any fields excluded by `--ignored-fields`. Any fields that are
not returned by [mrlookup], such as `eprint`, are retained. By default, a message is
printed whenever an entry in the database is changed. If the title of the retrieved paper
does not (fuzzily) match that of the original article, then the entry is NOT
updated and a warning message is printed.

Although some care is taken to ensure that the new [BibTeX] entries correspond
to the original entry in the [BibTeX] file, there is always a (small?)
chance the new entry corresponds to a completely different paper. In my
experience this happens rarely, and mostly with unpublished manuscripts. In any
case, before you delete your original [BibTeX] file *you are strongly advised to
check the updated BibTeX file carefully for errors!*

By default, the program prints a compact listing of any updated paper in the
[BibTeX] file, including any added [arXiv] fields. This can generate a
lot of output the first time [bibupdate] is run on large files, later runs
produce much less output.

As [bibupdate] calls [mrlookup], this program will only be useful if you have
papers in your database that are listed in [MathSciNet]. As described below it is
also possible to call [MathSciNet] directly, however, this is less flexible
because the `mrnumber` field for each paper is required.

I wrote this script because I wanted to automatically update the papers
in my [BibTeX] files when they were published, so that the file has
up-to-date information on all of the papers in the file, including
journal names, [arXiv] links, and DOIs for each paper. As a bonus
the script helped me to correct many minor errors that had crept
into my [BibTeX] file over the years (for example, incorrect page
numbers and publication years).

## Options and defaults

By default, [bibupdate] uses [mrlookup], sending one AMS lookup request at
a time. It prints a compact listing for each entry in the file that is
updated, writing the updated [BibTeX] file to `updated_file.bib` if the
original input file was `file.bib`.

### Updating and checking

#### `-m, --mrlookup`

Use [mrlookup] to update [BibTeX] entries (default)

#### `-M, --mathscinet`

Use [MathSciNet] to update [BibTeX] entries

#### `-n, --mref`

Use [MRef] to update [BibTeX] entries

By default [mrlookup] is used. Its main advantage is that it has a more
flexible search mechanism using author, title, year and page numbers,
so it can find papers even when they do not yet have an `mrnumber`.

With `-M`, the entry is looked up using its `mrnumber`. Entries with missing,
empty or non-numeric MR numbers are skipped, including with `--all` and `--check`.

With `-n`, the full [BibTeX] entry is handed to [MRef] as a free-form
reference. The best matching record is then checked against the original
title. Like [mrlookup], [MRef] normally checks only `article`, `book`,
`inproceedings` and `incollection` entries without an `mrnumber` field.
Use `--all` to check all recognised entry types.

#### `-a, --all`

Update or validate ALL BibTeX entries

#### `-x, --arxiv`

Update the arXiv entry for each paper

When the paper is found, add `eprint` and `archiveprefix` fields to
the [BibTeX] entry:

    eprint = {{2211.07069}},
    archiveprefix = {{arXiv}}

[bibupdate] searches the [arXiv] using the significant words in the
title. A paper returned by the [arXiv] is counted as a match only
when there is a unique paper with a sufficiently good (fuzzy) match.
The `-x` option can be combined with any of the `-m`, `-M` or
`-n` lookup tools. Entries with an existing `eprint` field, even an
empty one, are skipped unless `-a` or `--check` is used. Entries
without a usable title cannot be searched.

As requested by the [arXiv], [bibupdate] waits at least {arxiv_delay}
seconds after each completed [arXiv] request before sending the next one.
Updating a large database with `-x` can take a long time.

#### `-c, --check`

Check all entries and report duplicates, printing compact listings for each
database entry that would be changed. No output file is written. This option
implies `--all`.

#### `--threads N`

Number of concurrent AMS lookups

Use `--threads N` to run `N` searches in parallel. [arXiv] lookups remain
sequential and keep their delay between requests, regardless of the
thread count.

### Duplicate listing

#### `--list-duplicates`

List any duplicated papers, or citation keys, in the [BibTeX] database.

#### `--list-papers`

List duplicate papers only, including possible title-and-author matches

Two entries are treated as the same paper if they share a DOI, an MR number
or an arXiv identifier. As a fallback, entries whose normalized titles and
author lists (surnames and first initials) agree are reported as *possible*
duplicates. Entries linked through a chain of such matches are grouped
together. The same rules are used by `--check`, `--remove-duplicate-papers`
and `--deduplicate`.

#### `--list-keys`

List repeated citation keys only

### Duplicate cleanup

#### `--deduplicate`

Remove duplicate papers and rename repeated citation keys

This offline cleanup first removes repeated papers, then renames repeated
keys among the remaining entries. As usual, the updated file is written
to `updated_file.bib` unless `--overwrite` is used.
Existing fields and formatting are preserved; missing fields are added from
later copies. Ignored-field, font-conversion and wrapping options do not apply
to cleanup. Cleanup cannot be combined with listing options or `--check`, and
the lookup options (`-m`, `-M`, `-n`, `-a` and `-x`) are ignored.

#### `--remove-duplicate-papers`

Remove repeated papers, keeping the first occurrence in file order

Papers are matched as described under `--list-papers`. Title-and-author
matching is heuristic, so inspect the output of `--list-papers` before
removing entries.

The first entry is the base. Missing fields are copied from later entries in
file order. Existing fields, including empty fields, are kept; if several copies
supply the same missing field, the first value wins. Field expressions and
macros are copied without expansion. Each removal reports the removed key and
the retained key, with source lines. Literal `crossref`, `xref`, `related`,
`entryset` and `xdata` references inside the bibliography are updated where
the mapping is unambiguous. Use `-t` to update citations in external [LaTeX]
files; otherwise these must be updated manually.

#### `--rename-duplicate-keys`

Rename repeated keys without deleting papers. The first occurrence of each
key is kept. Duplicated key names are replaced with a key formed
from the authors' surnames and the year, for example `AcharRiche2019`.
Editors' names are used if there are no authors. If necessary, a suffix
of `a`, `b`, ..., `z`, `aa`, etc. is added to ensure that the key names
are unique.

Using `--remove-duplicate-papers` together with `--rename-duplicate-keys` is equivalent
to `--deduplicate`.

#### `--merge`

Merge two or more BibTeX files, removing duplicate papers and keys

The input files must have a `.bib` extension and are processed in the
order given. When removing duplicates, the rules used by `--deduplicate`
apply: the first copy of each paper and key is kept, and any repeated
keys are renamed using the authors' names and the publication year.
Fields missing from the first copy are added from
later entries, with earlier values having precedence. Retained entries,
comments, preambles and string macros keep their original formatting;
macros are not expanded for duplicate matching.

Merging is offline and cannot be combined with `--check` or duplicate
listing options.

By default, the merged [BibTeX] file is written to `merged_first.bib` if 
the first [BibTeX] file is called `first.bib`. Use `--output` to
write to a different filename, or `--overwrite` to overwrite the
first [BibTeX] file, which will be backed up with a `.bak` extension.

    bibupdate --merge first.bib second.bib --output combined.bib
    bibupdate --merge first.bib second.bib -t paper.tex --output combined.bib

Use `-t`/`--texfile` to update unambiguous citations in associated [LaTeX]
files, which are backed up with `.bak` extensions.

#### `-t, --texfile FILE.tex [FILE.tex ...]`

Update citation keys in the listed [LaTeX] files when cleaning duplicate entries

This option requires `--merge`, `--deduplicate`, `--remove-duplicate-papers` or
`--rename-duplicate-keys`. The `.tex` and `.bib` extensions distinguish the
TeX files from the bibliography and optional output file, for example:

    bibupdate --deduplicate -t chapter.tex appendix.tex papers.bib

Only unambiguous citation key mappings are applied to the [LaTeX] files.
If different papers share a key, citations of that key in the [LaTeX] files
are left unchanged and a warning is printed. The later papers are given new
unique keys in the merged [BibTeX] file.

Standard literal citation commands such as `\cite`, `\citep`, `\nocite`,
`\parencite` and `\textcites` are supported, including optional notes and
comma-separated key lists. Comments, verbatim text and other commands are
preserved. Custom citation macros are not expanded, and included files are
only processed if explicitly listed.

Before changing a TeX file, its original contents are saved with a `.bak`
extension, for example `chapter.tex.bak`. Unchanged files are not rewritten
or backed up. Existing backups of changed files are replaced.

### Output and formatting

#### `--output FILE.bib`

Write the bibliography to the specified file

Use this option to specify the name of the new [BibTeX] output file. An
existing non-empty output file is first backed up with a `.bak` extension.
This option cannot be combined with `--overwrite`, or with an output file
given as a second positional argument.

#### `-o, --overwrite`

Overwrite the existing [BibTeX] file with the updated version. The original
file is first backed up with a `.bak` extension. This option cannot be
combined with `--output`.

#### `-i FIELDS, --ignored-fields=FIELDS`

A space-separated list of [BibTeX] fields to omit when writing the updated
[BibTeX] file

By default [bibupdate] removes the following fields from each [BibTeX] entry:

{ignored_fields}

The `-i` option replaces this default list. Repeated `-i` options are combined;
field names are case-insensitive and separated by spaces. An empty string keeps
every field:

    bibupdate -i "coden fjournal" file.bib   # ignore coden and fjournal
    bibupdate -i coden -i fjournal file.bib  # ignore coden and fjournal
    bibupdate -i "" file.bib                 # do not ignore any fields

#### `-k, --keep-fonts`

Use `-k` to retain legacy LaTeX font names in titles. By default, the
following font substitutions are automatically applied:

- `\Bbb X` --> `\mathbb{{X}}`
- `\scr X` --> `\mathcal{{X}}`
- `\germ X` --> `\mathfrak{{X}}`

This applies both to input titles and to titles retrieved from AMS.

#### `-w LEN, --wrap LEN`

Wrap [BibTeX] fields to specified width

By default, lines are not wrapped (a wrap length of `0`). A positive
wrap length `LEN` wraps each field value to that width, with a tab before
continuation lines.

### Reports and logging

#### `-q, --quiet`

Print fewer messages

By default, [bibupdate] prints a compact reference for each paper that
is updated, and a warning for each paper for which the AMS lookup fails.
With `-q` (or `--quiet`) only a one-line message is printed for updated
papers, together with the warnings for failed lookups. With `-qq` only errors
are printed. Failed lookups are printed in red when the output is a terminal.

#### `-l LOG, --log LOG`

Log output to file (defaults to stdout)

By default, reports and warnings go to standard output. Use `-l` to write
them to a file. Existing log files are overwritten. Fatal errors are always
printed to standard error.

### Help and version

#### `-h, --help`

Print a short list of command-line options

#### `-H, --Help`

Print the full-help message

## Known issues

- **BibTeX syntax:** the parser uses regular expressions rather than a full
  BibTeX grammar. Use brace-delimited entries with one field per line and
  braced values or simple words. Multiple fields on one line, quoted
  values, `#` concatenation, string-macro expansion, and
  fields containing `@` or `={{...}}` can be misread or omitted. Inspect the
  output carefully when using these forms. Duplicate listing and cleanup use
  a source scanner that preserves quoted and nested values, comments and macros;
  macros are not expanded for duplicate matching.
- **Title matching:** capitalization braces and apostrophes are removed before
  searching and comparing. Mathematics is also removed, which can make distinct
  titles look similar. TeX commands and accents are not fully decoded, so
  equivalent spellings can still fail to match. A unique fuzzy title match is
  a heuristic, not a guarantee that a paper has been correctly identified.
- **Author names:** [mrlookup] uses only the first author's surname. A compound
  surname is preserved in `surname, given names` form; without a comma, only
  the last word is used. Accents, name particles and alternative spellings can
  therefore affect searches.
- **Page numbers:** numeric ranges and single numeric pages are supported.
  Article identifiers and prefixed page numbers are not handled reliably.
  When only non-numeric page numbers are given, the title is used
  instead. For non-book entries the year is then omitted from the query.
- **Ambiguous or missing results:** an AMS entry is updated only when
  there is a unique match. [BibTeX] entries are not changed when no
  matches, or multiple matches, are found. Entries without a usable title
  are more likely to receive an incorrect match. By default, skipped
  entries, such as those with an existing `mrnumber`, are never changed.
- **arXiv searches:** searches use significant title words and examine at most
  ten results. Titles dominated by mathematics or short words are more
  likely to receive false matches. As with [mrlookup] searches, the [arXiv]
  fields for a paper are updated only if there is a unique match.
- **Network services:** connections can fail, time out or return unexpected
  data. Requests use a {url_timeout}-second timeout. Connection errors and
  timeouts produce warnings, and processing continues. A missed lookup does
  not prove that the paper is absent from the database.

## Licence

{0.author}

{0.license}

[bibupdate] Version {0.version}. Copyright (C) {0.copyright}

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License ([GPL]) as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

[BibTeX]: http://www.bibtex.org/
[mrlookup]: https://mathscinet.ams.org/mathscinet/freetools/mrlookup
[MathSciNet]: https://mathscinet.ams.org/mathscinet/publications-search
[MRef]: https://mathscinet.ams.org/mathscinet/freetools/mref
[bibupdate]: {0.url}
[arXiv]: http://arxiv.org/
[LaTeX]: https://latex-project.org/
[GPL]: http://www.gnu.org/licenses/gpl.html
""".format(
    bibup,
    ignored_fields="\n".join("- `" + field + "`" for field in DEFAULT_IGNORED_FIELDS),
    arxiv_delay=arxiv_delay,
    url_timeout=url_timeout,
)

##############################################################################
if __name__ == "__main__":
    main()
