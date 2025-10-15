import os
import sys
import argparse
from pathlib import Path

########################################################################
# CONSTANTS
########################################################################

HEADER_TEXT = """// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2025 James D. Mitchell + Maria Tsalakou
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE, REQUIRE_NOTHROW, REQUIRE_THROWS_AS
#include "libsemigroups/libsemigroups.hpp"  // for *
#include "test-main.hpp"                    // for LIBSEMIGROUPS_TEST_CASE
"""

########################################################################
# INTERNAL
########################################################################


def __error(msg: str) -> None:
    sys.stderr.write(f"\033[0;31m{msg}\n\033[0m")


def __bold(msg: str) -> None:
    sys.stderr.write(f"\033[1m{msg}\n\033[0m")


def __parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Extract code example blocks from documentation.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
        Examples:
        python3 extract_code.py <folder>    <-- Process specific folder
        Flags:
        -r | Process folder recursively
        """,
    )

    parser.add_argument(
        "folder_path",
        nargs="?",
        default=".",
        help="Path to the folder to process (default: current directory)",
    )

    parser.add_argument(
        "-r",
        "--recursive",
        action="store_true",
        help="Process subdirectories recursively",
    )

    parser.add_argument(
        "--version", action="version", version="Code Block Extractor 1.0"
    )

    args = parser.parse_args()

    return args


########################################################################
# CODE BLOCK EXTRACTION
########################################################################


def extract_code_blocks(file_path):
    """
    Extract code examples from a file.

    Args:
        file_path (Path): Path to the file to process

    Returns:
        list: List of code blocks found in the file
        [["content", "start_line"], ...]
    """
    code_blocks = []

    try:
        with open(file_path, "r", encoding="utf-8", errors="ignore") as file:
            in_code_block = False
            ignore_this_block = False
            current_block = []

            for line_num, line in enumerate(file, 1):
                line = line.lstrip()
                line = line.removeprefix("//!")
                line = line.rstrip("\n\r")

                if "\\code" in line:
                    if in_code_block:
                        __error(
                            f"Warning: Found \\code while already in code block at {
                                file_path
                            }:{line_num}"
                        )
                    in_code_block = True
                    current_block = []
                    continue

                if "\\endcode" in line:
                    if not in_code_block:
                        __error(
                            f"Warning: Found \\endcode without matching \\code at {
                                file_path
                            }:{line_num}"
                        )
                    elif not ignore_this_block:
                        code_blocks.append(
                            {
                                "content": "\n".join(current_block),
                                "start_line": line_num - len(current_block),
                            }
                        )
                    in_code_block = False
                    ignore_this_block = False
                    current_block = []
                    continue

                if in_code_block:
                    if "using namespace libsemigroups" in line:
                        continue

                    # If line contains non-executable statements,
                    # skip this block.
                    if "\\skip-test" in line:
                        ignore_this_block = True
                        current_block = []
                        __bold(
                            f"Note: Example code block skipped in {file_path} at line {
                                line_num
                            }"
                        )
                        continue

                    if not ignore_this_block:
                        current_block.append(line)

            # discard unclosed code blocks
            if in_code_block and current_block:
                __error(f"Warning: Unclosed code block at end of file {file_path}")

    except Exception as e:
        print(f"Error reading file {file_path}: {e}")

    return code_blocks


def process_folder(folder_path, recursive=False):
    """
    Process all files in a folder to find code examples.

    Args:
        folder_path (string): Path to the folder to process
        recursive (bool): Whether to process subdirectories recursively
    """
    folder = Path(folder_path)

    if not folder.exists():
        print(f"Error: Directory '{folder_path}' does not exist.")
        sys.exit(1)

    if not folder.is_dir():
        print(f"Error: '{folder_path}' is not a directory.")
        sys.exit(1)

    print(f"Searching for code blocks in folder: {folder.absolute()}")
    if recursive:
        print("Processing subdirectories recursively...")
    print("=" * 60)

    # Get files
    if recursive:
        files = folder.rglob("*")
    else:
        files = folder.glob("*")

    # Filter
    files = [f for f in files if f.is_file()]

    if len(files) == 0:
        print("No files found in the specified directory.")
        return

    total_blocks = 0

    try:
        with open("./tests/test-docs-code-examples.cpp", "w") as testfile:
            testfile.write(f"{HEADER_TEXT}\n")
            testfile.write("namespace libsemigroups {\n")  # Open namespace

            for file_path in sorted(files):
                # Extract example code
                code_blocks = extract_code_blocks(file_path)

                # Write to cpp test file
                if code_blocks:
                    for i, block in enumerate(code_blocks, 1):
                        testfile.write(
                            f'// {file_path.relative_to(folder)}: Line {
                                block["start_line"]
                            }\nLIBSEMIGROUPS_TEST_CASE("docs", "{total_blocks}", "{
                                file_path.relative_to(folder)
                            }", "[docs][quick]") {{\n'
                        )
                        if block["content"].strip():
                            testfile.write("    " + block["content"])
                        else:
                            testfile.write("// ~ empty code block ~")
                        testfile.write("\n}\n\n")
                        total_blocks += 1

            testfile.write("\n}")  # Close namespace
    except IOError as e:
        print(f"Could not write to test file: {e}")

    if total_blocks == 0:
        print("No code blocks found in any files.")
    else:
        print(f"Total code blocks found: {total_blocks}")


########################################################################
# MAIN
#########################################################################


def main():
    args = __parse_args()
    process_folder(args.folder_path, args.recursive)


if __name__ == "__main__":
    main()
