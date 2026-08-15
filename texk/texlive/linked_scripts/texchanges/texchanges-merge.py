#!/usr/bin/env python3
"""Resolve or merge Texchanges markup without third-party dependencies."""

from __future__ import annotations

import argparse
import difflib
import re
import shutil
import sys
from dataclasses import dataclass
from pathlib import Path

VERSION = "0.2.3"
VERBATIM_ENVS = {"verbatim", "verbatim*", "lstlisting", "minted"}
NATIVE = {
    "txadd": (0, 1),
    "txremove": (1, 0),
    "txreplace": (1, 1),
    "txhighlight": (0, 1),
    "txcomment": (0, 0),
    "add": (0, 1),
    "remove": (1, 0),
    "replace": (1, 1),
}
COMPAT = {
    "added": (0, 1),
    "deleted": (1, 0),
    "replaced": (1, 1),  # arguments are new, old
    "chadded": (0, 1),
    "chdeleted": (1, 0),
    "chreplaced": (1, 1),
}
HIGHLIGHTS = {"txhighlight", "highlight", "chhighlight"}
COMMENTS = {"txcomment", "comment", "chcomment"}


class ParseError(ValueError):
    pass


@dataclass(frozen=True)
class Change:
    command: str
    start: int
    end: int
    options: str | None
    args: tuple[str, ...]
    old: str
    new: str
    author: str | None
    change_id: str | None
    status: str


def _skip_space(text: str, pos: int) -> int:
    while pos < len(text) and text[pos].isspace():
        pos += 1
    return pos


def _group(text: str, pos: int, opening: str, closing: str) -> tuple[str, int]:
    pos = _skip_space(text, pos)
    if pos >= len(text) or text[pos] != opening:
        raise ParseError(f"expected {opening!r} at offset {pos}")
    depth, i = 1, pos + 1
    while i < len(text):
        if text[i] == "\\":
            i += 2
            continue
        if text[i] == opening:
            depth += 1
        elif text[i] == closing:
            depth -= 1
            if depth == 0:
                return text[pos + 1 : i], i + 1
        i += 1
    raise ParseError(f"unclosed {opening!r} group at offset {pos}")


def _split_keys(options: str | None) -> dict[str, str]:
    if not options or "=" not in options:
        return {}
    result: dict[str, str] = {}
    token, depth = [], 0
    parts: list[str] = []
    for char in options + ",":
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth < 0:
                raise ParseError("unbalanced braces in optional arguments")
        if char == "," and depth == 0:
            parts.append("".join(token).strip())
            token = []
        else:
            token.append(char)
    if depth:
        raise ParseError("unbalanced braces in optional arguments")
    for part in parts:
        if not part:
            continue
        if "=" not in part:
            raise ParseError(f"invalid key-value option: {part!r}")
        key, value = part.split("=", 1)
        value = value.strip()
        if value.startswith("{") and value.endswith("}"):
            value = value[1:-1]
        result[key.strip()] = value
    return result


def _is_comment(text: str, pos: int) -> bool:
    backslashes = 0
    j = pos - 1
    while j >= 0 and text[j] == "\\":
        backslashes += 1
        j -= 1
    return backslashes % 2 == 0


def _verbatim_ranges(text: str) -> list[tuple[int, int]]:
    ranges: list[tuple[int, int]] = []
    for env in VERBATIM_ENVS:
        pattern = re.compile(r"\\begin\s*\{" + re.escape(env) + r"\}")
        end_token = rf"\end{{{env}}}"
        for match in pattern.finditer(text):
            end = text.find(end_token, match.end())
            if end < 0:
                raise ParseError(f"unclosed {env} environment")
            ranges.append((match.start(), end + len(end_token)))
    return sorted(ranges)


def parse_changes(text: str) -> list[Change]:
    ranges = _verbatim_ranges(text)
    range_index, line_comment, changes = 0, False, []
    i = 0
    while i < len(text):
        if range_index < len(ranges) and i >= ranges[range_index][1]:
            range_index += 1
        if range_index < len(ranges) and ranges[range_index][0] <= i < ranges[range_index][1]:
            i = ranges[range_index][1]
            continue
        if text[i] == "\n":
            line_comment = False
            i += 1
            continue
        if text[i] == "%" and _is_comment(text, i):
            line_comment = True
        if line_comment or text[i] != "\\":
            i += 1
            continue
        match = re.match(r"\\([A-Za-z@]+)", text[i:])
        if not match:
            i += 1
            continue
        command = match.group(1)
        if command not in NATIVE and command not in COMPAT and command not in HIGHLIGHTS and command not in COMMENTS:
            i += len(match.group(0))
            continue
        pos, options = i + len(match.group(0)), None
        pos = _skip_space(text, pos)
        if pos < len(text) and text[pos] == "[":
            options, pos = _group(text, pos, "[", "]")
        arg_count = 2 if command.endswith("replaced") or command in {"txreplace", "replace"} else 1
        args: list[str] = []
        for _ in range(arg_count):
            arg, pos = _group(text, pos, "{", "}")
            args.append(arg)
        keys = _split_keys(options)
        compat = command in COMPAT or command in {"chhighlight", "chcomment"}
        if command in {"highlight", "comment"}:
            compat = "changeid" in keys or ("author" not in keys and "id" in keys)
        author = keys.get("id") if compat else keys.get("author")
        change_id = keys.get("changeid") if compat else keys.get("id")
        status = keys.get("status", "pending")
        if command in {"txadd", "add", "added", "chadded"}:
            old, new = "", args[0]
        elif command in {"txremove", "remove", "deleted", "chdeleted"}:
            old, new = args[0], ""
        elif command in {"replaced", "chreplaced"}:
            old, new = args[1], args[0]
        elif command in {"txreplace", "replace"}:
            old, new = args[0], args[1]
        elif command in HIGHLIGHTS:
            old = new = args[0]
        else:
            old = new = ""
        changes.append(Change(command, i, pos, options, tuple(args), old, new, author, change_id, status))
        i = pos
    return changes


def _selected(change: Change, author: str | None, change_id: str | None) -> bool:
    return (author is None or change.author == author) and (change_id is None or change.change_id == change_id)


def _option_string(change: Change, status: str) -> str:
    keys = _split_keys(change.options)
    keys["status"] = status
    body = ",".join(f"{key}={{{value}}}" for key, value in keys.items())
    return f"[{body}]"


def transform(
    text: str,
    *,
    decision: str,
    merge: bool,
    author: str | None = None,
    change_id: str | None = None,
    interactive: bool = False,
) -> str:
    changes = parse_changes(text)
    output, cursor = [], 0
    for change in changes:
        output.append(text[cursor : change.start])
        cursor = change.end
        if not _selected(change, author, change_id):
            output.append(text[change.start : change.end])
            continue
        chosen = decision
        if interactive:
            answer = input(f"{change.command} {change.change_id or '-'} [a]ccept/[r]eject/[s]kip: ").strip().lower()
            if answer.startswith("s") or not answer:
                output.append(text[change.start : change.end])
                continue
            chosen = "accept" if answer.startswith("a") else "reject"
        if merge:
            if change.command in HIGHLIGHTS:
                output.append(change.new)
                continue
            if change.command in COMMENTS:
                continue
            output.append(change.new if chosen == "accept" else change.old)
        else:
            status = "accepted" if chosen == "accept" else "rejected"
            command_end = change.start + len(change.command) + 1
            suffix = text[command_end : change.end]
            if change.options is not None:
                option_start = suffix.find("[")
                _, option_end = _group(suffix, option_start, "[", "]")
                suffix = suffix[:option_start] + _option_string(change, status) + suffix[option_end:]
            else:
                suffix = _option_string(change, status) + suffix
            output.append("\\" + change.command + suffix)
    output.append(text[cursor:])
    return "".join(output)


def _help_formatter(prog: str) -> argparse.HelpFormatter:
    width = max(80, shutil.get_terminal_size(fallback=(80, 24)).columns)
    return argparse.HelpFormatter(prog, width=width, max_help_position=30)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="texchanges-merge",
        description=__doc__,
        formatter_class=_help_formatter,
    )
    parser.add_argument("--version", action="version", version=f"%(prog)s {VERSION}")
    parser.add_argument("input", type=Path, help="LaTeX source file containing Texchanges markup")
    parser.add_argument("output", type=Path, nargs="?", help="destination file for the updated source")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--accept", action="store_true", help="accept matching changes")
    group.add_argument("--reject", action="store_true", help="reject matching changes")
    group.add_argument("--interactive", action="store_true", help="prompt for each matching change")
    parser.add_argument("--merge", action="store_true", help="remove markup instead of updating status")
    parser.add_argument("--author", help="process only changes by this author ID")
    parser.add_argument("--id", dest="change_id", help="process only the specified change ID")
    parser.add_argument("--dry-run", action="store_true", help="print a unified diff without writing files")
    parser.add_argument("--in-place", action="store_true", help="update the input file and create a backup")
    parser.add_argument(
        "--backup-suffix",
        default=".bak",
        help="suffix for in-place backups (default: .bak)",
    )
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if args.in_place and args.output:
        raise SystemExit("output must be omitted with --in-place")
    if not args.in_place and not args.output and not args.dry_run:
        raise SystemExit("provide a distinct output file or use --in-place")
    if args.output and args.output.resolve() == args.input.resolve():
        raise SystemExit("output must differ from input unless --in-place is selected")
    original = args.input.read_text(encoding="utf-8")
    decision = "accept" if args.accept else "reject"
    try:
        updated = transform(
            original,
            decision=decision,
            merge=args.merge,
            author=args.author,
            change_id=args.change_id,
            interactive=args.interactive,
        )
    except ParseError as error:
        print(f"texchanges-merge: {error}", file=sys.stderr)
        return 2
    if args.dry_run:
        sys.stdout.writelines(
            difflib.unified_diff(original.splitlines(True), updated.splitlines(True), fromfile=str(args.input), tofile=str(args.output or args.input))
        )
        return 0
    destination = args.input if args.in_place else args.output
    assert destination is not None
    if args.in_place:
        shutil.copy2(args.input, Path(str(args.input) + args.backup_suffix))
    destination.write_text(updated, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
