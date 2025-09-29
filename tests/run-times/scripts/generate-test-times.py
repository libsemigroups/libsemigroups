#!/usr/bin/env python3
"""Run hyperfine on the given tests using the specified parameters"""

import argparse
import datetime
import git
import os
import platform
import re
import subprocess
import shutil
import sys

###########################################################
# CLI arguments
###########################################################

parser = argparse.ArgumentParser(
    description="""
A script for timing libsemigroups tests.
The output of this script is written to results/<DATE>-<COMMIT_HASH>-<ARCHITECTURE>-<TEST_COMMAND>.json
""",
    epilog=f"""
example:
{os.path.basename(__file__)} --max=5 ./test_knuth_bendix "[quick]"
""",
    formatter_class=argparse.RawDescriptionHelpFormatter,
)

parser.add_argument(
    "test_executable",
    type=str,
    help="The path to the test executable that hyperfine should benchmark",
)

parser.add_argument(
    "tags",
    type=str,
    default="",
    help="The tags that dictate which tests should be run. If no tags are specified, then all tests are run.",
)

parser.add_argument(
    "--max",
    type=int,
    help="Perform at most MAX runs for each command. By default, there is no limit and hyperfine will decide.",
)

args = parser.parse_args()

############################################################
# Main program                                             #
############################################################

if not shutil.which(args.test_executable):
    if args.test_executable.startswith("./"):
        make_name = args.test_executable[2:]
    else:
        make_name = args.test_executable

    sys.exit(
        f"Can't find `{args.test_executable}`, did you forget to `make {make_name}`?"
    )

# Construct the output file
date = f"{datetime.datetime.now():%Y_%m_%d_%H_%M_%S}"
hash = git.Repo(search_parent_directories=True).git.rev_parse("HEAD", short=8)
arch = platform.machine()
command = os.path.basename(args.test_executable)
output_file = f"tests/run-times/results/{date}-{hash}-{arch}-{command}.json"

print(f"Trying to write to {output_file} . . .")

# Find the the tags of the tests to run and separate by commas. Each test has a
# unique LIBSEMIGROUPS_TEST_PREFIX of the form classname number, e.g.
# Action 000. This value is used to identify the tests.
raw_tags = subprocess.run(
    [args.test_executable, args.tags, "--list-tags"], capture_output=True
)
tags = re.findall(r"(?<=\[LIBSEMIGROUPS_TEST_PREFIX=).+? \d+(?=\])", str(raw_tags.stdout))
tag_string = ",".join(tags)

# Construct the call to hyperfine
hyperfine_args = [
    "hyperfine",
    "-L",
    "tag",
    tag_string,
    f"{args.test_executable} '[{{tag}}]'",
    "--export-json",
    output_file,
    "--sort=command",
]
if args.max:
    hyperfine_args.insert(1, f"--max-runs={args.max}")

try:
    # Run the specified tests and export to json file.
    hyperfine_result = subprocess.run(hyperfine_args)
except KeyboardInterrupt:
    print(f"KeyboardInterrupt: removing {output_file} . . .")
    os.remove(output_file)
    sys.exit(1)

if hyperfine_result.returncode == 0:
    print("SUCCESS!")
else:
    print(f"Failure whilst running {hyperfine_result.args}, ")
