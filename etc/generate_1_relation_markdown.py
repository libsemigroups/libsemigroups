#!/usr/bin/env python3
"""Generate a MkDocs page for one generated 1-relation example.

The expected modified presentation and active Knuth--Bendix rules may be live
assertions or commented-out assertions.  Both forms are parsed.
"""

from __future__ import annotations

import argparse
import ast
import itertools
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parent.parent
DEFAULT_SOURCE_DIR = REPO_ROOT / "examples" / "1-relation"
SOURCE_GLOB = "examples-1-relation-*.cpp"
TEST_CASE_RE = re.compile(
    r"""LIBSEMIGROUPS_TEST_CASE\(
        \s*"1-relation"\s*,\s*
        "(?P<number>\d{3})"\s*,\s*
        "(?P<description>(?:[^"\\]|\\.)*)"\s*,\s*
        "(?P<tags>[^"]*)"\s*\)
    """,
    re.VERBOSE | re.DOTALL,
)
CPP_STRING_RE = re.compile(r'"(?:[^"\\]|\\.)*"')
REDUCTION_ORDERS = {
    "LenLexTrie": ("len-lex order", "LenLexCmp"),
    "RPOTrie": ("recursive path order", "RPOCmp"),
    "RevRPOTrie": ("reversed recursive path order", "RevRPOCmp"),
}


@dataclass(frozen=True)
class PageLink:
    """The test number and flat relation used by a navigation link."""

    number: str
    relation: str


@dataclass(frozen=True)
class Example:
    """The presentations and rewriting system recorded by one test case."""

    number: str
    source: Path
    input_alphabet: str
    input_rules: list[tuple[str, str]]
    modified_alphabet: str
    modified_rules: list[tuple[str, str]]
    reduction_order_name: str
    reduction_order_comparator: str
    active_rules: list[tuple[str, str]]


def parse_args() -> argparse.Namespace:
    """Parse command-line arguments."""

    parser = argparse.ArgumentParser(
        description=(
            "Generate a MkDocs-style Markdown page for one test case in "
            "examples/1-relation/examples-1-relation-*.cpp."
        )
    )
    parser.add_argument(
        "test_case",
        help="test-case number, for example 000, 45, or 501",
    )
    parser.add_argument(
        "--source",
        type=Path,
        help=(
            "source file containing the test case; by default all generated "
            "1-relation source files are searched"
        ),
    )
    parser.add_argument(
        "--source-dir",
        type=Path,
        default=DEFAULT_SOURCE_DIR,
        help="directory searched when --source is omitted",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        help="write the page to this file instead of standard output",
    )
    return parser.parse_args()


def normalize_test_number(value: str) -> str:
    """Return ``value`` as a zero-padded three-digit test number."""

    try:
        number = int(value)
    except ValueError as exc:
        raise ValueError(f"invalid test-case number: {value!r}") from exc
    if number < 0 or number > 999:
        raise ValueError(f"test-case number must be in 000..999: {value!r}")
    return f"{number:03d}"


def cpp_string_value(token: str) -> str:
    """Decode one ordinary C++ string literal used by the generated tests."""

    return ast.literal_eval(token)


def uncomment_cpp_lines(text: str) -> str:
    """Remove one leading C++ line-comment marker from every commented line."""

    return re.sub(r"^(?P<indent>\s*)// ?", r"\g<indent>", text, flags=re.MULTILINE)


def test_case_blocks(text: str) -> list[tuple[re.Match[str], str]]:
    """Return macro matches paired with their complete test-case source."""

    matches = list(TEST_CASE_RE.finditer(text))
    result = []
    for index, match in enumerate(matches):
        end = matches[index + 1].start() if index + 1 < len(matches) else len(text)
        result.append((match, text[match.start() : end]))
    return result


def source_location(path: Path, match: re.Match[str]) -> str:
    """Return the source path and one-based line number of ``match``."""

    line = match.string.count("\n", 0, match.start()) + 1
    return f"{path}:{line}"


def find_source_and_block(
    number: str, source: Path | None, source_dir: Path
) -> tuple[Path, re.Match[str], str]:
    """Find the unique source and test-case block for ``number``."""

    sources = [source] if source is not None else sorted(source_dir.glob(SOURCE_GLOB))
    found = []
    for candidate in sources:
        if candidate is None or not candidate.is_file():
            continue
        text = candidate.read_text()
        for match, block in test_case_blocks(text):
            if match.group("number") == number:
                found.append((candidate, match, block))

    if not found:
        location = source if source is not None else source_dir
        raise ValueError(f"test case {number} was not found in {location}")
    if len(found) != 1:
        locations = ", ".join(
            source_location(path, match) for path, match, _ in found
        )
        raise ValueError(
            f"test case {number} was found more than once: {locations}"
        )
    return found[0]


def navigation_links(
    number: str, source: Path | None, source_dir: Path
) -> tuple[PageLink | None, PageLink | None]:
    """Return links to the adjacent non-failing test cases."""

    sources = {candidate.resolve() for candidate in source_dir.glob(SOURCE_GLOB)}
    if source is not None:
        sources.add(source.resolve())

    links = {}
    locations = {}
    for candidate in sorted(sources):
        if not candidate.is_file():
            continue
        for match, _ in test_case_blocks(candidate.read_text()):
            if "[fail]" in match.group("tags"):
                continue
            test_number = match.group("number")
            if test_number in links:
                duplicate_locations = ", ".join(
                    [locations[test_number], source_location(candidate, match)]
                )
                raise ValueError(
                    f"test case {test_number} was found more than once: "
                    f"{duplicate_locations}"
                )
            relation = cpp_string_value(f'"{match.group("description")}"')
            links[test_number] = PageLink(test_number, relation)
            locations[test_number] = source_location(candidate, match)

    previous_numbers = [value for value in links if value < number]
    next_numbers = [value for value in links if value > number]
    previous_page = links[max(previous_numbers)] if previous_numbers else None
    next_page = links[min(next_numbers)] if next_numbers else None
    return previous_page, next_page


def extract_string_after(pattern: str, text: str, description: str) -> str:
    """Extract and decode the first C++ string captured after ``pattern``."""

    match = re.search(pattern, text, flags=re.DOTALL)
    if match is None:
        raise ValueError(f"could not find {description}")
    return cpp_string_value(match.group("value"))


def extract_string_vector(text: str, anchor: str, description: str) -> list[str]:
    """Extract the string literals in a vector following ``anchor``."""

    anchor_index = text.find(anchor)
    if anchor_index == -1:
        raise ValueError(f"could not find {description}")
    vector_match = re.search(
        r"std::vector<std::string>\s*\(\s*\{(?P<body>.*?)\}\s*\)",
        text[anchor_index:],
        flags=re.DOTALL,
    )
    if vector_match is None:
        raise ValueError(f"could not parse {description}")
    return [
        cpp_string_value(token)
        for token in CPP_STRING_RE.findall(vector_match.group("body"))
    ]


def pair_words(words: list[str], description: str) -> list[tuple[str, str]]:
    """Pair alternating left- and right-hand sides in ``words``."""

    if len(words) % 2 != 0:
        raise ValueError(f"{description} contains an odd number of words")
    return list(zip(words[::2], words[1::2]))


def extract_active_rules(text: str) -> list[tuple[str, str]]:
    """Extract the active Knuth--Bendix rules from normalized test source."""

    anchor_index = text.find("kb.active_rules()")
    if anchor_index == -1:
        raise ValueError("could not find kb.active_rules() output")
    vector_match = re.search(
        r"std::vector<rule_type>\s*\(\s*\{(?P<body>.*?)\}\s*\)",
        text[anchor_index:],
        flags=re.DOTALL,
    )
    if vector_match is None:
        raise ValueError("could not parse kb.active_rules() output")

    rules = []
    for match in re.finditer(
        rf"\{{\s*(?P<lhs>{CPP_STRING_RE.pattern})\s*,"
        rf"\s*(?P<rhs>{CPP_STRING_RE.pattern})\s*\}}",
        vector_match.group("body"),
        flags=re.DOTALL,
    ):
        rules.append(
            (
                cpp_string_value(match.group("lhs")),
                cpp_string_value(match.group("rhs")),
            )
        )
    if not rules:
        raise ValueError("kb.active_rules() output contains no rules")
    return rules


def extract_reduction_order(text: str) -> tuple[str, str]:
    """Extract the readable name and comparator type of the reduction order."""

    match = re.search(
        r"KnuthBendix\s*<\s*std::string\s*,\s*"
        r"(?P<trie>LenLexTrie|RPOTrie|RevRPOTrie)\s*>",
        text,
    )
    if match is None:
        raise ValueError("could not find the Knuth--Bendix reduction order")
    return REDUCTION_ORDERS[match.group("trie")]


def parse_example(number: str, source: Path | None, source_dir: Path) -> Example:
    """Parse one generated example, including commented expected output."""

    source_path, _, block = find_source_and_block(number, source, source_dir)
    normalized = uncomment_cpp_lines(block)

    input_alphabet = extract_string_after(
        r'p\.alphabet\(\s*(?P<value>"(?:[^"\\]|\\.)*")\s*\)',
        normalized,
        "input alphabet",
    )
    input_rule_match = re.search(
        r"presentation::add_rule\(\s*p\s*,\s*"
        rf"(?P<lhs>{CPP_STRING_RE.pattern})\s*,\s*"
        rf"(?P<rhs>{CPP_STRING_RE.pattern})\s*\)",
        normalized,
        flags=re.DOTALL,
    )
    if input_rule_match is None:
        raise ValueError("could not find the input relation")
    input_rules = [
        (
            cpp_string_value(input_rule_match.group("lhs")),
            cpp_string_value(input_rule_match.group("rhs")),
        )
    ]

    modified_alphabet = extract_string_after(
        r"result\.value\(\)\.alphabet\(\)\s*==\s*"
        r'(?P<value>"(?:[^"\\]|\\.)*")',
        normalized,
        "modified alphabet output",
    )
    modified_words = extract_string_vector(
        normalized,
        "result.value().rules",
        "modified presentation rules",
    )
    modified_rules = pair_words(modified_words, "modified presentation rules")
    reduction_order_name, reduction_order_comparator = extract_reduction_order(
        normalized
    )

    return Example(
        number=number,
        source=source_path,
        input_alphabet=input_alphabet,
        input_rules=input_rules,
        modified_alphabet=modified_alphabet,
        modified_rules=modified_rules,
        reduction_order_name=reduction_order_name,
        reduction_order_comparator=reduction_order_comparator,
        active_rules=extract_active_rules(normalized),
    )


def latex_word(word: str) -> str:
    """Return a word in compact LaTeX, using powers for repeated letters."""

    if not word:
        return "1"

    result = []
    for letter, occurrences in itertools.groupby(word):
        count = sum(1 for _ in occurrences)
        escaped = latex_flat_word(letter)
        result.append(escaped if count == 1 else rf"{escaped}^{{{count}}}")
    return " ".join(result)


def latex_flat_word(word: str) -> str:
    """Return a word as a flat LaTeX string."""

    if not word:
        return "1"

    return (
        word.replace("\\", r"\backslash ")
        .replace("{", r"\{")
        .replace("}", r"\}")
        .replace("_", r"\_")
        .replace("%", r"\%")
        .replace("#", r"\#")
        .replace("&", r"\&")
        .replace("$", r"\$")
    )


def latex_presentation(alphabet: str, rules: list[tuple[str, str]]) -> list[str]:
    """Return a LaTeX presentation, wrapping relations longer than 100 chars."""

    generators = ", ".join(latex_word(letter) for letter in alphabet)
    relation_length = len(", ".join(f"{lhs or '1'}={rhs or '1'}" for lhs, rhs in rules))
    if relation_length <= 100:
        relations = r",\; ".join(
            f"{latex_word(lhs)} = {latex_word(rhs)}" for lhs, rhs in rules
        )
        return [
            r"\left\langle",
            rf"  {generators} \;\middle|\; {relations}",
            r"\right\rangle",
        ]

    result = [
        r"\left\langle",
        rf"  {generators} \;\middle|\;",
        r"  \begin{aligned}",
    ]
    for index, (lhs, rhs) in enumerate(rules):
        suffix = r", \\" if index + 1 < len(rules) else ""
        result.append(f"    {latex_word(lhs)} &= {latex_word(rhs)}{suffix}")
    result.extend([r"  \end{aligned}", r"\right\rangle"])
    return result


def latex_rewriting_system(rules: list[tuple[str, str]]) -> list[str]:
    """Return the active rewriting system as aligned display mathematics."""

    result = [r"\begin{aligned}"]
    for index, (lhs, rhs) in enumerate(rules):
        suffix = r" \\" if index + 1 < len(rules) else ""
        result.append(f"  {latex_word(lhs)} &\\rightarrow {latex_word(rhs)}{suffix}")
    result.append(r"\end{aligned}")
    return result


def markdown_page(
    example: Example,
    previous_page: PageLink | None = None,
    next_page: PageLink | None = None,
) -> str:
    """Render ``example`` as a MkDocs-style Markdown page."""

    if len(example.input_rules) != 1:
        raise ValueError("the input presentation does not have exactly one relation")
    lhs, rhs = example.input_rules[0]
    title = f"#{example.number} {lhs or '1'}={rhs or '1'}"
    navigation = []
    if previous_page is not None:
        navigation.append(
            f"Previous: [#{previous_page.number} {previous_page.relation}]"
            f"({previous_page.number}.md)"
        )
    if next_page is not None:
        navigation.append(
            f"Next: [#{next_page.number} {next_page.relation}]"
            f"({next_page.number}.md)"
        )
    generator_order = " < ".join(
        latex_flat_word(letter) for letter in example.modified_alphabet
    )
    lines = [
        "---",
        f"title: {json.dumps(title, ensure_ascii=False)}",
        "---",
        "",
        f"# {title}",
        "",
        " · ".join(navigation),
        "",
        "Original presentation:",
        "",
        "$$",
        *latex_presentation(example.input_alphabet, example.input_rules),
        "$$",
        "",
        "Modified presentation:",
        "",
        "$$",
        *latex_presentation(
            "".join(sorted(example.modified_alphabet)), example.modified_rules
        ),
        "$$",
        "",
        (
            f"Complete rewriting system: using {example.reduction_order_name} "
            f"(`{example.reduction_order_comparator}`) with "
            f"\\({generator_order}\\)"
        ),
        "",
        "$$",
        *latex_rewriting_system(example.active_rules),
        "$$",
        "",
        (f"<!-- Generated from {example.source.name}, test case {example.number}. -->"),
        "",
    ]
    return "\n".join(lines)


def main() -> int:
    """Run the command-line interface."""

    args = parse_args()
    try:
        number = normalize_test_number(args.test_case)
        example = parse_example(number, args.source, args.source_dir)
        previous_page, next_page = navigation_links(
            number, args.source, args.source_dir
        )
        page = markdown_page(example, previous_page, next_page)
        if args.output is None:
            sys.stdout.write(page)
        else:
            args.output.write_text(page)
    except (OSError, SyntaxError, ValueError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
