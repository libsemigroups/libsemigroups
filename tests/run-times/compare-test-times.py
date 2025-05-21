#!/usr/bin/env python3
"""Compare the runtime of libsemigroups tests"""

from glob import iglob
import argparse
import pandas as pd
import json

# Define CLI arguments

parser = argparse.ArgumentParser(
    description="A tool for comparing the runtime of libsemigroups tests"
)

parser.add_argument(
    "command",
    type=str,
    help="the command for which runs should be compared",
)

parser.add_argument(
    "-n",
    "--number",
    type=int,
    help="the number of runs to compare",
)

args = parser.parse_args()

# Read in data

json_filenames = "tests/run-times/results/*.json"

# Rows will be test executables, columns will be the json file corresponding to
# one run. Input to the function should be a list of json files.
