#!/usr/bin/env python3
import re
import os
from os import listdir
from os.path import isfile, splitext

begin_warn_col = "\033[93m"
end_warn_col = "\033[0m"


def all_cpp_files(start):
    files = []

    def dive(path):
        for entry in listdir(path):
            candidate = os.path.join(path, entry)
            if isfile(candidate) and splitext(entry)[1] == ".hpp":
                files.append(candidate)
            elif not isfile(candidate):
                dive(candidate)

    dive(start)
    files.sort()
    return files


include_path = "include"
files = all_cpp_files(include_path)

commands = {
    r"\tparam": 1,
    r"\param": 2,
    r"\return": 3,
    r"\exceptions": 4,
    r"\throws": 4,
    r"\complexity": 5,
    r"\note": 6,
    r"\warning": 7,
    r"\sa": 8,
}


def process_file(f):
    check_doc = True
    in_doc = False
    any_warnings = False
    for line_no, line in enumerate(f):
        if check_doc and "/*" in line:
            check_doc = False
        if not check_doc and "*/" in line:
            check_doc = True
        if not check_doc:
            continue

        if not in_doc and r"//!" in line:
            in_doc = True
            highest_level = 0

        if in_doc and r"//!" not in line:
            in_doc = False

        for command, level in commands.items():
            if command not in line:
                continue
            if level >= highest_level:
                highest_level = level
                break
            if in_doc:
                any_warnings = True
                print(
                    f"{begin_warn_col}warning: {command} is not in the correct place in docstring at {file}:{line_no + 1}{end_warn_col}"
                )
    return any_warnings


print("Checking docstring order in .hpp files . . .")
any_warnings = False
for file in files:
    with open(file, "r") as f:
        any_warnings |= process_file(f)

if any_warnings:
    print(f"{begin_warn_col}The correct ordering is:")
    for entity in commands.keys():
        print(f"  {entity}")
    print(f"{end_warn_col}", end="")
