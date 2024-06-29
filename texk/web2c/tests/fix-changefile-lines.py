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
Usage: {os.path.basename(sys.argv[0])} [-i|--init] <web file> <change file>

Reads a WEB file and a change file and writes a change file to stdout with
potentially corrected part, section and line numbers.

The option '-i' ('--init') forces a '[part.section] l.line' tag after each
'@x'; this is useful for untagged change files.

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
            with open(web_file, "r") as file:
                self._web_lines = [line.rstrip() for line in file]
        except OSError:
            eprint(f"Could not open {web_file}")
            print(USAGE)
            sys.exit(1)

    def next_line(self):
        """Returns the triple of current part, section and line numbers, as
        well as the next line. Updates part and section numbers.
        """
        if self._pos >= len(self._web_lines):
            return None
        line = self._web_lines[self._pos]
        self._pos += 1

        # Look for starred section == part
        if line.startswith("@*"):
            self.part_cnt += 1
            self.section_cnt += 1

        # Look for unstarred section
        if line.startswith("@ ") or line == "@":
            self.section_cnt += 1

        # Prepare return values
        part = self.part_cnt
        section = self.section_cnt
        line_number = self._pos

        # Look for '@i'nclude line
        result = re.match("^@i \"?(\\w+(\\.\\w+)?)\"?", line)
        if result:
            inc_reader = WebReader(result[1])
            while inc_reader.next_line():
                pass
            self.part_cnt += inc_reader.part_cnt
            self.section_cnt += inc_reader.section_cnt
            # Do not increase 'part' and 'section' just yet
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
            with open(change_file, "r") as file:
                self._lines = [line.rstrip() for line in file]
        except OSError:
            eprint(f"Could not open {change_file}")
            print(USAGE)
            sys.exit(1)

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
                        eprint(f"! Change file ended before @y. (l. {self._pos+1} of change file)")
                        sys.exit(1)
                    line = self._lines[self._pos]
                    if line.startswith("@y"):
                        self._match_lines = self._lines[
                            self._chunk_start + 1 : self._pos
                        ]
                        return True
                    elif line.startswith("@x") or line.startswith("@z"):
                        eprint(f"! Where is the matching @y?. (l. {self._pos+1} of change file)")
                        eprint(line)
                        sys.exit(1)
            self._pos += 1
        return False

    def find_match_in_web(self, web_reader):
        """Find the match for the current change chunk in the WEB file.
        Returns the part, section, and line number of the first match line in
        the WEB file.
        """
        while True:
            try:
                (part, section, line_number), tex_line = web_reader.next_line()
            except:
                eprint(f"! Change file entry did not match. (l. {self._chunk_start+2} of change file)")
                eprint(self._match_lines[0])
                sys.exit(1)
            if tex_line == self._match_lines[0]:
                for i in range(1, len(self._match_lines)):
                    try:
                        _, tex_line = web_reader.next_line()
                    except:
                        tex_line = None
                    if tex_line is None or tex_line != self._match_lines[i]:
                        eprint(f"! Change file entry did not match. (l. {self._chunk_start+2+i} of change file)")
                        eprint(self._match_lines[i])
                        sys.exit(1)

                return part, section, line_number

    def traverse(self, web_reader, opt_handler):
        """Go through all individual change chunks while updating their tags."""
        while self.advance_to_next_chunk():
            part, section, line_number = self.find_match_in_web(web_reader)

            # Replace '@x' line with updated information.
            new_line = self._lines[self._chunk_start]

            new_line = re.sub(
                    "\\[\\d+\\.\\d+\\]", f"[{part}.{section}]", new_line, 1)
            new_line = re.sub(
                    "^@x \\[\\d+\\]", f"@x [{section}]", new_line, 1)
            new_line = re.sub(
                    "l\\.\\d+", f"l.{line_number}", new_line, 1)

            # Force '[part.section] l.line' tag after '@x'; useful for untagged
            # change files, e.g., CWEB's '*-w2c.ch' monsters.
            if opt_handler.init_b:
                new_line = re.sub(
                        "^@x", f"@x [{part}.{section}] l.{line_number}",
                        new_line, 1)

            ch_line = self._lines[self._chunk_start]
            if new_line[:10] != ch_line[:10]:
                eprint("Old:")
                eprint(f"  {ch_line}")
                eprint("New:")
                eprint(f"  {new_line}")
                eprint()

            self._lines[self._chunk_start] = new_line

    def get_lines(self):
        return self._lines


class OptHandler:
    """Parses the invocation line and extracts the options.
    Returns the remaining arguments, i.e., the WEB and Change File names.
    """

    def __init__(self):
        # Should we insert tag '[{part}.{section}] l.{line}' after '@x'?
        self.init_b = False

        try:
            opts, self.args = getopt.getopt(sys.argv[1:], "i", ["init"])
        except getopt.GetoptError as err:
            eprint(f"\n{os.path.basename(sys.argv[0])}: {err}!\n")
            print(USAGE)
            sys.exit(1)

        for opt, _ in opts:
            if opt in ("-i", "--init"):
                self.init_b = True
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
