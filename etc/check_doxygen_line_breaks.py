#!/usr/bin/env python3
"""Check for bad linebreaks within doxygen commands.

The script should be run from the libsemigroups root directory. It searches
every .hpp file in the libsemigroups directory for doxygen-style documentation
lines that end with one of the following:
    * \\ref
    * \\c
    * \\p
    * \\copydoc
"""
import re
from glob import iglob
from sys import version_info

BOLD_TEXT = "\033[1m"
YELLOW = "\033[93m"
END_COLOUR = "\033[0m"

command_with_line_break = re.compile(
    r"//!.*((?:\\ref)|(?:\\c)|(?:\\p)|(?:\\copydoc))(\s+)?$",
    re.IGNORECASE | re.MULTILINE,
)

globs = ["include/libsemigroups/*.hpp", "include/libsemigroups/detail*.hpp"]


def process_file(filename):
    """Check for doxygen commands that contain line breaks in the specified
    file.
    """
    with open(filename, "r") as f:
        check_doc = True
        for line_no, line in enumerate(f):
            if check_doc and "/*" in line:
                check_doc = False
            if not check_doc and "*/" in line:
                check_doc = True
            if not check_doc:
                continue

            command_match = command_with_line_break.search(line)
            if command_match:
                command = command_match[1]
                print(
                    f"{YELLOW}warning: {command} appears at the end of the "
                    f"line at {filename}:{line_no + 1}{END_COLOUR}"
                )


def process_path(pathname):
    """Check for doxygen commands that contain line breaks for all files a
    specified path.
    """
    print(
        BOLD_TEXT
        + f"Checking for for bad linebreaks in {pathname} . . ."
        + END_COLOUR
    )
    if version_info[1] < 11:
        files = iglob(pathname)
    else:
        files = iglob(pathname, include_hidden=True)
    for filename in files:
        process_file(filename)
    print(f"{BOLD_TEXT}Done{END_COLOUR}")


if __name__ == "__main__":
    for pathname in globs:
        process_path(pathname)
