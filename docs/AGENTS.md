# docs/AGENTS.md

Requirements for documentation changes: Doxygen comments (`//!`) in
`include/libsemigroups/` and the pages in `docs/`.

## Building

Requires Doxygen, the `docs/doxygen-awesome-css` submodule
(`git submodule update --init --recursive`), and
`pip install -r docs/requirements.txt` (for `docs/fixup.py`), then:

```sh
make doc   # output in docs/html/index.html
```

Doxygen and bs4 are provided by the conda environment
(`source etc/make-dev-environment.sh`, see the root `AGENTS.md`).
`pdflatex` and `inkscape` are optional system extras: `make doc` runs the
two checker scripts below and rebuilds `docs/pictures/to-table.svg` from
`to-table.tex` when they are installed, and skips the table otherwise.

## Use the project Doxygen aliases

`docs/Doxyfile` (`ALIASES`) defines project-specific commands — use them
instead of hand-writing the equivalent text. The most common:

- `\exceptions`, `\complexity`, `\parameters`, `\iterator_validity` —
  section headers
- `\noexcept`, `\no_libsemigroups_except`, `\strong_guarantee`,
  `\basic_guarantee`, `\iterator_invalid` — standard boilerplate sentences
- `\ref_todd_coxeter`, `\ref_knuth_bendix`, `\ref_kambites`,
  `\ref_congruence` — links to the main class pages
- `\deprecated_warning{...}`, `\deprecated_alias_warning{...}` and the
  `\cong_common_*` family — shared boilerplate for deprecations and the
  common congruence interface

## Section order and line breaks

Doxygen sections must appear in the order `\tparam`, `\param`, `\return`,
`\exceptions`/`\throws`, `\complexity`, `\note`, `\warning`, `\sa`, and no
documentation line may end with `\ref`, `\c`, `\p`, or `\copydoc`. Check
with (from the repository root):

```sh
python3 etc/check_doc_order.py
python3 etc/check_doxygen_line_breaks.py
```

## Code examples are compiled and run

`\code ... \endcode` blocks in the public headers are extracted into
`tests/test-docs-code-examples.cpp`. That file is auto-generated — never
edit it by hand. Use `\code_no_test ... \end_code_no_test` for examples
that should not be compiled and run (e.g. pseudo-code or snippets needing
context). After changing a documentation code example, run:

```sh
make doc-test   # regenerates, formats, compiles, and runs the examples
```

## Doc groups and the `to` table

Documentation is organised into `\defgroup`/`\ingroup` groups; new entities
must join the appropriate group (follow the pattern of a similar existing
class or helper file). When adding a new `to<ToType>(FromType)` overload:

- add it to the matching `to_*_group` (see the `to_group` overview in
  `include/libsemigroups/to-froidure-pin.hpp`), and
- update the summary table in `docs/pictures/to-table.tex` (the `.svg` is
  regenerated from it by `make doc`).

## Authors and contributors

Contributions from new contributors are credited in `docs/authors.md`
(the "Contributors" section, with a one-line description of the
contribution).
