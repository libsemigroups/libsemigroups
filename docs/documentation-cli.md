# Querying documentation from agents

The local documentation CLI returns JSON using Python 3.9 or later and its
standard library. It reads the checkout on every invocation, including local
edits. No library build, Doxygen build, network connection, or saved index is
required.

From the repository root:

```sh
python3 etc/libsemigroups-docs.py search 'FroidurePin size'
python3 etc/libsemigroups-docs.py search 'libsemigroups::ToddCoxeter' --limit 5
python3 etc/libsemigroups-docs.py search 'install' --kind page
python3 etc/libsemigroups-docs.py stats
```

Use `show` with an `id` returned by `search` to retrieve the complete entry:

```sh
python3 etc/libsemigroups-docs.py show '<id from search>'
```

The script can be invoked by absolute path from any working directory. It
defaults to the checkout containing the script. To query another checkout, put
`--repo /path/to/libsemigroups` before the command.

## Search behavior

Search takes keywords or C++ names. It ignores case and treats CamelCase,
snake_case, and hyphenated names equivalently. All query words must match by
default; `--match any` allows partial matches. This is lexical search: it does
not interpret natural-language questions or expand synonyms. Use focused terms
such as `ToddCoxeter number_of_classes` or `presentation rules`.

Results are ranked using term frequency and rarity, with extra weight for titles,
adjacent source declarations, and exact identifiers. File names associate member
documentation with its component. Scores are relative to the selected corpus.
They are ranking scores, not confidence estimates.

Options following `search`:

| Option        | Meaning                                                 |
| ------------- | ------------------------------------------------------- |
| `--limit N`   | Return at most N entries; default 10, must be positive. |
| `--offset N`  | Skip N ranked entries; default 0.                       |
| `--path TEXT` | Keep paths containing this case-sensitive substring.    |
| `--kind api`  | Search documentation comments in headers.               |
| `--kind page` | Search guide pages.                                     |
| `--match any` | Match any query word instead of all words.              |

Headers under `detail/` are excluded by default. Put `--include-internal` before
the command to include them, including when retrieving one with `show`.

## Output contract

Successful commands write one JSON object to standard output and exit with
status 0. Every response includes `schema_version` (currently 1), the absolute
`repo` path, `source_format` (`raw`), and `command`.

- `search` includes `query`, `match`, `total`, `offset`, `limit`, and `results`.
  Each result has an `id`, repository-relative `path`, inclusive one-based
  `start_line` and `end_line`, `kind`, `title`, `source_context`, `score`, a
  snippet of at most 482 characters, and `example_count`.
- `show` includes an `entry` with the same source identity, full `documentation`,
  `source_context`, extracted `examples`, and raw Doxygen `references`.
  Example objects contain `kind` (`code`, `code_no_test`, or `fenced`) and `text`.
- `stats` includes entry and file counts, counts by kind, and `include_internal`.

No matches is a successful search with `total: 0` and `results: []`. Invalid
arguments, unknown IDs, invalid checkouts, and read errors exit with status 2,
write a JSON object with `schema_version` and `error` to standard error, and
leave standard output empty. `--help` prints conventional CLI help text.

For an agent, the intended workflow is to search, select relevant IDs, then use
`show` to read full documentation and examples before writing code. Cite the
returned path and line numbers. IDs are `path:start_line` references to the
current source, so repeat the search after editing or updating the checkout.

## Coverage and limitations

The corpus consists of contiguous `//!` documentation blocks in
`include/libsemigroups/**/*.hpp`, the root README and contribution guide, and
top-level Markdown pages in `docs/`. Markdown pages are split at ATX headings;
the contribution guide is returned as one entry. Agent instructions, vendored
documentation, and generated HTML are excluded.

Documentation is returned as written, with `//!` prefixes removed. Doxygen
aliases, references, conditional sections, inheritance, and `copydoc` are not
expanded. Follow a reference by searching its target. `source_context` is a
bounded excerpt of up to 12 following source lines, not a parsed C++ signature;
the source line range identifies the documentation itself. This tool does not
replace a compiler or Doxygen's resolved view of the API.

## Development

Run the regression tests with `python3 tests/test-docs-cli.py`, or
`make check-docs-cli` in a configured checkout. The tool and its tests do not
require compiling the C++ library.
