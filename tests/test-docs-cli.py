#!/usr/bin/env python3
"""Regression tests for the documentation CLI (Python standard library only)."""

import importlib.util
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import textwrap
import unittest


SCRIPT = Path(__file__).resolve().parents[1] / "etc" / "libsemigroups-docs.py"
SPEC = importlib.util.spec_from_file_location("docs_cli", SCRIPT)
CLI = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(CLI)


class DocumentationCLI(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name).resolve()
        self.write("docs/Doxyfile", "INPUT = ../include/libsemigroups")
        self.header = textwrap.dedent(r"""\
            // Not documentation: secretimplementation
            //! \brief Returns the size of a FroidurePin instance.
            //!
            //! Fully enumerates the semigroup.
            //! \exceptions
            //! \no_libsemigroups_except
            //! \code
            //! auto n = S.size();
            //! if (n > 0) {
            //!   use(n);
            //! }
            //! \endcode
            size_t size();

            //! \copydoc FroidurePinBase::size_type
            using size_type = unsigned;

            //! \brief Enumerate a bounded number of elements.
            //! \code_no_test
            //! S.enumerate(100);
            //! \end_code_no_test
            void enumerate(size_t limit);
            """).removeprefix("\\\n")
        self.write("include/libsemigroups/froidure-pin.hpp", self.header)
        self.write(
            "include/libsemigroups/detail/hidden.hpp",
            "//! Internal implementation detail.\nvoid hidden();\n",
        )
        self.write(
            "docs/guide.md",
            "# Guide\nIntroduction.\n\n## Examples\n"
            "```cpp\n# not a heading\n~~~\n```\n\n## Installation\nInstall locally.\n",
        )
        self.write("docs/AGENTS.md", "# Private agent instructions\nagentonlytext\n")
        self.write("docs/vendor/ignored.md", "# vendoredonlytext\n")
        self.entries = CLI.read_entries(self.root)

    def write(self, path, content):
        target = self.root / path
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(content, encoding="utf-8")

    def run_cli(self, *args, returncode=0):
        result = subprocess.run(
            [sys.executable, str(SCRIPT), "--repo", str(self.root), *args],
            cwd=self.temporary.name,
            capture_output=True,
            text=True,
            check=False,
        )
        self.assertEqual(result.returncode, returncode, result.stderr)
        self.assertEqual(result.stderr if returncode == 0 else result.stdout, "")
        return json.loads(result.stdout if returncode == 0 else result.stderr)

    def test_identifiers_and_ranking(self):
        for query in (
            "FroidurePin size",
            "froidure_pin::size",
            "froidure-pin size",
            "libsemigroups::FroidurePin::size",
        ):
            with self.subTest(query=query):
                result = CLI.search(self.entries, query, "all", 10, 0)
                self.assertEqual(
                    result["results"][0]["source_context"], "size_t size();"
                )

    def test_exact_source_locations_and_examples(self):
        item = self.entries[0]
        source = self.header.splitlines()[item["start_line"] - 1 : item["end_line"]]
        self.assertTrue(all(CLI.DOC_LINE.match(line) for line in source))
        self.assertEqual(
            item["documentation"],
            "\n".join(CLI.DOC_LINE.match(line).group(1) for line in source),
        )
        self.assertEqual(
            item["examples"],
            [
                {
                    "kind": "code",
                    "text": "auto n = S.size();\nif (n > 0) {\n  use(n);\n}",
                }
            ],
        )
        self.assertEqual(self.entries[2]["examples"][0]["kind"], "code_no_test")
        self.assertIn(r"\no_libsemigroups_except", item["documentation"])

    def test_references_are_preserved(self):
        self.assertEqual(self.entries[1]["references"], ["FroidurePinBase::size_type"])

    def test_markdown_fences_do_not_create_sections(self):
        pages = [item for item in self.entries if item["kind"] == "page"]
        self.assertEqual(
            [item["title"] for item in pages], ["Guide", "Examples", "Installation"]
        )
        self.assertEqual(
            pages[1]["examples"], [{"kind": "fenced", "text": "# not a heading\n~~~"}]
        )

    def test_exclusions_and_internal_opt_in(self):
        for query in (
            "hidden",
            "agentonlytext",
            "vendoredonlytext",
            "secretimplementation",
        ):
            self.assertEqual(CLI.search(self.entries, query, "all", 10, 0)["total"], 0)
        result = self.run_cli("--include-internal", "search", "hidden")
        self.assertEqual(result["total"], 1)

    def test_search_show_round_trip(self):
        result = self.run_cli("search", "FroidurePin::size", "--limit", "1")
        self.assertEqual(result["schema_version"], 1)
        item = self.run_cli("show", result["results"][0]["id"])["entry"]
        self.assertIn("Fully enumerates", item["documentation"])
        self.assertEqual(item["path"], "include/libsemigroups/froidure-pin.hpp")

    def test_pagination_and_filters(self):
        full = self.run_cli("search", "FroidurePin")
        page = self.run_cli("search", "FroidurePin", "--limit", "1", "--offset", "1")
        self.assertEqual(page["results"], full["results"][1:2])
        self.assertEqual(page["total"], full["total"])
        self.assertEqual(self.run_cli("search", "size", "--kind", "page")["total"], 0)
        self.assertEqual(
            self.run_cli("search", "Install", "--path", "docs/guide")["total"], 1
        )

    def test_all_and_any_matching(self):
        self.assertEqual(self.run_cli("search", "size nonexistent")["total"], 0)
        self.assertGreater(
            self.run_cli("search", "size nonexistent", "--match", "any")["total"], 0
        )

    def test_empty_results_are_successful(self):
        result = self.run_cli("search", "nonexistent")
        self.assertEqual(result["total"], 0)
        self.assertEqual(result["results"], [])

    def test_errors_are_json_on_stderr(self):
        for args in (
            ("search", "!!!"),
            ("search", "size", "--limit", "0"),
            ("search", "size", "--offset", "-1"),
            ("show", "../../secret:1"),
            ("unknown",),
            ("search",),
            ("search", "size", "--limit", "abc"),
        ):
            with self.subTest(args=args):
                self.assertIn("error", self.run_cli(*args, returncode=2))

    def test_invalid_checkout_is_actionable(self):
        (self.root / "docs/Doxyfile").unlink()
        result = self.run_cli("stats", returncode=2)
        self.assertIn("Not a libsemigroups checkout", result["error"])

    def test_reloads_edits_without_a_cache(self):
        self.write("docs/guide.md", "# Newlyedited\nFresh content.\n")
        self.assertEqual(self.run_cli("search", "Newlyedited")["total"], 1)

    def test_real_checkout(self):
        entries = CLI.read_entries(SCRIPT.parent.parent)
        result = CLI.search(entries, "FroidurePin size", "all", 1, 0)
        self.assertIn("size()", result["results"][0]["source_context"])
        result = CLI.search(entries, "ToddCoxeter", "all", 10, 0)
        self.assertGreater(result["total"], 0)


if __name__ == "__main__":
    unittest.main()
