#!/usr/bin/env python3
"""Check that quick Catch test cases disable reporting with ReportGuard.

The script should be run from the libsemigroups root directory. By default it
checks libsemigroups test and benchmark .cpp files, excluding third_party.
Pass file or directory paths to restrict the check.
"""

import argparse
import re
import sys
from pathlib import Path


TEST_CASE_MACROS = (
    "LIBSEMIGROUPS_TEMPLATE_TEST_CASE",
    "LIBSEMIGROUPS_TEST_CASE",
    "TEMPLATE_TEST_CASE",
    "TEST_CASE",
)

SOURCE_SUFFIXES = {".cc", ".cpp", ".cxx"}

FALSE_REPORT_GUARD_REGEX = re.compile(
    r"""
    \bReportGuard\b
    \s*
    (?:
        <[^;\n{}()]*>\s*
    )?
    (?:
        [A-Za-z_]\w*\s*
    )?
    (?:
        \(\s*false\s*\)
      | \{\s*false\s*\}
    )
    """,
    re.VERBOSE,
)

TEST_CASE_REGEX = re.compile(
    r"\b(" + "|".join(re.escape(x) for x in TEST_CASE_MACROS) + r")\s*\("
)


def is_digit_separator(text, pos):
    """Return True if text[pos] is a C++ digit separator, not a quote."""
    if text[pos] != "'" or pos == 0 or pos + 1 >= len(text):
        return False
    return text[pos - 1].isalnum() and text[pos + 1].isalnum()


def strip_comments(text):
    """Return text with C++ comments replaced by spaces, preserving strings."""
    out = []
    i = 0
    state = "code"
    quote = ""
    raw_delim = ""

    while i < len(text):
        ch = text[i]
        nxt = text[i + 1] if i + 1 < len(text) else ""

        if state == "code":
            raw_match = re.match(r'R"([^\s()\\]{0,16})\(', text[i:])
            if raw_match:
                raw_delim = raw_match.group(1)
                marker = ")" + raw_delim + '"'
                end = text.find(marker, i + raw_match.end())
                if end == -1:
                    out.append(text[i:])
                    break
                end += len(marker)
                out.append(text[i:end])
                i = end
                continue
            if ch == "'" and is_digit_separator(text, i):
                out.append(ch)
                i += 1
            elif ch in ('"', "'"):
                state = "string"
                quote = ch
                out.append(ch)
                i += 1
            elif ch == "/" and nxt == "/":
                out.extend((" ", " "))
                i += 2
                while i < len(text) and text[i] != "\n":
                    out.append(" ")
                    i += 1
            elif ch == "/" and nxt == "*":
                out.extend((" ", " "))
                i += 2
                while i < len(text):
                    if text[i] == "*" and i + 1 < len(text) and text[i + 1] == "/":
                        out.extend((" ", " "))
                        i += 2
                        break
                    out.append("\n" if text[i] == "\n" else " ")
                    i += 1
            else:
                out.append(ch)
                i += 1
        elif state == "string":
            out.append(ch)
            i += 1
            if ch == "\\" and i < len(text):
                out.append(text[i])
                i += 1
            elif ch == quote:
                state = "code"

    return "".join(out)


def line_number(text, pos):
    return text.count("\n", 0, pos) + 1


def skip_string(text, pos):
    raw_match = re.match(r'R"([^\s()\\]{0,16})\(', text[pos:])
    if raw_match:
        marker = ")" + raw_match.group(1) + '"'
        end = text.find(marker, pos + raw_match.end())
        return len(text) if end == -1 else end + len(marker)

    quote = text[pos]
    pos += 1
    while pos < len(text):
        if text[pos] == "\\":
            pos += 2
        elif text[pos] == quote:
            return pos + 1
        else:
            pos += 1
    return pos


def find_matching(text, open_pos, open_ch, close_ch):
    depth = 0
    pos = open_pos
    while pos < len(text):
        ch = text[pos]
        if ch == "'" and is_digit_separator(text, pos):
            pos += 1
            continue
        if ch in ('"', "'") or text.startswith('R"', pos):
            pos = skip_string(text, pos)
            continue
        if ch == open_ch:
            depth += 1
        elif ch == close_ch:
            depth -= 1
            if depth == 0:
                return pos
        pos += 1
    return None


def next_nonspace(text, pos):
    while pos < len(text) and text[pos].isspace():
        pos += 1
    return pos


def find_quick_test_cases(filename, text):
    clean = strip_comments(text)
    start = 0
    while True:
        match = TEST_CASE_REGEX.search(clean, start)
        if match is None:
            break

        macro = match.group(1)
        args_open = clean.find("(", match.start())
        args_close = find_matching(clean, args_open, "(", ")")
        if args_close is None:
            print(
                f"{filename}:{line_number(clean, match.start())}: "
                f"warning: could not parse {macro} invocation",
                file=sys.stderr,
            )
            start = match.end()
            continue

        body_open = next_nonspace(clean, args_close + 1)
        if body_open >= len(clean) or clean[body_open] != "{":
            start = args_close + 1
            continue

        body_close = find_matching(clean, body_open, "{", "}")
        if body_close is None:
            print(
                f"{filename}:{line_number(clean, body_open)}: "
                f"warning: could not parse {macro} body",
                file=sys.stderr,
            )
            start = body_open + 1
            continue

        args = clean[args_open + 1 : args_close]
        if "[quick]" in args:
            yield {
                "macro": macro,
                "line": line_number(clean, match.start()),
                "args": args,
                "body": clean[body_open + 1 : body_close],
            }

        start = body_close + 1


def has_false_report_guard(body):
    return FALSE_REPORT_GUARD_REGEX.search(body) is not None


def default_files():
    roots = [Path("tests"), Path("benchmarks")]
    for root in roots:
        if not root.exists():
            continue
        for filename in root.rglob("*"):
            if (
                filename.is_file()
                and filename.suffix in SOURCE_SUFFIXES
                and "third_party" not in filename.parts
            ):
                yield filename


def files_from_args(paths):
    for path in paths:
        filename = Path(path)
        if filename.is_dir():
            for child in filename.rglob("*"):
                if child.is_file() and child.suffix in SOURCE_SUFFIXES:
                    yield child
        elif filename.is_file():
            yield filename


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "paths",
        nargs="*",
        help="files or directories to check (default: tests and benchmarks)",
    )
    args = parser.parse_args()

    files = sorted(set(files_from_args(args.paths) if args.paths else default_files()))
    failures = []
    quick_count = 0

    for filename in files:
        text = filename.read_text(encoding="utf-8")
        for test_case in find_quick_test_cases(filename, text):
            quick_count += 1
            if not has_false_report_guard(test_case["body"]):
                failures.append((filename, test_case))

    for filename, test_case in failures:
        print(
            f"{filename}:{test_case['line']}: "
            f"{test_case['macro']} with [quick] does not contain "
            "ReportGuard(false)"
        )

    if failures:
        print(
            f"Checked {quick_count} [quick] test cases; "
            f"{len(failures)} missing ReportGuard(false).",
            file=sys.stderr,
        )
        return 1

    print(f"Checked {quick_count} [quick] test cases; all disable reporting.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
