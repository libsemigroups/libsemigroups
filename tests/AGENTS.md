# tests/AGENTS.md

Requirements for adding or modifying tests. Setup and build commands are in
the root `AGENTS.md`.

## Declaring test cases

```cpp
LIBSEMIGROUPS_TEST_CASE("FamilyName", "042", "short description", "[quick][order]")
```

Use `LIBSEMIGROUPS_TEMPLATE_TEST_CASE` for templated tests. The macro adds
`[FamilyName]` and `[042]` as Catch2 tags automatically.

## Numbering (not checked at compile time)

The 3-digit number must be unique for the family across ALL test files. To
find the next free number, build the relevant executable, then run one of:

```sh
etc/test-next-number.sh test_order   # inspect one executable
etc/test-next-number.sh "[order]"    # inspect by tag, uses ./test_all
```

Do not run `etc/test-renumber-all.py` as part of a normal change: it
renumbers every test in the repository.

## Tags

| Tag        | Runtime                   |
| ---------- | ------------------------- |
| `quick`    | < 200ms                   |
| `standard` | < 3s                      |
| `extreme`  | > 3s                      |
| `fail`     | known to fail or time out |

Every test must have exactly one of the tags above. Also tag with the file
prefix (e.g. `[order]` in `test-order.cpp`) whenever it differs from the
family name. Tags are case insensitive. `etc/update-test-tags.py` re-times
and retags existing tests.

## Quick tests must silence reporting

Every `[quick]` test must begin with:

```cpp
auto rg = ReportGuard(false);
```

Check with `python3 etc/check-quick-report-guard.py` (run from the root).

## Adding a new test file

`tests/test-foo-bar.cpp` builds as target `test_foo_bar` (dashes become
underscores). Register the new file in `Makefile.am` in three places,
keeping alphabetical order:

1. `EXTRA_PROGRAMS += test_foo_bar`
2. `test_all_SOURCES += tests/test-foo-bar.cpp`
3. A per-target block — copy the `test_order_SOURCES`/`test_order_LDADD`
   block and adjust the name.
