#!/usr/bin/env python3
"""
A simple script to generate the lines:

eigeninclude_HEADERS +=

from a directory.
"""
# pylint: disable=invalid-name

import os
import sys
from pathlib import Path

from os.path import isdir, exists, join

import magic


def is_cpp_file(path):
    mime = magic.from_file(path, mime=True)
    return mime in ("text/x-c", "text/x-c++") or Path(path).suffix == ".inc"


def check_args() -> str:
    if len(sys.argv) != 2:
        sys.exit(f"Expected 1 argument (a directory), found {len(sys.argv) - 1}")

    include_dir = sys.argv[1]

    if not exists(include_dir):
        sys.exit(f"{include_dir} does not exist")
    elif not isdir(include_dir):
        sys.exit(f"{include_dir} is not a directory")
    return include_dir


def generate_content(include_dir: str) -> list[str]:
    seen = set()
    result = ""
    # TODO only use Pathlib not a mixture of Pathlib and os.walk
    for dirpath, dirnames, filenames in sorted(os.walk(include_dir)):
        path = Path(*Path(dirpath).parts[2:])
        prefix = "".join(x.lower() for x in path.parts)
        if dirpath not in seen:
            seen.add(dirpath)
            if len(path.parts) > 0:
                includedir = (
                    f"\n{prefix}includedir = $(includedir)/libsemigroups/{path}\n"
                )
        prefix += "include_HEADERS"
        sep = "="
        for filename in filenames:
            file = join(dirpath, filename)
            if is_cpp_file(file):
                result += includedir
                result += f"{prefix} {sep} {file}\n"
                sep = "+="
                includedir = ""
            # else:
            #     print(file)
    return result


def insert_into_makefile_am(text: str) -> None:
    with open("Makefile.am") as f:
        makefile_am = f.readlines()

    # TODO more robust checks
    first = makefile_am.index("if LIBSEMIGROUPS_WITH_INTERNAL_EIGEN\n")
    last = makefile_am.index("endif ## LIBSEMIGROUPS_WITH_INTERNAL_EIGEN\n")

    makefile_am = "".join(makefile_am[: first + 1]) + text + "".join(makefile_am[last:])

    return makefile_am


def main():
    include_dir = check_args()
    print(insert_into_makefile_am(generate_content(include_dir)))

    sys.exit(0)


if __name__ == "__main__":
    main()
