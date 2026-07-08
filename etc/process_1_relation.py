#!/usr/bin/env python3
"""Run and tag generated 1-relation example tests.

The script runs selected test cases from ``examples_1_relation_000_049`` one at
a time, classifies their runtime according to ``CONTRIBUTING.rst``, and updates
the corresponding ``LIBSEMIGROUPS_TEST_CASE`` tags in the generated source.
Failing tests, including tests killed after the timeout, also receive
``[fail]``.  Passing extreme tests also get a generated comment recording how
long they took to run.
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path


DEFAULT_EXE = Path("./examples_1_relation")
DEFAULT_TIMEOUT_SECONDS = 10.0

RUNTIME_TAGS = {"quick", "standard", "extreme"}
REMOVED_TAGS = RUNTIME_TAGS | {"fail"}
TEST_CASE_RE = re.compile(
    r"""(?P<comment>[ \t]*//\ This\ extreme\ test\ passed\ in\ \d+(?:\.\d+)?s\.\n)?
        (?P<indent>[ \t]*)
        (?P<prefix>
        LIBSEMIGROUPS_TEST_CASE\(
        \s*"1-relation"\s*,\s*
        "(?P<number>\d{3})"\s*,\s*
        "(?:[^"\\]|\\.)*"\s*,\s*
        ")
        (?P<tags>[^"]*)
        (?P<suffix>"\s*\))
    """,
    re.VERBOSE | re.DOTALL,
)
TAG_RE = re.compile(r"\[([^\]]+)\]")


@dataclass(frozen=True)
class TestResult:
    """Result of running one generated 1-relation test case."""

    number: int
    status: str
    category: str
    elapsed: float
    timed_out: bool
    returncode: int | None


def parse_args() -> argparse.Namespace:
    """Parse command line options."""

    parser = argparse.ArgumentParser(
        description="Run and retag generated 1-relation example tests.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "range",
        help=(
            "test range to process, for example 000-049, 0-49, 003, "
            "or a comma-separated mixture such as 000-004,010"
        ),
    )
    parser.add_argument(
        "--exe",
        type=Path,
        default=DEFAULT_EXE,
        help="test executable to run",
    )
    parser.add_argument(
        "--source",
        type=Path,
        help="source file whose test tags should be updated",
        required=True,
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=DEFAULT_TIMEOUT_SECONDS,
        help="seconds before a test subprocess is killed and marked as fail",
    )
    parser.add_argument(
        "--no-edit",
        action="store_true",
        help="run tests and print the summary without modifying the source file",
    )
    return parser.parse_args()


def parse_test_range(spec: str) -> list[int]:
    """Return sorted test numbers parsed from a range expression."""

    result: set[int] = set()
    for part in spec.split(","):
        part = part.strip()
        if not part:
            continue
        if "-" in part:
            start_text, end_text = part.split("-", maxsplit=1)
            start = int(start_text)
            end = int(end_text)
            if start > end:
                raise ValueError(f"invalid descending range: {part}")
            result.update(range(start, end + 1))
        else:
            result.add(int(part))

    if not result:
        raise ValueError("empty test range")
    invalid = [number for number in result if number < 0 or number > 999]
    if invalid:
        raise ValueError(f"test numbers must be in 000..999: {invalid}")
    return sorted(result)


def runtime_category(elapsed: float) -> str:
    """Return the CONTRIBUTING.rst runtime category for ``elapsed`` seconds."""

    if elapsed < 0.2:
        return "quick"
    if elapsed < 3:
        return "standard"
    return "extreme"


def executable_name(exe: Path) -> str:
    """Return a subprocess-safe executable path.

    ``subprocess`` searches ``PATH`` when the executable contains no slash, but
    build-tree executables are normally run from the current directory.  Add the
    explicit ``./`` for that common case.
    """

    text = str(exe)
    if exe.is_absolute() or "/" in text:
        return text
    return f"./{text}"


def run_test(exe: Path, number: int, timeout: float) -> TestResult:
    """Run one test case and return its status, runtime, and category."""

    tag = f"[LIBSEMIGROUPS_TEST_NUM={number:03d}]"
    start = time.monotonic()
    try:
        proc = subprocess.run(
            [executable_name(exe), tag, "--durations", "yes"],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            timeout=timeout,
        )
        elapsed = time.monotonic() - start
        status = "pass" if proc.returncode == 0 else "fail"
        return TestResult(
            number=number,
            status=status,
            category=runtime_category(elapsed),
            elapsed=elapsed,
            timed_out=False,
            returncode=proc.returncode,
        )
    except subprocess.TimeoutExpired:
        elapsed = time.monotonic() - start
        return TestResult(
            number=number,
            status="fail",
            category=runtime_category(elapsed),
            elapsed=elapsed,
            timed_out=True,
            returncode=None,
        )


def split_tags(tags: str) -> list[str]:
    """Extract bracketed Catch tags from a tag string."""

    return [match.group(1) for match in TAG_RE.finditer(tags)]


def format_tags(existing: str, result: TestResult) -> str:
    """Return a normalized tag string for ``result`` preserving unrelated tags."""

    kept = [tag for tag in split_tags(existing) if tag.lower() not in REMOVED_TAGS]
    kept.append(result.category)
    if result.status == "fail":
        kept.append("fail")
    return "".join(f"[{tag}]" for tag in kept)


def format_extreme_comment(indent: str, result: TestResult) -> str:
    """Return the generated runtime comment for a passing extreme test."""

    if result.status == "pass" and result.category == "extreme":
        return f"{indent}// This extreme test passed in {result.elapsed:.3f}s.\n"
    return ""


def update_source_tags(source: Path, results: dict[int, TestResult]) -> int:
    """Update matching test-case tags/comments and return edit count."""

    content = source.read_text()
    seen: set[int] = set()

    def replace(match: re.Match[str]) -> str:
        number = int(match.group("number"))
        if number not in results:
            return match.group(0)
        seen.add(number)
        indent = match.group("indent")
        result = results[number]
        return (
            format_extreme_comment(indent, result)
            + indent
            + match.group("prefix")
            + format_tags(match.group("tags"), result)
            + match.group("suffix")
        )

    updated = TEST_CASE_RE.sub(replace, content)
    missing = sorted(set(results) - seen)
    if missing:
        raise RuntimeError(
            "could not find test cases in source: "
            + ", ".join(f"{number:03d}" for number in missing)
        )
    if updated != content:
        source.write_text(updated)
    return len(seen)


def print_summary(results: list[TestResult], edited: int | None) -> None:
    """Print a compact summary after all tests have run."""

    status_counts = {"pass": 0, "fail": 0}
    category_counts = {"quick": 0, "standard": 0, "extreme": 0}
    timed_out = []

    for result in results:
        status_counts[result.status] += 1
        category_counts[result.category] += 1
        if result.timed_out:
            timed_out.append(result.number)

    print()
    print("Summary")
    print("=======")
    print(f"tests:    {len(results)}")
    print(f"pass:     {status_counts['pass']}")
    print(f"fail:     {status_counts['fail']}")
    print(f"quick:    {category_counts['quick']}")
    print(f"standard: {category_counts['standard']}")
    print(f"extreme:  {category_counts['extreme']}")
    if timed_out:
        print("timeout:  " + ", ".join(f"{number:03d}" for number in timed_out))
    if edited is not None:
        print(f"retagged: {edited}")


def main() -> int:
    """Run requested tests, update their tags, and print a summary."""

    args = parse_args()
    if args.timeout <= 0:
        raise ValueError("--timeout must be positive")
    numbers = parse_test_range(args.range)

    results = []
    for number in numbers:
        result = run_test(args.exe, number, args.timeout)
        results.append(result)
        suffix = " timeout" if result.timed_out else ""
        print(
            f"{number:03d},{result.status},{result.category},"
            f"{result.elapsed:.3f}{suffix}",
            flush=True,
        )

    edited = None
    if not args.no_edit:
        edited = update_source_tags(
            args.source, {result.number: result for result in results}
        )
    print_summary(results, edited)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, ValueError) as e:
        print(f"error: {e}", file=sys.stderr)
        raise SystemExit(1)
