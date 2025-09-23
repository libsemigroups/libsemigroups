#!/usr/bin/env python3
"""Run hyperfine on the given tests using the specified parameters"""

import argparse
import datetime
import git
import os
import platform
import re
import subprocess

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

# Construct the output file
date = f"{datetime.datetime.now():%Y_%m_%d_%H_%M_%S}"
hash = git.Repo(search_parent_directories=True).git.rev_parse("HEAD", short=8)
arch = platform.machine()
command = os.path.basename(args.test_executable)
output_file = f"tests/run-times/results/{date}-{hash}-{arch}-{command}.json"

# Find the the tags of the tests to run and separate by commas
raw_tags = subprocess.run(
    [args.test_executable, args.tags, "--list-tags"], capture_output=True
)
tags = re.findall(r"(?<=LIBSEMIGROUPS_TEST_NUM=)\d+", str(raw_tags.stdout))
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

# Run the specified tests and export to json file.
hyperfine_result = subprocess.run(hyperfine_args)

if hyperfine_result.returncode == 0:
    print(f"Writing results to {output_file} . . .")
else:
    print(f"Failure whilst running {hyperfine_result.args}")
