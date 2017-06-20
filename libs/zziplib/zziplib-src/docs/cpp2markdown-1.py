#! /usr/bin/env python
import pygments.lexers.compiled as lexer
import optparse
import re
from pygments.token import Token
import logging

logg = logging.getLogger(__name__)

FileComment = "FileComment"
FileInclude = "FileInclude"
FunctionComment = "FunctionComment"
FunctionPrototype = "FunctionPrototype"

# use the markdown lexer to identify elements
# then filter only those we want. The returned
# token list is more global flagging the role
# of each token for the manual generation.
class CppToMarkdown:
    def __init__(self):
        self.alldefinitions = 0
        self.internaldefs = ["static"]
        self.filecomment_done = ""
        self.fileinclude_done = ""
        self.filecomment_text = ""
        self.fileinclude_text = ""
        self.comment_text = ""
        self.function_text = ""
        self.nesting = 0
    def split_copyright(self, text):
        # there are two modes - the copyright starts in the first line
        # and the source description follows or the other way round.
        lines = text.split("\n")
        if len(lines) <= 2:
            return "", text
        introtext = [lines[0]]
        copyright = [lines[0]]
        check1 = re.compile(r"^\s[*]\s+[(][c][C][)]")
        check2 = re.compile(r"^\s[*]\s+\b[Cc]opyright\b")
        empty1 = re.compile(r"^\s[*]\s*$")
        state = "intro"
        for i in xrange(1,len(lines)-1):
            line = lines[i]
            if state == "intro":
                if empty1.match(line):
                    introtext += [ line ]
                    continue
                if check1.match(line) or check2.match(line):
                    state = "copyrightfirst"
                    copyright += [ line ]
                else:
                    state = "introtextfirst"
                    introtext += [ line ]
            elif state == "copyrightfirst":
                if empty1.match(line):
                    state = "introtextlast"
                    introtext += [ line ]
                else:
                    copyright += [ line ]
            elif state == "introtextfirst":
                if check1.match(line) or check2.match(line):
                    state = "copyrightlast"
                    copyright += [ line ]
                else:
                    introtext += [ line ]
            elif state == "copyrightlast":
                copyright += [ line ]
            elif state == "introtextlast":
                introtext += [ line ]
            else:
                logg.fatal("UNKNOWN STATE %s", state)
        introtext += [lines[-1]]
        copyright += [lines[-1]]
        logg.debug("@ COPYRIGHT\n %s", copyright)
        logg.debug("@ INTROTEXT\n %s", introtext)
        return "\n".join(copyright), "\n".join(introtext)
    def commentblock(self, text):
        prefix = re.compile(r"(?s)^\s*[/][*]+([^\n]*)(?=\n)")
        suffix = re.compile(r"(?s)\n [*][/]\s*")
        empty = re.compile(r"(?s)\n [*][ \t]*(?=\n)")
        lines1 = re.compile(r"(?s)\n [*][ ][\t]")
        lines2 = re.compile(r"(?s)\n [*][ ]")
        lines3 = re.compile(r"(?s)\n [*][\t][\t]")
        lines4 = re.compile(r"(?s)\n [*][\t]")
        text = suffix.sub("\n", text)
        text = prefix.sub("> \\1\n", text)
        text = empty.sub("\n", text)
        text = lines1.sub("\n     ", text)
        text = lines2.sub("\n", text)
        text = lines3.sub("\n         ", text)
        text = lines4.sub("\n     ", text)
        return text
    def functionblock(self, text):
        empty = re.compile(r"(?s)\n[ \t]*(?=\n)")
        text = "    " + text.replace("\n", "\n    ")
        text = empty.sub("", text) 
        return text
    def functionname(self, text):
        check1 = re.compile(r"^[^()=]*(\b\w+)\s*[(=]")
        found = check1.match(text)
        if found:
            return found.group(1)
        check2 = re.compile(r"^[^()=]*(\b\w+)\s*$")
        found = check2.match(text)
        if found:
            return found.group(1)
        return ""
    def run(self, filename):
        filetext = open(filename).read()
        for line in self.process(filetext, filename):
            print line
    def process(self, filetext, filename=""):
        section_ruler = "-----------------------------------------"
        copyright = ""
        for token, text in self.parse(filetext):
            if token == FileInclude:
                yield "## SOURCE " + filename.replace("../", "")
                yield "    #" + text.replace("\n", "\n    ")
            elif token == FileComment:
                yield "### INTRODUCTION"
                copyright, introduction = self.split_copyright(text)
                yield self.commentblock(introduction)
            elif token == FunctionPrototype:
                name = self.functionname(text)
                yield section_ruler
                yield "### " + name
                # yield '<a id="%s"></a>' % name
                yield "#### NAME"
                yield "    " + name
                yield "#### SYNOPSIS"
                yield self.functionblock(text)
            elif token == FunctionComment:
                if text:
                    yield "#### DESCRIPTION"
                    yield self.commentblock(text)
            else:
                if text:
                    yield "#### NOTES"
                    print token, text.replace("\n", "\n  ")
        if copyright:
            yield section_ruler
            yield "### COPYRIGHT"
            yield self.commentblock(copyright)            
    def isexported_function(self):
        function = self.function_text.strip().replace("\n"," ")
        logg.debug("@ --------------------------------------") 
        logg.debug("@ ALLDEFINITIONS %s", self.alldefinitions)
        if function.startswith("static ") and self.alldefinitions < 3:
            logg.debug("@ ONLY INTERNAL %s", function)
            return False
        if not self.comment_text:
            if not self.alldefinitions:
                logg.info("@ NO COMMENT ON %s", function)
                return False
            else:
                logg.warn("@ NO COMMENT ON %s", function)
        text = self.comment_text
        if text.startswith("/**"): return True
        if text.startswith("/*!"): return True
        if text.startswith("///"): return True
        if text.startswith("//!"): return True
        if self.alldefinitions >= 1:
            if text.startswith("/*"): return True
            if text.startswith("//"): return True
        if self.alldefinitions >= 2:
            return True
        logg.debug("@ NO ** COMMENT %s", self.function_text.strip())
        defs = self.function_text
        return False
    def parse(self, filetext):
        c = lexer.CLexer()
        for token, text in c.get_tokens(filetext):
            logg.debug("|| %s %s", token, text.replace("\n", "\n |"))
            # completion
            if token != Token.Comment.Preproc and self.fileinclude_done == "no":
                    yield FileInclude, self.fileinclude_text
                    if self.filecomment_text:
                        yield FileComment, self.filecomment_text
                    self.fileinclude_done = "done"
            # parsing
            if token == Token.Comment.Multiline:
                if not self.filecomment_done:
                    self.filecomment_done = "done"
                    self.filecomment_text = text
                    # wait until we know it is not a function documentation
                    self.comment_text = text
                else:
                    self.comment_text = text
            elif token == Token.Comment.Preproc and "include" in text:
                if not self.fileinclude_done:
                    self.fileinclude_done = "no"
                    self.fileinclude_text += text
                    self.comment_text = ""
            elif token == Token.Comment.Preproc and self.fileinclude_done == "no":
                if not "\n" in self.fileinclude_text:
                    self.fileinclude_text += text
                self.comment_text = ""
            elif token == Token.Comment.Preproc:
                    self.comment_text = ""
                    self.function_text = ""
            elif token == Token.Operator and text == "=":
                if not self.nesting and self.function_text.strip():
                    if self.isexported_function():
                        yield FunctionPrototype, self.function_text
                        yield FunctionComment, self.comment_text
                self.comment_text = ""
                self.function_text = ""
            elif token == Token.Punctuation and text == ";":
                self.comment_text = ""
                self.function_text = ""
            elif token == Token.Punctuation and text == "{":
                if not self.nesting and self.function_text.strip():
                    if self.isexported_function():
                        yield FunctionPrototype, self.function_text
                        yield FunctionComment, self.comment_text
                self.comment_text = ""
                self.function_text = ""
                self.nesting += 1
            elif token == Token.Punctuation and text == "}":
                self.nesting -= 1
                self.comment_text = ""
                self.function_text = ""
            else:
                if not self.nesting:
                    self.function_text += text
                else:
                    pass # yield "|",text
                

if __name__ == "__main__":
    _o = optparse.OptionParser()
    _o.add_option("-v", "--verbose", action="count", default=0)
    _o.add_option("-a", "--all", action="count", default=0,
                  help="include all definitions in the output (not only /**)")
    opt, args = _o.parse_args()

    logg.addHandler(logging.StreamHandler())
    if opt.verbose:
        logg.setLevel(logging.WARN - 10 * opt.verbose)
    
    c = CppToMarkdown()
    if opt.all:
        c.alldefinitions = opt.all
    for arg in args:
        c.run(arg)
    
    


