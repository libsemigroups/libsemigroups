#!/usr/bin/env python3
"""Query documentation in a libsemigroups checkout using only Python's stdlib."""

import argparse
from collections import Counter
import json
import math
from pathlib import Path
import re
import sys
import textwrap


SCHEMA_VERSION = 1
DOC_LINE = re.compile(r"^\s*//! ?(.*)$")
HEADING = re.compile(r"^(#{1,6})\s+(.+?)\s*#*\s*$")
FENCE = re.compile(r"^\s*(`{3,}|~{3,})")


def tokens(text):
    """Treat CamelCase, snake_case, and hyphenated names equivalently."""
    text = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1 \2", text)
    text = re.sub(r"([a-z0-9])([A-Z])", r"\1 \2", text)
    return re.findall(r"[^\W_]+", text.casefold(), flags=re.UNICODE)


def examples(documentation, kind):
    if kind == "api":
        pattern = (
            r"\\(code_no_test|code)(?:\{[^}]*\})?\s*\n(.*?)\\end_?code(?:_no_test)?"
        )
        return [
            {"kind": match.group(1), "text": match.group(2).strip("\n")}
            for match in re.finditer(pattern, documentation, flags=re.DOTALL)
        ]
    result = []
    fence = None
    lines = []
    for line in documentation.splitlines():
        match = FENCE.match(line)
        if fence is None and match:
            fence = match.group(1)
            lines = []
        elif fence and is_closing_fence(line, fence):
            result.append({"kind": "fenced", "text": "\n".join(lines)})
            fence = None
        elif fence:
            lines.append(line)
    return result


def is_closing_fence(line, fence):
    return (
        re.fullmatch(
            r"\s*" + re.escape(fence[0]) + "{" + str(len(fence)) + r",}\s*", line
        )
        is not None
    )


def entry(path, start, end, kind, title, documentation, context=""):
    return {
        "id": f"{path}:{start}",
        "path": path,
        "start_line": start,
        "end_line": end,
        "kind": kind,
        "title": title,
        "documentation": documentation,
        "source_context": context,
        "examples": examples(documentation, kind),
        "references": re.findall(
            r"\\(?:copydoc|copybrief|copydetails|ref|sa)\s+([^\n]+)", documentation
        ),
    }


def api_entries(path, lines):
    index = 0
    while index < len(lines):
        match = DOC_LINE.match(lines[index])
        if not match:
            index += 1
            continue
        start = index
        block = []
        while index < len(lines) and DOC_LINE.match(lines[index]):
            block.append(DOC_LINE.match(lines[index]).group(1))
            index += 1
        documentation = "\n".join(block).strip("\n")
        if not documentation.strip() or documentation.strip() in ("@{", "@}"):
            continue
        brief = re.search(r"\\brief\s+([^\n]+)", documentation)
        title = (
            brief.group(1)
            if brief
            else next((line.strip() for line in block if line.strip()), path)
        )
        # This is a source excerpt, deliberately not a C++ signature parser.
        context = []
        for line in lines[index : index + 12]:
            if not line.strip() or DOC_LINE.match(line):
                break
            context.append(line)
            if line.rstrip().endswith((";", "{")):
                break
        yield entry(
            path,
            start + 1,
            index,
            "api",
            title,
            documentation,
            textwrap.dedent("\n".join(context)),
        )


def page_entries(path, lines):
    start = 0
    title = path
    fence = None
    for index, line in enumerate(lines):
        match = FENCE.match(line)
        if fence:
            if is_closing_fence(line, fence):
                fence = None
            continue
        if match:
            fence = match.group(1)
            continue
        heading = HEADING.match(line)
        if heading:
            if any(part.strip() for part in lines[start:index]):
                yield entry(
                    path,
                    start + 1,
                    index,
                    "page",
                    title,
                    "\n".join(lines[start:index]).strip("\n"),
                )
            start = index
            title = heading.group(2)
    if any(part.strip() for part in lines[start:]):
        yield entry(
            path,
            start + 1,
            len(lines),
            "page",
            title,
            "\n".join(lines[start:]).strip("\n"),
        )


def read_entries(root, include_internal=False):
    include = root / "include" / "libsemigroups"
    if not include.is_dir() or not (root / "docs" / "Doxyfile").is_file():
        raise ValueError(f"Not a libsemigroups checkout: {root}")
    headers = sorted(include.rglob("*.hpp"))
    pages = [root / "README.md", root / "CONTRIBUTING.rst"]
    pages += sorted((root / "docs").glob("*.md"))
    result = []
    for file in headers + pages:
        if not file.is_file() or file.name == "AGENTS.md":
            continue
        path = file.relative_to(root).as_posix()
        # Do not follow documentation symlinks outside the selected checkout.
        if root not in file.resolve().parents:
            continue
        if not include_internal and "detail" in file.relative_to(root).parts:
            continue
        lines = file.read_text(encoding="utf-8").splitlines()
        parser = api_entries if file.suffix == ".hpp" else page_entries
        result.extend(parser(path, lines))
    return result


def snippet(documentation, query_tokens, length=480):
    text = " ".join(documentation.split())
    positions = [text.casefold().find(term) for term in query_tokens]
    first = min((pos for pos in positions if pos >= 0), default=0)
    start = max(0, first - 100)
    end = min(len(text), start + length)
    return ("…" if start else "") + text[start:end] + ("…" if end < len(text) else "")


def search(entries, query, match, limit, offset):
    terms = set(tokens(query))
    if not terms:
        raise ValueError("Query must contain at least one letter or number")
    frequencies = []
    document_frequency = Counter()
    for item in entries:
        counts = Counter(tokens(item["documentation"]))
        counts.update(tokens(item["title"]) * 3)
        counts.update(tokens(item["source_context"]) * 3)
        # File names connect member documentation to its class or component.
        counts.update(tokens(Path(item["path"]).with_suffix("").as_posix()) * 2)
        frequencies.append(counts)
        document_frequency.update(terms.intersection(counts))
    average = sum(sum(counts.values()) for counts in frequencies) / max(1, len(entries))
    results = []
    for item, counts in zip(entries, frequencies):
        matched = terms.intersection(counts)
        if not matched or (match == "all" and matched != terms):
            continue
        score = 0.0
        for term in sorted(matched):
            inverse_frequency = math.log(
                1
                + (len(entries) - document_frequency[term] + 0.5)
                / (document_frequency[term] + 0.5)
            )
            frequency = counts[term]
            score += (
                inverse_frequency
                * frequency
                * 2.2
                / (frequency + 1.2 * (0.25 + 0.75 * sum(counts.values()) / average))
            )
        # Prefer an exact identifier (size) to a partial one (size_type).
        identifiers = set(re.findall(r"\w+", item["source_context"].casefold()))
        query_identifiers = set(re.findall(r"\w+", query.casefold()))
        score += 4 * len(identifiers.intersection(query_identifiers))
        result = {
            key: item[key]
            for key in (
                "id",
                "path",
                "start_line",
                "end_line",
                "kind",
                "title",
                "source_context",
            )
        }
        result.update(
            score=round(score, 6),
            snippet=snippet(item["documentation"], sorted(terms)),
            example_count=len(item["examples"]),
        )
        results.append(result)
    results.sort(key=lambda item: (-item["score"], item["path"], item["start_line"]))
    return {
        "query": query,
        "match": match,
        "total": len(results),
        "offset": offset,
        "limit": limit,
        "results": results[offset : offset + limit],
    }


class Parser(argparse.ArgumentParser):
    def error(self, message):
        raise ValueError(message)


def nonnegative(value):
    number = int(value)
    if number < 0:
        raise argparse.ArgumentTypeError("must be nonnegative")
    return number


def positive(value):
    number = nonnegative(value)
    if number == 0:
        raise argparse.ArgumentTypeError("must be positive")
    return number


def main(argv=None):
    parser = Parser(description=__doc__.split("\n")[0])
    parser.add_argument(
        "--repo",
        type=Path,
        default=Path(__file__).resolve().parent.parent,
        help="checkout to query (default: this script's checkout)",
    )
    parser.add_argument(
        "--include-internal", action="store_true", help="include headers under detail/"
    )
    commands = parser.add_subparsers(dest="command", required=True)
    find = commands.add_parser(
        "search", help="rank documentation matching query keywords"
    )
    find.add_argument("query", help="keywords or C++ names; not a query language")
    find.add_argument("--limit", type=positive, default=10)
    find.add_argument("--offset", type=nonnegative, default=0)
    find.add_argument("--match", choices=("all", "any"), default="all")
    find.add_argument(
        "--path", default="", help="restrict to paths containing this text"
    )
    find.add_argument("--kind", choices=("api", "page"))
    show = commands.add_parser(
        "show", help="retrieve a complete entry by its search result ID"
    )
    show.add_argument(
        "id", help="ID from search, e.g. include/libsemigroups/order.hpp:42"
    )
    commands.add_parser("stats", help="describe the documentation corpus")
    try:
        args = parser.parse_args(argv)
        root = args.repo.resolve()
        entries = read_entries(root, args.include_internal)
        result = {
            "schema_version": SCHEMA_VERSION,
            "repo": str(root),
            "source_format": "raw",
            "command": args.command,
        }
        if args.command == "search":
            entries = [
                item
                for item in entries
                if args.path in item["path"]
                and (args.kind is None or item["kind"] == args.kind)
            ]
            result.update(
                search(entries, args.query, args.match, args.limit, args.offset)
            )
        elif args.command == "show":
            item = next((item for item in entries if item["id"] == args.id), None)
            if item is None:
                raise ValueError(f"Unknown entry ID: {args.id}; run search again")
            result["entry"] = item
        else:
            result.update(
                entries=len(entries),
                files=len({item["path"] for item in entries}),
                kinds=dict(Counter(item["kind"] for item in entries)),
                include_internal=args.include_internal,
            )
        print(json.dumps(result, ensure_ascii=False, indent=2))
        return 0
    except (ValueError, OSError) as error:
        print(
            json.dumps({"schema_version": SCHEMA_VERSION, "error": str(error)}),
            file=sys.stderr,
        )
        return 2


if __name__ == "__main__":
    sys.exit(main())
