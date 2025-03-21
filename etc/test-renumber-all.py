#!/usr/bin/env python3
"""Renumber all tests in tests/test-*.cpp files in a duplicate free manner.

The script should be run from the libsemigroups root directory. The numbering
always starts from 0. Files are processed in lexicographic order. Within each
file, test cases are numbered sequentially in order of occurrence. Across test
files, the test case family name (e.g. "Stephen" or "ToddCoxeter") is used to
determine the number of the first test case.
"""

import re
from collections import Counter
from glob import glob


if __name__ == "__main__":
    test_file_names = glob("./tests/test-*.cpp")
    test_file_names.sort()
    test_case_only_regex = re.compile(
        r"^\s*(LIBSEMIGROUPS_TEST_CASE|LIBSEMIGROUPS_TEMPLATE_TEST_CASE)",
        flags=re.MULTILINE,
    )
    test_case_and_args_regex = re.compile(
        r'^\s*(LIBSEMIGROUPS_TEST_CASE|LIBSEMIGROUPS_TEMPLATE_TEST_CASE)\s*\(\s*"(.*?)"\s*,\s*"(\w\w\w)"',
        flags=re.MULTILINE,
    )

    smallest_free_number = Counter()
    for test_file_name in test_file_names:
        with open(test_file_name, "r") as test_file:
            content = test_file.read()
        new_content = []

        names_seen = set()
        last_end = 0
        start = 0
        # TODO(2): dry this out
        while start is not None:
            partial_match = test_case_only_regex.search(content, start)
            match = test_case_and_args_regex.search(content, start)
            if partial_match is None:
                assert match is None, match
                break

            start = partial_match.start()
            if match is None or match.start() != start:
                count = content[:start].count("\n")
                raise RuntimeError(
                    f"Badly formed test case in file {test_file_name} line {count}!"
                )

            # TODO(2): figure out a better way of doing this
            name = match.group(2).split("<")[0]
            names_seen.add(name)

            last_end = match.end(3)
            start = last_end

        current_free_number = 0
        for name in names_seen:
            current_free_number = max(smallest_free_number[name], current_free_number)

        last_end = 0
        start = 0
        while start is not None:
            match = test_case_and_args_regex.search(content, start)
            if match is None:
                break

            if current_free_number > 999:
                raise RuntimeError(f"Too many test cases in file {test_file_name}!")

            new_content.append(content[last_end : match.start(3)])

            new_content.append(str(current_free_number).zfill(3))
            current_free_number += 1

            last_end = match.end(3)
            start = last_end

        for name in names_seen:
            smallest_free_number[name] = current_free_number

        new_content.append(content[last_end:])

        new_content = "".join(new_content)
        assert len(content) == len(new_content), test_file_name
        with open(test_file_name, "w") as test_file:
            test_file.write(new_content)
    print("Smallest free numbers for each test case name:")
    for name, count in smallest_free_number.most_common():
        print(f"{name}: {count}")
