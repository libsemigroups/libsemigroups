#!/usr/bin/env python3
"""Generate test-tietze.cpp FindIf test cases from a CSV of word pairs.

The CSV file may have a header with columns named ``lhs`` and ``rhs`` (as in
``kb_backtrack_examples.csv``), or it may simply contain two columns.  The
generated C++ is written to stdout.
"""

import argparse
import csv
import sys
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate FindIf tests analogous to test-tietze.cpp case 022.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("csv_file", type=Path, help="CSV file containing word pairs")
    parser.add_argument(
        "--start",
        type=int,
        default=23,
        help="first test number to use; 23 is suitable for appending after 022",
    )
    parser.add_argument(
        "--skip",
        type=int,
        default=0,
        help="number of CSV data rows to skip before generating tests",
    )
    parser.add_argument(
        "--limit",
        type=int,
        help="maximum number of tests to generate after --skip is applied",
    )
    parser.add_argument(
        "--alphabet",
        help="alphabet to use in every generated test; defaults to the sorted "
        "characters appearing in each word pair",
    )
    parser.add_argument(
        "--depth",
        type=int,
        default=2,
        help="template argument for pedersen_pestov",
    )
    parser.add_argument(
        "--min-length",
        type=int,
        default=2,
        help="minimum subword length passed to pedersen_pestov",
    )
    parser.add_argument(
        "--max-length",
        type=int,
        default=4,
        help="maximum subword length passed to pedersen_pestov",
    )
    parser.add_argument(
        "--run-for-ms",
        type=int,
        default=1,
        help="milliseconds passed to kb.run_for in the FindIf predicate",
    )
    parser.add_argument(
        "--threads",
        type=int,
        default=1,
        help="number of threads passed to FindIf",
    )
    parser.add_argument(
        "--tags",
        default="[standard]",
        help="Catch2 tags for every generated test",
    )
    parser.add_argument(
        "--case-name",
        default="FindIf",
        help="first argument to LIBSEMIGROUPS_TEST_CASE",
    )
    parser.add_argument(
        "--no-empty-word",
        action="store_true",
        help="omit p.contains_empty_word(true)",
    )
    return parser.parse_args()


def cxx_string_literal(value: str) -> str:
    escaped = (
        value.replace("\\", "\\\\")
        .replace('"', '\\"')
        .replace("\n", "\\n")
        .replace("\r", "\\r")
        .replace("\t", "\\t")
    )
    return f'"{escaped}"'


def non_empty_rows(csv_file: Path) -> list[list[str]]:
    with csv_file.open(newline="") as f:
        return [
            [cell.strip() for cell in row]
            for row in csv.reader(f)
            if any(cell.strip() for cell in row)
        ]


def word_pairs(csv_file: Path) -> list[tuple[str, str]]:
    rows = non_empty_rows(csv_file)
    if not rows:
        raise ValueError(f"{csv_file} does not contain any rows")

    first = [cell.lower() for cell in rows[0]]
    has_header = "lhs" in first and "rhs" in first
    if has_header:
        lhs_index = first.index("lhs")
        rhs_index = first.index("rhs")
        rows = rows[1:]
    else:
        lhs_index = 0
        rhs_index = 1

    pairs = []
    for line_number, row in enumerate(rows, start=2 if has_header else 1):
        if len(row) <= max(lhs_index, rhs_index):
            raise ValueError(f"row {line_number} has fewer than two columns")
        lhs = row[lhs_index]
        rhs = row[rhs_index]
        if not lhs or not rhs:
            raise ValueError(f"row {line_number} contains an empty word")
        pairs.append((lhs, rhs))
    return pairs


def alphabet_for(lhs: str, rhs: str, override: str | None) -> str:
    if override is not None:
        return override
    return "".join(sorted(set(lhs + rhs)))


def emit_test(
    *,
    number: int,
    lhs: str,
    rhs: str,
    alphabet: str,
    args: argparse.Namespace,
) -> str:
    contains_empty_word = ""
    if not args.no_empty_word:
        contains_empty_word = "    p.contains_empty_word(true);\n"

    return f"""  LIBSEMIGROUPS_TEST_CASE(
      {cxx_string_literal(args.case_name)},
      {cxx_string_literal(str(number).zfill(3))},
      {cxx_string_literal(f"{lhs}={rhs}")},
      {cxx_string_literal(args.tags)}) {{
    using rx::operator|;
    using literals::operator\"\"_p;
    using std::string_literals::operator\"\"s;

    auto rg = ReportGuard(false);

    Presentation<std::string> p;
    p.alphabet({cxx_string_literal(alphabet)});
{contains_empty_word}    presentation::add_rule(
        p, {cxx_string_literal(lhs)}, {cxx_string_literal(rhs)});

    KnuthBendix<std::string, RPOTrie> kb(congruence_kind::twosided, p);

    auto input = (p | pedersen_pestov<{args.depth}>(kb)
                          .min_length({args.min_length})
                          .max_length({args.max_length})
                          .proper(true));

    auto find_if = FindIf([kb](auto const& p) mutable {{
                     kb.init(congruence_kind::twosided, p);
                     kb.run_for(std::chrono::milliseconds({args.run_for_ms}));
                     return kb.rewriting_system().confluent();
                   }}).number_of_threads({args.threads});

    auto result = (input | find_if).get();

    REQUIRE(result.has_value());
  }}
"""


def main() -> int:
    args = parse_args()
    if args.start < 0:
        raise ValueError("--start must be non-negative")
    for name in ("skip", "depth", "min_length", "max_length", "run_for_ms", "threads"):
        if getattr(args, name) < 0:
            raise ValueError(f"--{name.replace('_', '-')} must be non-negative")
    if args.depth == 0:
        raise ValueError("--depth must be positive")
    if args.threads == 0:
        raise ValueError("--threads must be positive")
    if args.min_length > args.max_length:
        raise ValueError("--min-length must be at most --max-length")

    pairs = word_pairs(args.csv_file)
    pairs = pairs[args.skip :]
    if args.limit is not None:
        if args.limit < 0:
            raise ValueError("--limit must be non-negative")
        pairs = pairs[: args.limit]

    print("// Generated by etc/generate-tietze-findif-tests.py")
    print("// Paste into tests/test-tietze.cpp inside namespace libsemigroups.")
    print()
    for offset, (lhs, rhs) in enumerate(pairs):
        print(
            emit_test(
                number=args.start + offset,
                lhs=lhs,
                rhs=rhs,
                alphabet=alphabet_for(lhs, rhs, args.alphabet),
                args=args,
            )
        )
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError) as e:
        print(f"error: {e}", file=sys.stderr)
        raise SystemExit(1)
