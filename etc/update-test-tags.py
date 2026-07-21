#!/usr/bin/env python3
"""Run Catch2 tests individually and update their runtime category tags.

Each matching active LIBSEMIGROUPS_TEST_CASE or
LIBSEMIGROUPS_TEMPLATE_TEST_CASE in the source file is selected in the
executable using its LIBSEMIGROUPS_TEST_PREFIX tag.  Successful tests are
classified by elapsed wall-clock time; tests that time out or return a
non-zero status are tagged ``fail``.
"""

from __future__ import annotations

import argparse
import ast
from dataclasses import dataclass
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import time


CATEGORY_RE = re.compile(r"\[(quick|standard|extreme|fail)\]")
MACRO_RE = re.compile(r"\b(LIBSEMIGROUPS_(?:TEMPLATE_)?TEST_CASE)\s*\(")


@dataclass(frozen=True)
class TestCase:
    family: str
    number: str
    tag_start: int
    tag_end: int
    tags: str

    @property
    def selector(self) -> str:
        return f"[LIBSEMIGROUPS_TEST_PREFIX={self.family} {self.number}]"


@dataclass(frozen=True)
class Result:
    test: TestCase
    category: str
    elapsed: float
    detail: str = ""


def is_digit_separator(text: str, pos: int) -> bool:
    return (
        text[pos] == "'"
        and pos > 0
        and pos + 1 < len(text)
        and text[pos - 1].isalnum()
        and text[pos + 1].isalnum()
    )


def skip_quoted(text: str, pos: int) -> int:
    quote = text[pos]
    pos += 1
    while pos < len(text):
        if text[pos] == "\\":
            pos += 2
        elif text[pos] == quote:
            return pos + 1
        else:
            pos += 1
    raise ValueError("unterminated quoted string")


def mask_comments(text: str) -> str:
    """Replace comments with spaces while retaining positions and newlines."""
    out = list(text)
    pos = 0
    state = "code"
    while pos < len(text):
        if state == "code":
            if text.startswith("//", pos):
                out[pos : pos + 2] = "  "
                pos += 2
                while pos < len(text) and text[pos] != "\n":
                    out[pos] = " "
                    pos += 1
            elif text.startswith("/*", pos):
                out[pos : pos + 2] = "  "
                pos += 2
                state = "block-comment"
            elif text[pos] == "'" and is_digit_separator(text, pos):
                pos += 1
            elif text[pos] in "\"'":
                pos = skip_quoted(text, pos)
            else:
                pos += 1
        else:
            if text.startswith("*/", pos):
                out[pos : pos + 2] = "  "
                pos += 2
                state = "code"
            else:
                if text[pos] != "\n":
                    out[pos] = " "
                pos += 1
    if state == "block-comment":
        raise ValueError("unterminated block comment")
    return "".join(out)


def matching_parenthesis(text: str, opening: int) -> int:
    depth = 0
    pos = opening
    while pos < len(text):
        if text.startswith("//", pos):
            newline = text.find("\n", pos + 2)
            pos = len(text) if newline == -1 else newline + 1
        elif text.startswith("/*", pos):
            closing = text.find("*/", pos + 2)
            if closing == -1:
                raise ValueError("unterminated block comment")
            pos = closing + 2
        elif text[pos] == "'" and is_digit_separator(text, pos):
            pos += 1
        elif text[pos] in "\"'":
            pos = skip_quoted(text, pos)
        elif text[pos] == "(":
            depth += 1
            pos += 1
        elif text[pos] == ")":
            depth -= 1
            if depth == 0:
                return pos
            pos += 1
        else:
            pos += 1
    raise ValueError("unterminated test-case macro")


def string_literals(text: str, start: int, end: int) -> list[tuple[int, int, str]]:
    result = []
    pos = start
    while pos < end:
        if text.startswith("//", pos):
            newline = text.find("\n", pos + 2, end)
            pos = end if newline == -1 else newline + 1
        elif text.startswith("/*", pos):
            closing = text.find("*/", pos + 2, end)
            if closing == -1:
                raise ValueError("unterminated block comment in test-case macro")
            pos = closing + 2
        elif text[pos] == '"':
            closing = skip_quoted(text, pos)
            value = ast.literal_eval(text[pos:closing])
            result.append((pos + 1, closing - 1, value))
            pos = closing
        elif text[pos] == "'" and is_digit_separator(text, pos):
            pos += 1
        elif text[pos] == "'":
            pos = skip_quoted(text, pos)
        else:
            pos += 1
    return result


def find_test_cases(source: Path, text: str) -> list[TestCase]:
    clean = mask_comments(text)
    result = []
    for match in MACRO_RE.finditer(clean):
        opening = clean.find("(", match.start())
        closing = matching_parenthesis(text, opening)
        strings = string_literals(text, opening, closing)
        if len(strings) < 4:
            line = text.count("\n", 0, match.start()) + 1
            raise ValueError(
                f"{source}:{line}: expected at least four string arguments"
            )
        family = strings[0][2]
        number = strings[1][2]
        tag_start, tag_end, tags = strings[3]
        categories = CATEGORY_RE.findall(tags)
        if len(categories) > 1:
            line = text.count("\n", 0, tag_start) + 1
            raise ValueError(f"{source}:{line}: multiple runtime category tags")
        result.append(TestCase(family, number, tag_start, tag_end, tags))
    if not result:
        raise ValueError(f"{source}: no active libsemigroups test cases found")
    return result


def classify(elapsed: float, returncode: int | None, args: argparse.Namespace) -> str:
    if returncode is None or returncode != 0:
        return "fail"
    if elapsed < args.quick_seconds:
        return "quick"
    if elapsed < args.standard_seconds:
        return "standard"
    return "extreme"


def run_test(executable: Path, test: TestCase, args: argparse.Namespace) -> Result:
    command = [str(executable), test.selector, "--reporter", "compact"]
    started = time.monotonic()
    try:
        with tempfile.TemporaryFile(mode="w+t", encoding="utf-8") as output_file:
            completed = subprocess.run(
                command,
                stdout=output_file,
                stderr=subprocess.STDOUT,
                text=True,
                errors="replace",
                timeout=args.timeout,
                check=False,
            )
            output_file.seek(0)
            output = output_file.read(1_000_000)
        elapsed = time.monotonic() - started
        if "No test cases matched" in output or "No tests ran" in output:
            raise RuntimeError(
                f"the executable contains no test matching {test.selector}"
            )
        category = classify(elapsed, completed.returncode, args)
        detail = "" if completed.returncode == 0 else f"exit {completed.returncode}"
        return Result(test, category, elapsed, detail)
    except subprocess.TimeoutExpired:
        elapsed = time.monotonic() - started
        return Result(test, "fail", elapsed, "timeout")


def updated_tags(tags: str, category: str) -> str:
    match = CATEGORY_RE.search(tags)
    if match is None:
        return f"[{category}]" + tags
    return tags[: match.start()] + f"[{category}]" + tags[match.end() :]


def normalized_tags(arguments: list[str]) -> list[str]:
    result = []
    for argument in arguments:
        if argument.startswith("["):
            tags = re.findall(r"\[([^]]+)\]", argument)
            if "".join(f"[{tag}]" for tag in tags) != argument:
                raise ValueError(f"invalid tag expression {argument!r}")
        else:
            tags = [argument]
        for tag in tags:
            if not tag or any(char in tag for char in "[] ,"):
                raise ValueError(f"invalid tag {tag!r}; use a simple tag name")
            if tag not in result:
                result.append(tag)
    return result


def has_tags(test: TestCase, wanted: list[str]) -> bool:
    present = set(re.findall(r"\[([^]]+)\]", test.tags))
    return all(tag in present for tag in wanted)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("executable", type=Path, help="Catch2 test executable")
    parser.add_argument("source", type=Path, help="C++ source file to update")
    parser.add_argument(
        "tags",
        nargs="*",
        metavar="TAG",
        help=(
            "only run tests containing every given tag; accepts maf, [maf], "
            "or [maf][quick]"
        ),
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=30.0,
        help="per-test timeout in seconds (default: 30)",
    )
    parser.add_argument(
        "--quick-seconds",
        type=float,
        default=0.2,
        help="successful tests below this duration are quick (default: 0.2)",
    )
    parser.add_argument(
        "--standard-seconds",
        type=float,
        default=3.0,
        help="successful tests below this duration are standard (default: 3)",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="show classifications without modifying the source",
    )
    args = parser.parse_args()
    if args.timeout <= 0:
        parser.error("--timeout must be positive")
    if not 0 <= args.quick_seconds < args.standard_seconds <= args.timeout:
        parser.error("thresholds must satisfy 0 <= quick < standard <= timeout")
    return args


def main() -> int:
    args = parse_args()
    executable = args.executable.resolve()
    source = args.source.resolve()
    if not executable.is_file():
        raise ValueError(f"test executable does not exist: {executable}")
    if not source.is_file():
        raise ValueError(f"test source does not exist: {source}")

    text = source.read_text()
    tests = find_test_cases(source, text)
    wanted_tags = normalized_tags(args.tags)
    tests = [test for test in tests if has_tags(test, wanted_tags)]
    if not tests:
        requested = " ".join(f"[{tag}]" for tag in wanted_tags)
        raise ValueError(f"{source}: no active test cases match {requested}")
    results = []
    for index, test in enumerate(tests, 1):
        print(
            f"[{index:>3}/{len(tests)}] {test.family} {test.number}",
            end=" ",
            flush=True,
        )
        result = run_test(executable, test, args)
        results.append(result)
        suffix = f" ({result.detail})" if result.detail else ""
        print(f"{result.elapsed:7.3f}s -> {result.category}{suffix}")

    updated = text
    for result in reversed(results):
        test = result.test
        tags = updated_tags(test.tags, result.category)
        updated = updated[: test.tag_start] + tags + updated[test.tag_end :]

    if args.dry_run:
        print(f"Dry run: {source} was not modified")
    elif updated == text:
        print(f"No tag changes required in {source}")
    else:
        source.write_text(updated)
        print(f"Updated runtime category tags in {source}")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, RuntimeError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        sys.exit(2)
