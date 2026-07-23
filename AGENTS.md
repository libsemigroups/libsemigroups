# AGENTS.md

## Project overview

`libsemigroups` is a C++17 library for algorithms involving finite and
finitely presented semigroups and monoids. Public headers live in `include/`,
implementations in `src/`, tests in `tests/`, and user documentation in
`docs/`.

## Working in this repository

- Keep changes focused and preserve unrelated work in the working tree.
- Follow the naming, exception-safety, construction, and documentation rules
  in `CONTRIBUTING.rst`.
- Format C++ with clang-format 15 using the repository `.clang-format` file.
  Use formatting or lint suppressions only when necessary and explain why in
  the code.
- Add or update tests for behaviour changes. Tests use Catch2 and are declared
  with `LIBSEMIGROUPS_TEST_CASE`. Tag new cases `quick`, `standard`, or
  `extreme` as described in `CONTRIBUTING.rst`.
- If a source or test target is added or removed, update `Makefile.am`; do not
  hand-edit generated Autotools files solely to mirror that change.
- Do not modify vendored code in `third_party/` unless the task specifically
  requires it.

## Building and testing

Use the narrowest relevant check while iterating, then broaden verification in
proportion to the change.

```sh
# Build the library and test executables
make -j2

# Run one test executable (for example, tests/test-order.cpp)
make -j2 test_order
./test_order "[quick]"

# Build and run the complete test suite
make -j2 test_all
./test_all "[quick]"

# Run repository formatting, lint, and spelling hooks
pre-commit run --all-files

# Build documentation (requires Doxygen and initialized submodules)
make doc
```

Catch2 filters can select a test name, number, or tag. Avoid running
`standard` or `extreme` tests unnecessarily during a small edit, but report
which filters were used when handing work back.

For documentation changes, also run:

```sh
python3 etc/check_doc_order.py
```

## Documentation and API expectations

- Treat files under `include/libsemigroups/` as public API unless clearly
  marked otherwise; avoid breaking source compatibility without an explicit
  requirement.
- Document argument validation, exceptions, complexity, and failure behaviour
  where relevant. Preserve the Doxygen section order specified in
  `CONTRIBUTING.rst`.
- Functions that deliberately skip validation must follow the `_no_checks`
  convention described in `CONTRIBUTING.rst`.
- Prefer nearby code and tests as the model for style and API shape.

## Before finishing

- Review `git diff` and confirm only intended files changed.
- Run the most relevant build and tests, plus formatting/lint checks for
  touched files when available.
- State exactly what was tested and mention any checks that could not be run.
