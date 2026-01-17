#!/usr/bin/env python3
"""Update all copyright years.

The script should be run from the libsemigroups root directory. Once run, all of
the copyright information in the files listed below will either have the form

Copyright (C) CREATION_YEAR-CURRENT_YEAR Author 1 (+ Author 2 ...)

for files not created this year, or

Copyright (C) CURRENT_YEAR Author 1 (+ Author 2 ...)

for files created this year.

The files edit by this script are autogen.sh, and any cpp, hpp or tpp files in:
    * benchmarks/**
    * include/libsemigroups/**
    * src/**
    * tests/**
"""

import re
from datetime import date
from glob import glob
from os.path import isfile

CYAN = "\033[93m"
GREEN = "\033[92m"
BOLD = "\033[1m"
END_DECORATION = "\033[0m"


year = date.today().year

# Match copyright for the date ranges 20XX-20YY, unless 20YY is the same as the
# current year
copyright_info_old_file = re.compile(
    rf"(copyright \(c\) 20\d\d)-(?!{year})(20\d\d|\d\d\b)", re.IGNORECASE
)

# Match copyright for the date 20XX, unless 20XX is the current year
copyright_info_new_file = re.compile(
    rf"(copyright \(c\) (?!{year})20\d\d)(?!-)", re.IGNORECASE
)

# Match any copyright and capture the author
copyright_author = re.compile(r"copyright \(c\).*20\d\d (.*)", re.IGNORECASE)

pathnames = [
    "autogen.sh",
    "benchmarks/**.[cht]pp",
    "include/libsemigroups/**.[cht]pp",
    "src/**.[cht]pp",
    "tests/**.[cht]pp",
]

authors = set()


def process_line(line):
    """Update the copyright information in a given line, and add the author to
    the list of authors if any changes are made.
    """
    new_line = line
    new_line = copyright_info_old_file.sub(rf"\1-{year}", new_line)
    new_line = copyright_info_new_file.sub(rf"\1-{year}", new_line)
    changed = line != new_line
    if changed:
        author_matches = copyright_author.search(new_line)
        authors.add(author_matches[1])
    return new_line, changed


def process_file(filename):
    """Update the copyright information in a file, and report any changes"""
    print(f"{filename + ' . . .':64}", end="")

    with open(filename, "r") as f:
        content = f.readlines()

    changed = False
    for line_number, line in enumerate(content):
        new_line, changed = process_line(line)
        if changed:
            content[line_number] = new_line
            break

    if changed:
        with open(filename, "w") as f:
            f.writelines(content)
        print(f"{CYAN}{'Changed':>16}{END_DECORATION}")
    else:
        print(f"{GREEN}{'Already correct':>16}{END_DECORATION}")


def process_path(pathname):
    """Update the copyright information of all files in a specified path."""
    print(
        f"{BOLD}Checking for Copyright year changes in {pathname} . . .{END_DECORATION}"
    )
    filenames = glob(pathname, recursive=True, include_hidden=True)
    for filename in filter(isfile, filenames):
        process_file(filename)
    print()


if __name__ == "__main__":
    for pathname in pathnames:
        process_path(pathname)
    if authors:
        print("\nThe following contributors' Copyright years have been updated:")
        for author in sorted(authors):
            print(f" * {author}")
        print(
            "Please check that these are all libsemigroups contributors. If "
            "they are not, then the script has updated an incorrect Copyright "
            "and this should be reverted."
        )
