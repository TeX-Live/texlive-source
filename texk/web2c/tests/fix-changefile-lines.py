#!/usr/bin/env python3
# $Id$
# Applied to tex.ch and mf.ch on 2024-06-03, following the request at:
# https://tug.org/pipermail/tex-k/2024-June/004064.html
"""
Reads a WEB file and a change file and writes a change file to stdout with
potentially corrected part, section and line numbers.
Written by Tyge Tiessen, 2024. Public domain.
"""

import getopt, os, re, sys

USAGE = f"""
Usage: {os.path.basename(sys.argv[0])} [options] <web file> <change file>

Reads a WEB file and a change file and writes a change file to stdout with
potentially corrected part, section and line numbers.

Options influence the '@x [part.section] l.line - text' format:
    -p, --parts     Suppress the 'part' (starred section) number
    -s, --sections  Suppress the (unstarred) 'section' number
    -l, --lines     Suppress the 'l.line' number
    -h, --hyphens   Suppress the '-'
    -t, --texts     Suppress the 'text'

Written by Tyge Tiessen, 2024. Public domain.
""".strip()


def main():
    # Handle command-line options
    opt_handler = OptHandler()

    # Read WEB file
    web_reader = WebReader(opt_handler.args[0])

    # Read change file
    ch_reader = ChangeReader(opt_handler.args[1])

    # Run through the two files in parallel
    ch_reader.traverse(web_reader, opt_handler)

    for line in ch_reader.get_lines():
        print(line)


class WebReader:
    """An iterator that produces the lines of a WEB file while keeping track
    of the current part and section number.
    """

    def __init__(self, web_file):
        self._pos = 0
        self.part_cnt = 0
        self.section_cnt = 0
        try:
            self._web_file = open(web_file, "r")
        except OSError:
            eprint(f"Could not open {web_file}")
            print(USAGE)
            sys.exit(1)
        with self._web_file:
            self._web_lines = [line.rstrip() for line in self._web_file]

    def next_line(self):
        """Returns the triple of current part, section and line numbers, as
        well as the next line. Updates part and section numbers.
        """
        if self._pos >= len(self._web_lines):
            return None
        line = self._web_lines[self._pos]
        self._pos += 1

        part = self.part_cnt
        section = self.section_cnt
        line_number = self._pos

        # Look for starred section == part
        if line.startswith("@*"):
            self.part_cnt += 1
            self.section_cnt += 1

        # Look for unstarred section
        if line.startswith("@ ") or line == "@":
            self.section_cnt += 1

        # Look for '@i'nclude line
        result = re.match("^@i \"?(\\w+(\\.\\w+)?)\"?", line)
        if result:
            inc_reader = WebReader(result[1])
            while inc_reader.next_line():
                pass
            self.part_cnt += inc_reader.part_cnt
            self.section_cnt += inc_reader.section_cnt
            # Ignore line count in include file; we're only one step beyond

        return (part, section, line_number), line


class ChangeReader:
    """Reads the lines of a change file while updating the tags with
    information from the corresponding WEB file.
    """

    def __init__(self, change_file):
        self._pos = 0
        self._chunk_start = None
        self._match_lines = None
        try:
            self._change_file = open(change_file, "r")
        except OSError:
            eprint(f"Could not open {change_file}")
            print(USAGE)
            sys.exit(1)
        with self._change_file:
            self._lines = [line.rstrip() for line in self._change_file]

    def advance_to_next_chunk(self):
        """Find the next change chunk. Store where it starts and
        the lines to be matched.
        """

        while self._pos < len(self._lines):
            line = self._lines[self._pos]
            if line.startswith("@x"):
                self._chunk_start = self._pos
                while True:
                    self._pos += 1
                    if self._pos >= len(self._lines):
                        eprint(
                            f"ERROR: Missing @y for @x on l.{self._chunk_start + 1} in change file"
                        )
                        sys.exit(1)
                    line = self._lines[self._pos]
                    if line.startswith("@y"):
                        self._match_lines = self._lines[
                            self._chunk_start + 1 : self._pos
                        ]
                        return True
            self._pos += 1
        return False

    def find_match_in_web(self, web_reader):
        """Find the match for the current change chunk in the WEB file.
        Returns the part number and section number just before the first match line,
        as well as the line number of the first match line in the WEB file.
        """
        while True:
            (part, section, line_number), tex_line = web_reader.next_line()
            if tex_line is None:
                eprint("ERROR: Could not find match for line:")
                eprint(f"  {self._match_lines[0]}")
                sys.exit(1)
            if tex_line == self._match_lines[0]:
                for i in range(1, len(self._match_lines)):
                    _, tex_line = web_reader.next_line()
                    if tex_line is None or tex_line != self._match_lines[i]:
                        eprint("ERROR: Could not match all lines following match line:")
                        eprint(f"  {self._match_lines[0]}")
                        sys.exit(1)

                return part, section, line_number

    def traverse(self, web_reader, opt_handler):
        """Go through all individual change chunks while updating their tags."""
        while self.advance_to_next_chunk():
            part, section, line_number = self.find_match_in_web(web_reader)

            # Attempt to catch the case where something is inserted just before
            # the start of a section.
            match_start = self._match_lines[0].strip()[:2]
            for repl_index in range(self._pos + 1, len(self._lines)):
                repl_start = self._lines[repl_index].strip()[:2]
                # CWEB @qcomments@> are ignored; see ctwill-w2c.ch
                if repl_start != "@q":
                    break
            if match_start == "@ ":
                if repl_start in ["@ ", "@*"]:
                    section += 1
            elif match_start == "@*":
                if repl_start == "@*":
                    part += 1
                    section += 1
                elif repl_start == "@ ":
                    section += 1

            # Remove leading @x.
            text = self._lines[self._chunk_start][2:].strip()

            # Remove potentially leading [part.section] tag.
            pattern = "\\[\\d+(\\.\\d+)?\\]"
            if re.match(pattern, text):
                text = re.sub(pattern, "", text, 1).strip()

                # Remove potentially line number information.
                pattern = "l\\.\\d+"
                if re.match(pattern, text):
                    text = re.sub(pattern, "", text, 1)

                    # Remove potentially text comment separator.
                    pattern = " -+ "
                    if re.match(pattern, text):
                        text = re.sub(pattern, "", text, 1).strip()

            # Create line with standard tag and optional information.
            new_line = "@x"
            if opt_handler.part_b or opt_handler.section_b:
                new_line += " ["
                if opt_handler.part_b:
                    new_line += f"{part}"
                if opt_handler.part_b and opt_handler.section_b:
                    new_line += "."
                if opt_handler.section_b:
                    new_line += f"{section}"
                new_line += "]"
            if opt_handler.line_b:
                new_line += f" l.{line_number}"

            if opt_handler.text_b and text:
                if opt_handler.hyphen_b:
                    new_line += " -"
                new_line += f" {text}"

            ch_line = self._lines[self._chunk_start]
            if new_line[:10] != ch_line[:10]:
                print("Old:")
                print(f"  {ch_line}")
                print("New:")
                print(f"  {new_line}")
                print()

            self._lines[self._chunk_start] = new_line

    def get_lines(self):
        return self._lines


class OptHandler:
    """Parses the invocation line and extracts the options.
    Returns the remaining arguments, i.e., the WEB and Change File names.
    """

    def __init__(self):
        """Optional elements of the output format
        '@x [{part}.{section}] l.{line} {-(hyphen)} {text}'
        """
        self.part_b = True
        self.section_b = True
        self.line_b = True
        self.hyphen_b = True
        self.text_b = True

        try:
            opts, self.args = getopt.getopt(sys.argv[1:], "pslht",
                ["parts", "sections", "lines", "hyphens", "texts"])
        except getopt.GetoptError as err:
            print(USAGE)
            sys.exit(1)

        for opt, _ in opts:
            if opt in ("-p", "--parts"):
                self.part_b = False
            elif opt in ("-s", "--sections"):
                self.section_b = False
            elif opt in ("-l", "--lines"):
                self.line_b = False
            elif opt in ("-h", "--hyphens"):
                self.hyphen_b = False
            elif opt in ("-t", "--texts"):
                self.text_b = False
            else:
                assert False, f"Unhandled option {opt}"

        if len(self.args) != 2:
            print(USAGE)
            sys.exit(1)


def eprint(*args, **kwargs):
    """Print to stderr."""
    print(*args, file=sys.stderr, **kwargs)


if __name__ == "__main__":
    main()
