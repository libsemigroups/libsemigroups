#!/usr/bin/env python3
"""Compare the runtime of libsemigroups tests"""

import argparse
import pandas as pd
import json

# Define some helpful function


def get_time_data(f):
    results = json.load(f)["results"]
    return {record["command"]: [record["mean"]] for record in results}


# Define CLI arguments

parser = argparse.ArgumentParser(
    description="A tool for comparing the runtime of libsemigroups tests"
)

parser.add_argument(
    "base_file",
    type=argparse.FileType("r"),
    help="file path to the test time data against which all other test times will be compared",
)

parser.add_argument(
    "comparison_files",
    type=argparse.FileType("r"),
    nargs="+",
    help="file paths to the test time data to compare against base_file",
)

args = parser.parse_args()

# Read in data
base_time_data = get_time_data(args.base_file)

for f in args.comparison_files:
    time_data = get_time_data(f)
    for command in base_time_data:
        try:
            base_time_data[command] += time_data[command]
        except KeyError as e:
            base_time_data[command].append("-")

df = pd.DataFrame.from_dict(base_time_data, orient="index")

print(df.to_markdown())
