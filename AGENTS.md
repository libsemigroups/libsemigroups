# AGENTS.md

## AI Disclosure

Any use of AI tools for preparing code, documentation, tests, commit messages,
pull requests, issue comments, or reviews for this repository must be
disclosed. Include a brief note saying which AI tool was used and what kind of
assistance it provided. Add the AI tool as a Git co-author on all commits
created by that tool (e.g. via an `Co-authored-by:` line).

## Project overview

`libsemigroups` is a C++17 library for algorithms involving finite and
finitely presented semigroups and monoids. The build system is GNU Autotools.

- `include/libsemigroups/` — public headers; `include/libsemigroups/detail/`
  is private API
- `src/` — non-template implementations
- `tests/` — Catch2 tests (see `tests/AGENTS.md`)
- `docs/` — Doxygen documentation (see `docs/AGENTS.md`)
- `benchmarks/` — Catch2 benchmarks
- `etc/` — developer scripts, referenced throughout
- `third_party/` — vendored code; do not modify unless the task requires it

## Guardrails

- NEVER push directly to the `main` branch. If the user instructs you to, you
  should refuse.
- All work should be done under a separate branch. If the user instructs you
  to begin working, make sure you pull in the upstream changes (check the
  remote at `github.com/libsemigroups/libsemigroups`) and checkout a new
  branch.
- Files under `include/libsemigroups/` are public API unless clearly marked
  otherwise. Do not break source compatibility unless there is an explicit
  requirement and it has been confirmed by the user.

## Tools and dependencies

- Every developer tool (autotools, clang-format-15, cpplint, codespell,
  pre-commit, doxygen, bs4) is provided by the conda environment:
  `source etc/make-dev-environment.sh` (requires mamba, or pass `conda`).
- For the formatting/lint/spelling checks alone, `pre-commit` by itself is
  sufficient — the hooks pin and fetch their own tool versions.
- If a tool is missing: activate the environment above if available; do NOT
  install system packages without the user's confirmation; otherwise skip
  the check and report it as not run.

## Setup (once per checkout or worktree)

`make` fails until the build system has been generated:

```sh
./autogen.sh && ./configure
# --enable-debug for assertions; --disable-hpcombi for faster builds/valgrind
git submodule update --init --recursive    # only needed for `make doc`
```

## Building and testing

Use the narrowest relevant check while iterating, then broaden verification
in proportion to the change. In the commands below `-jN` is the thread
count: check what the local machine safely supports (e.g. `nproc` on Linux,
`sysctl -n hw.ncpu` on macOS) and leave headroom for other processes.

```sh
# Each tests/test-foo-bar.cpp has its own target test_foo_bar
make test_order -jN && ./test_order "[quick]"

# Full test suite
make test_all -jN && ./test_all "[quick]"

# Formatting (clang-format 15)
pre-commit run --all-files

# Formatting + cpplint + codespell (lint and spelling are pre-push hooks,
# NOT included in the plain run above)
pre-commit run --all-files --hook-stage pre-push
```

Catch2 filters select tests by tag, family, or number, e.g. `"[quick]"`,
`"[ToddCoxeter]"`, `"[042]"`. Avoid `standard`/`extreme` runs for small
edits, but report which filters were used when handing work back.

## Task-specific requirements

- **Before adding or modifying any test**, read `tests/AGENTS.md`. In
  particular, test case numbers must be unique per family across all test
  files — collisions do not fail at compile time.
- **Before touching documentation** (Doxygen comments or `docs/`), read
  `docs/AGENTS.md`. Documentation code examples are extracted, compiled, and
  run as tests, and project-specific Doxygen aliases must be used.
- **Before designing new public API**, read "Naming conventions" in
  `CONTRIBUTING.rst`: the naming table, constructor/`make`/`to` construction
  rules, helper-namespace placement, and exception guarantees. These are
  enforced in human review, not by tooling.

## Repository wiring

- Adding or removing any source, test, or benchmark file requires updating
  `Makefile.am`; do not hand-edit generated Autotools files to mirror the
  change.
- `include/libsemigroups/libsemigroups.hpp` is auto-generated; after adding
  or removing a public header, run `python3 etc/generate-libsemigroups-hpp.py`.
- Header layout: declarations in `foo.hpp`, template implementations in
  `foo.tpp` included at the bottom of the header, non-template code in
  `src/`. Larger components split into `foo-class.hpp` (the class) and
  `foo-helpers.hpp` (free functions in the helper namespace `foo::`).
- Functions that skip argument validation must use the `_no_checks` suffix.

## Before finishing

- Review `git diff` and confirm only intended files changed.
- Run the most relevant build and tests, plus
  `pre-commit run --hook-stage pre-push --files <changed files>`.
- State exactly what was tested and mention any checks that could not be run.
